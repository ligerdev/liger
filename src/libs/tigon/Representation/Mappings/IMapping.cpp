/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Mappings/IMappingPrivate.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Log/LogManager.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/BatchSolveRegister.h>
#include <tigon/Representation/Container/SampleVectors.h>
#include <tigon/Representation/Container/TimeSeriesContainer.h>

#include <future>
#include <set>
using namespace std;

namespace Tigon {
namespace Representation {

std::mutex IMapping::ms_mutex;

IMapping::IMapping()
    : d(new IMappingPrivate())
    , m_problem(ProblemSPtr(new Problem))
    , m_log(Log::LogManagerSPtr(new Log::LogManager()))
{

}

IMapping::IMapping(const IMapping& mapping)
    : d(new IMappingPrivate())
{
    copyMapping(mapping);
}

IMapping::IMapping(ProblemSPtr prob)
    : d(new IMappingPrivate())
    , m_log(Log::LogManagerSPtr(new Log::LogManager()))
{
    defineProblem(prob);
}

IMapping::IMapping(ProblemSPtr prob, Log::LogManagerSPtr log)
    : d(new IMappingPrivate())
    , m_log(log)
{
    defineProblem(prob);
}

IMapping::~IMapping()
{
    m_problem.reset();
    delete d;
}

IMapping &IMapping::operator=(const IMapping &mapping)
{
    // If self-assignment, return itself
    if(this != &mapping) {
        copyMapping(mapping);
    }
    return *this;
}

IMappingSPtr IMapping::clone()
{
    return IMappingSPtr(new IMapping(*this));
}

ProblemSPtr IMapping::problem() const
{
    return m_problem;
}

ProblemDefinitionStatus IMapping::defineProblem(ProblemSPtr prob)
{
    ProblemDefinitionStatus defStat = prob->isProblemDefined();
    if(defStat == UnprocessedChanges) {
        defStat = prob->processProblemDefinition();
        if(defStat != FullyDefined) {
            // Warning - invalid problem
        }
    }

    m_problem.reset();
    m_problem = prob;

    createDecisionVariables();
    createOutputVectors();
    createParameters();

    int ovecSize = d->m_ovec.size();
    if(ovecSize > 0) {
        fill(d->m_wvec, 1.0 / ovecSize, ovecSize);
        fill(d->m_isObjEvaluated, false, ovecSize);
    }

    fill(d->m_isConstEvaluated, false, d->m_cvec.size());
    fill(d->m_isFuncEvaluated, false, m_problem->functionVec().size());

    return defStat;
}

/// Input vector
TVector<IElementSPtr> IMapping::decisionVec() const
{
    return d->m_dvec;
}

IElementSPtr IMapping::decisionVar(int var) const
{
    return d->m_dvec[var];
}

TVector<double> IMapping::doubleDecisionVec() const
{
    return IElementVecToRealVec(d->m_dvec);
}

double IMapping::doubleDecisionVar(int var) const
{
    if(isInRange(var, d->m_dvec.size())) {
        return d->m_dvec[var]->value<double>();
    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
}

bool IMapping::defineDecisionVar(int idx, const IElement &val)
{
    if(isInRange(idx, d->m_dvec.size())) {
        d->m_dvec[idx]->defineValue(val);
        if(val.dist() != nullptr) {
            d->m_dvec[idx]->defineDist(val.dist()->clone());
        }

        // Update evaluation status
        for(int i=0; i<static_cast<int>(m_problem->functionVec().size()); i++) {
            if(m_problem->f2dMap()[i][idx] >= 0) {
                defineFuncEvaluated(i, false);

                TVector<int> cFs = m_problem->connectedFunctions()[i];
                for(int j : cFs) {
                    defineFuncEvaluated(j, false);
                }
            }
        }

        return true;
    } else {
        // ERROR - not in range
        throw TException(className(), DomainException);
    }
}

bool IMapping::defineDecisionVar(int idx, IElementSPtr val)
{
    if(!val) {
        // ERROR - null pointer access
        throw TException(className(), NullPointerException);
    }

    if(isInRange(idx, d->m_dvec.size())) {
        d->m_dvec[idx]->defineValue(val->value());
        if(val->dist() != nullptr) {
            d->m_dvec[idx]->defineDist(val->dist()->clone());
        }

        // Update evaluation status
        for(int i=0; i<static_cast<int>(m_problem->functionVec().size()); i++) {
            if(m_problem->f2dMap()[i][idx] >= 0) {
                defineFuncEvaluated(i, false);

                TVector<int> cFs = m_problem->connectedFunctions()[i];
                for(int j : cFs) {
                    defineFuncEvaluated(j, false);
                }
            }
        }
        return true;
    } else {
        // ERROR - not in range
        throw TException(className(), DomainException);
    }
}

bool IMapping::defineDecisionVec(const TVector<IElement> &dvec)
{
    if(d->m_dvec.size() != dvec.size()) {
        // ERROR
        throw TException(className(), DomainException);
    }

    for(size_t i=0; i<d->m_dvec.size(); i++) {
        d->m_dvec[i]->defineValue(dvec[i]);
        if(dvec[i].dist() != nullptr) {
            d->m_dvec[i]->defineDist(dvec[i].dist()->clone());
        }

    }

    // Update evaluation status
    defineEvaluated(false);
    return true;
}

bool IMapping::defineDecisionVec(const TVector<IElementSPtr> &dvec)
{
    if(d->m_dvec.size() != dvec.size()) {
        // ERROR
        throw TException(className(), DomainException);
    }

    for(size_t i=0; i<d->m_dvec.size(); i++) {
        d->m_dvec[i]->defineValue(dvec[i]->value());
        if(dvec[i]->dist() != nullptr) {
            d->m_dvec[i]->defineDist(dvec[i]->dist()->clone());
        }
    }

    // Update evaluation status
    defineEvaluated(false);
    return true;
}

/// Parameter vector
TVector<IElementSPtr> IMapping::parameterVec() const
{
    return d->m_pvec;
}

IElementSPtr IMapping::parameterVar(int var) const
{
    if(isInRange(var, d->m_pvec.size())) {
        return d->m_pvec[var];
    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
}

TVector<double> IMapping::doubleParameterVec() const
{
    return IElementVecToRealVec(d->m_pvec);
}

double IMapping::doubleParameterVar(int var) const
{
    if(isInRange(var, d->m_pvec.size())) {
        return d->m_pvec[var]->value<double>();
    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
}

bool IMapping::defineParameterVec(const TVector<IElement> &pvec)
{
    if(d->m_pvec.size() != pvec.size()) {
        // ERROR
        throw TException(className(), DomainException);
    }

    for(size_t i=0; i<d->m_pvec.size(); i++) {
        d->m_pvec[i]->defineValue(pvec[i]);
        if(pvec[i].dist() != nullptr) {
            d->m_pvec[i]->defineDist(pvec[i].dist()->clone());
        }
    }

    // Update evaluation status
    defineEvaluated(false);
    return true;
}

bool IMapping::defineParameterVec(const TVector<IElementSPtr> &pvec)
{
    if(d->m_pvec.size() != pvec.size()) {
        // ERROR
        throw TException(className(), DomainException);
    }

    for(size_t i=0; i<d->m_pvec.size(); i++) {
        d->m_pvec[i]->defineValue(pvec[i]->value());
        if(pvec[i]->dist() != nullptr) {
            d->m_pvec[i]->defineDist(pvec[i]->dist()->clone());
        }
    }

    // Update evaluation status
    defineEvaluated(false);
    return true;
}

TVector<bool> IMapping::isExternalParameters() const
{
    return m_problem->isExternalParameters();
}

int IMapping::numberExternalParameters() const
{
    return m_problem->numberExternalParameters();
}

/// Objective vector
TVector<IElementSPtr> IMapping::objectiveVec() const
{
    return d->m_ovec;
}

IElementSPtr IMapping::objectiveVar(int var) const
{
    return d->m_ovec[var];
}

TVector<double> IMapping::doubleObjectiveVec() const
{
    return IElementVecToRealVec(d->m_ovec);
}

double IMapping::doubleObjectiveVar(int var) const
{
    if(isInRange(var, d->m_ovec.size())) {
        return d->m_ovec[var]->value<double>();
    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
}

bool IMapping::defineObjectiveVar(int idx, const IElement &val)
{
    if(isInRange(idx, d->m_ovec.size())) {
        d->m_ovec[idx]->defineValue(val.value());
        if(val.dist() != nullptr) {
            d->m_ovec[idx]->defineDist(val.dist()->clone());
        }

    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
    return true;
}

bool IMapping::defineObjectiveVar(int idx, IElementSPtr val)
{
    if(!val) {
        // ERROR
        throw TException(className(), NullPointerException);
    }

    if(isInRange(idx, d->m_ovec.size())) {
        if(val->type() == d->m_ovec[idx]->type()) {
            d->m_ovec[idx]->defineValue(val->value());
            if(val->dist() != nullptr) {
                d->m_ovec[idx]->defineDist(val->dist()->clone());
            }

            return true;
        } else {
            // ERROR
            throw TException(className(), InvalidTypeException);
        }
    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
}

bool IMapping::defineObjectiveVec(const TVector<IElement> &ovec)
{
    if(ovec.size() != d->m_ovec.size()) {
        throw TException(className(), DomainException);
    }
    for(size_t i=0; i<d->m_ovec.size(); i++) {
        d->m_ovec[i]->defineValue(ovec[i]);
        if(ovec[i].dist() != nullptr) {
            d->m_ovec[i]->defineDist(ovec[i].dist()->clone());
        }
    }

    return true;
}

bool IMapping::defineObjectiveVec(const TVector<IElementSPtr> &ovec)
{
    if(ovec.size() == d->m_ovec.size()) {
        for(size_t i=0; i<d->m_ovec.size(); i++) {
            d->m_ovec[i]->defineValue(ovec[i]->value());
            if(ovec[i]->dist() != nullptr) {
                d->m_ovec[i]->defineDist(ovec[i]->dist()->clone());
            }
        }
        return true;
    } else {
        // ERROR
        throw TException(className(), DomainException);
    }
}

/// Constraints vector
TVector<IElementSPtr> IMapping::constraintVec() const
{
    return d->m_cvec;
}

IElementSPtr IMapping::constraintVar(int idx) const
{
    return d->m_cvec[idx];
}

TVector<double> IMapping::doubleConstraintVec() const
{
    return IElementVecToRealVec(d->m_cvec);
}

double IMapping::doubleConstraintVar(int idx) const
{
    if(isInRange(idx, d->m_cvec.size())) {
        return d->m_cvec[idx]->value<double>();
    } else {
        // ERROR
        return PROTOTYPE_REAL_ERROR_CODE;
    }
}

bool IMapping::defineConstraint(int idx, IElementSPtr val)
{
    if(!val) {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }

    if(isInRange(idx, d->m_cvec.size())) {
        if(val->type() == d->m_cvec[idx]->type()) {
            d->m_cvec[idx]->defineValue(val->value());
            if(val->dist() != nullptr) {
                d->m_cvec[idx]->defineDist(val->dist()->clone());
            }

            return true;
        } else {
            // ERROR
            return PROTOTYPE_BOOL_ERROR_CODE;
        }
    } else {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }
}

bool IMapping::defineConstraintVec(const TVector<IElementSPtr> &cvec)
{
    if(cvec.size() == d->m_cvec.size()) {
        for(size_t i=0; i<d->m_cvec.size(); i++) {
            d->m_cvec[i]->defineValue(cvec[i]->value());
            if(cvec[i]->dist() != nullptr) {
                d->m_cvec[i]->defineDist(cvec[i]->dist()->clone());
            }
        }
        return true;
    } else {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }
}

/// Unused vector
TVector<IElementSPtr> IMapping::unusedVec() const
{
    return d->m_uvec;
}

IElementSPtr IMapping::unusedVar(int idx) const
{
    return d->m_uvec[idx];
}

TVector<double> IMapping::doubleUnusedVec() const
{
    return IElementVecToRealVec(d->m_uvec);
}

double IMapping::doubleUnusedVar(int idx) const
{
    if(isInRange(idx, d->m_uvec.size())) {
        return d->m_uvec[idx]->value<double>();
    } else {
        // ERROR
        return PROTOTYPE_REAL_ERROR_CODE;
    }
}

bool IMapping::defineUnused(int idx, const IElement &val)
{
    if(isInRange(idx, d->m_uvec.size())) {
        d->m_uvec[idx]->defineValue(val);
        if(val.dist() != nullptr) {
            d->m_uvec[idx]->defineDist(val.dist()->clone());
        }
        return true;
    } else {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }
}

bool IMapping::defineUnused(int idx, IElementSPtr val)
{
    if(!val) {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }

    if(isInRange(idx, d->m_uvec.size())) {
        if(val->type() == d->m_uvec[idx]->type()) {
            d->m_uvec[idx]->defineValue(val->value());
            if(val->dist() != nullptr) {
                d->m_uvec[idx]->defineDist(val->dist()->clone());
            }
            return true;
        } else {
            // ERROR
            return PROTOTYPE_BOOL_ERROR_CODE;
        }
    } else {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }
}

bool IMapping::defineUnusedVec(const TVector<IElement> &uvec)
{
    if(uvec.size() != d->m_uvec.size()) {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }
    for(size_t i=0; i<d->m_uvec.size(); i++) {
        d->m_uvec[i]->defineValue(uvec[i]);
        if(uvec[i].dist() != nullptr) {
            d->m_uvec[i]->defineDist(uvec[i].dist()->clone());
        }
    }

    return true;
}

bool IMapping::defineUnusedVec(const TVector<IElementSPtr> &uvec)
{
    if(uvec.size() == d->m_uvec.size()) {
        for(size_t i=0; i<d->m_uvec.size(); i++) {
            d->m_uvec[i]->defineValue(uvec[i]->value());
            if(uvec[i]->dist() != nullptr) {
                d->m_uvec[i]->defineDist(uvec[i]->dist()->clone());
            }
        }
        return true;
    } else {
        // ERROR
        return PROTOTYPE_BOOL_ERROR_CODE;
    }
}


/// Weighting vector
TVector<double> IMapping::weightingVec() const
{
    return d->m_wvec;
}

bool IMapping::defineWeight(int idx,  double val)
{
    if(val<=0) {
        return false;
    }

    if(isInRange(idx, d->m_wvec.size())) {
        d->m_wvec[idx] = val;
    } else {
        return false;
    }

    toUnitVec(d->m_wvec, 1.0);
    d->m_isScalarised = false;
    return true;
}

bool IMapping::defineWeightingVec(const TVector<double> &wvec)
{
    if(wvec.size() == d->m_ovec.size()) {
        d->m_wvec = wvec;
        toUnitVec(d->m_wvec, 1.0);
    } else {
        return false;
    }

    d->m_isScalarised = false;
    return true;
}

/// Cost
void IMapping::defineCostType(ElementType t)
{
    d->m_cost->defineType(t);
}

IElementSPtr IMapping::cost() const
{
    return d->m_cost;
}

double IMapping::doubleCost() const
{
    return d->m_cost->value<double>();
}

void IMapping::defineCost(const IElement &c)
{
    d->m_cost->defineValue(c);
}

void IMapping::defineCost(const IElementSPtr c)
{
    if(!c) {
        return;
    }

    if(c->type() == d->m_cost->type()) {
        d->m_cost  = c;
    }
}

void IMapping::defineCost(double c)
{
    d->m_cost->defineValue(c);
}

/// Evaluation checks
bool IMapping::isEvaluated() const
{
    if(d->m_isFuncEvaluated.empty()) {
        return false;
    }

    for(auto isFuncEvaluated : d->m_isFuncEvaluated) {
        if(!isFuncEvaluated) {
            return false;
        }
    }
    return true;
}

bool IMapping::isObjectiveVecEvaluated() const
{
    if(d->m_isObjEvaluated.empty()) {
        return true;
    } else {
        for(auto isObjEvaluated : d->m_isObjEvaluated) {
            if(!isObjEvaluated) {
                return false;
            }
        }
        return true;
    }
}

bool IMapping::isObjectiveEvaluated(int idx) const
{
    if(isInRange(idx, d->m_isObjEvaluated.size())) {
        return d->m_isObjEvaluated[idx];
    }
    return false;
}

bool IMapping::isConstraintVecEvaluated() const
{
    if(d->m_isConstEvaluated.empty()) {
        return true;
    } else {
        for (bool flag : d->m_isConstEvaluated) {
            if(!flag) {
                return false;
            }
        }
        return true;
    }
}

bool IMapping::isConstraintEvaluated(int idx) const
{
    if(isInRange(idx, d->m_isConstEvaluated.size())) {
        return d->m_isConstEvaluated[idx];
    }
    return false;
}

ValidationStatus IMapping::isValidated() const
{
    return d->m_isValidated;
}

bool IMapping::isSuccessfulEval() const
{
    return d->m_isSuccessfulEval;
}

void IMapping::defineErrorFlag(bool flag)
{
    d->m_isSuccessfulEval = !flag;
}

/// \todo Does not account for cases where the a solution is generated
/// over the constraint since this implies that constraint==threshold
bool IMapping::isFeasible() const
{
    TVector<IElementSPtr> thresholds = m_problem->thresholdVector();
    for(int i = 0; i < static_cast<int>(thresholds.size()); i++) {
        if(*constraintVar(i) > *thresholds[i]) {
            return false;
        }
    }
    return true;
}

bool IMapping::isPertinent() const
{
    TVector<IElementSPtr> goals = m_problem->goalVector();
    TVector<bool> isGoalUsed = m_problem->setGoalVector();
    for(size_t i = 0; i < goals.size(); i++) {
        if(isGoalUsed[i] && (*objectiveVec()[i] > *goals[i]) ) {
            return false;
        }
    }
    return true;
}

void IMapping::defineEvaluated(bool status)
{
    std::fill(d->m_isObjEvaluated.begin(), d->m_isObjEvaluated.end(), status);
    std::fill(d->m_isConstEvaluated.begin(), d->m_isConstEvaluated.end(), status);
    std::fill(d->m_isFuncEvaluated.begin(), d->m_isFuncEvaluated.end(), status);
}

void IMapping::defineObjectiveVecEvaluated(bool status)
{
    std::fill(d->m_isObjEvaluated.begin(), d->m_isObjEvaluated.end(), status);
}

void IMapping::defineObjectiveEvaluated(int idx, bool status)
{
    if(isInRange(idx, d->m_isObjEvaluated.size())) {
        d->m_isObjEvaluated[idx] = status;
    } else {
        // ERROR
    }
}

void IMapping::defineConstraintVecEvaluated(bool status)
{
    std::fill(d->m_isConstEvaluated.begin(), d->m_isConstEvaluated.end(), status);
}

void IMapping::defineConstraintEvaluated(int idx, bool status)
{
    if(isInRange(idx, d->m_isConstEvaluated.size())) {
        d->m_isConstEvaluated[idx] = status;
    } else {
        // ERROR
    }
}

void IMapping::defineValidated(ValidationStatus status)
{
    d->m_isValidated = status;
}

/// Scalarization checks
bool IMapping::isScalarised() const
{
    return d->m_isScalarised;
}

void IMapping::defineScalarised(bool status)
{
    d->m_isScalarised = status;
}

/// Run number
void IMapping::defineRunNumber(int runNumber)
{
    d->m_runNumber = runNumber;
}

/// Private methods
TVector<IElementSPtr> IMapping::fetchInputs(int funcIdx)
{
    IFunctionSPtr func = m_problem->functionVec()[funcIdx];
    TVector<IElementSPtr> inputs = createIElementSPtrVector(func->inputPrpts());
    TVector<bool> inputsOK(inputs.size(), false);
    BoxConstraintsDataSPtr box = m_problem->boxConstraints();

    // Decision variables
    TVector<int> f2dMap = m_problem->f2dMap()[funcIdx];
    for(size_t i=0; i<d->m_dvec.size(); i++) {
        int idx = f2dMap[i];
        // Use a sample of the uncertain decision variable if the mapping is defined
        if(isInRange(idx, inputs.size()) && !inputsOK[idx]) {
            if(m_problem->dVecUncertainties().at(i) == nullptr) {
                inputs[idx]->defineValue(d->m_dvec[i]->value());
            } else {
                m_problem->dVecUncertainties().at(i)->
                        evaluateUncertainty(d->m_dvec[i]);
                //inputs[idx]->defineValue(d->m_dvec[j]->sample());

                /// \note A sample drawn from the random variable can be out of bounds.
                /// This can happen if the distribution of an uncertain variable does
                /// not take into account the variable bounds.
                /// To address this, check if the decision variable is within bounds
                /// If not, truncate into feasible design

                ms_mutex.lock();
                double dvalue = d->m_dvec[i]->sample();
                ms_mutex.unlock();
                IElement ub = box->upperBound(i);
                IElement lb = box->lowerBound(i);

                dvalue = truncateValue(dvalue, d->m_dvec[i]->type(), ub, lb);
                inputs[idx]->defineValue(dvalue);
            }
            inputsOK[idx] = true;
        } else {
            //throw TException(this->className(),RangeException);
        }
    }

    // Parameters
    TVector<int> f2pMap = m_problem->f2pMap()[funcIdx];
    for(size_t i=0; i<d->m_pvec.size(); i++) {
        int idx = f2pMap[i];

        if(isInRange(idx, inputs.size()) && !inputsOK[idx]) {
            if(m_problem->isExternalParameters()[i]) {
                double val = d->m_pvec[i]->value();

                // When fetching inputs for connected parameters, account for
                // the fact that connected outputs might have been negated due
                // to maximization (affects objectives and constraints only).
                if(m_problem->paramConnectIsConnected(i)) {
                    if(m_problem->paramConnectIsMaximization(i)) {
                        val = -val;
                    }
                }

                inputs[idx]->defineValue(val);
            }
            else {
                // The sample function is not thread safe. All IMappings share
                // the same pointer to the distribution of a parameter. When
                // the function is called for the first time, the pdf (or cdf)
                // is calculated. Multiple concurrent calls could lead to
                // unwanted behaviour. This locks the call to a single thread.
                ms_mutex.lock();
                double val = d->m_pvec[i]->sample();
                ms_mutex.unlock();

                /// \todo this code is only temporary
                /// there should be box constraints defined separately
                /// for variables and parameters
                IElement lb = func->boxConstraints()->lowerBound(idx);
                IElement ub = func->boxConstraints()->upperBound(idx);
                val = truncateValue(val, d->m_pvec[i]->type(), ub, lb);

                inputs[idx]->defineValue(val);
            }

            inputsOK[idx] = true;
        } else {
            //throw TException(this->className(),RangeException);
        }
    }

    // Make sure all inputs are there
    for(auto input : inputsOK) {
        if(!input) {
            return TVector<IElementSPtr>(); // ERROR
        }
    }

    return inputs;
}

DirectedElements IMapping::fetchOutputs(int funcIdx)
{
    IFunctionSPtr func = m_problem->functionVec()[funcIdx];
    TVector<IElementSPtr>     elements(func->TP_nOutputs());
    TVector<OptimizationType> optTypes(func->TP_nOutputs(), Minimization);
    TVector<bool> outputsOK(elements.size(), false);

    for(size_t i=0; i<d->m_ovec.size(); i++) {
        int idx = m_problem->f2oMap()[funcIdx][i];
        if(isInRange(idx, elements.size()) && !outputsOK[idx]) {
            elements[idx] = d->m_ovec[i];
            optTypes[idx] = m_problem->oPrpts()[i].optimizationType();
            outputsOK[idx] = true;
        }
    }
    for(size_t i=0; i<d->m_cvec.size(); i++) {
        int idx = m_problem->f2cMap()[funcIdx][i];
        if(isInRange(idx, elements.size()) && !outputsOK[idx]) {
            elements[idx] = d->m_cvec[i];
            optTypes[idx] = m_problem->cPrpts()[i].optimizationType();
            outputsOK[idx] = true;
        }
    }

    for(size_t i=0; i<d->m_uvec.size(); i++) {
        int idx = m_problem->f2uMap()[funcIdx][i];
        if(isInRange(idx, elements.size()) && !outputsOK[idx]) {
            elements[idx] = d->m_uvec[i];
            optTypes[idx] = NonOptimization;;
            outputsOK[idx] = true;
        }
    }

    // Make sure all outputs are there
    // If an output is unused, place an unused IElementSPtr.
    // TODO: consider storing the unused elements in the problem to avoid
    //       repeatedly creating and distructing IElements (may cause problems
    //       in parallel evaluations)
    /*
    for(size_t j=0; j<outputsOK.size(); j++) {
        if(!outputsOK[j]) {
            elements[j].reset(new IElement(func->outputPrpts().at(j).type()));
            optTypes[j] = NonOptimization;
        }
    }
    */

    DirectedElements outputs;
    outputs.elems = elements;
    outputs.optTypes = optTypes;

    return outputs;
}

/*
 * PRE: number of inputs > 0
 *      number of outputs > 0
 *
 * POS: update the outputs
 */
int IMapping::evaluate()
{
    int evalCount = 0;

    int nfunctions = m_problem->functionVec().size();

    for(int i=0; i<nfunctions; i++) {
        if(!d->m_isFuncEvaluated[i]) {
            IFunctionSPtr func = m_problem->functionVec()[i];

            // Fetch the inputs and outputs
            TVector<IElementSPtr> inputs = fetchInputs(i);
            if(inputs.empty()) {
                throw TException(className(), InputOutputSizeMissMatch);
            }
            DirectedElements outputs = fetchOutputs(i);
            if(outputs.elems.empty()) {
                throw TException(className(), InputOutputSizeMissMatch);
            }

            // Evaluate the function (update the outputs)
            func->evaluate(inputs, outputs.elems);
            if(log()->isLogEvaluation()) {
                log()->logEvaluation(func, inputs, outputs.elems);
            }

            applyEvaluationUncertainty(i, outputs.elems);
            ///\todo
            //            if(log()->isLogOutputPerturbation()) {
            //                log()->logOutputPerturbation(func, output, pertOutput);
            //            }

            // Negate the maximization outputs
            for(size_t j = 0; j < outputs.elems.size(); j++) {
                if(outputs.optTypes[j] == Maximization) {
                    outputs.elems[j]->negate();
                }
            }

            // Update evaluation status
            defineFuncEvaluated(i, true);
            d->m_isScalarised = false;

            evalCount++;
        }
    }
    return evalCount;
}

int IMapping::parallelEvaluate()
{
    int evalCount = 0;

    /// \todo add interwoven sysetem handling for parallel evaluate
    /// \todo add output bounds handling for parallel evaluate
    TVector<future<void> > evalThreads;
    TVector<IFunctionSPtr> funcThreadMap; // for logging
    TVector<int> funcIndThreadMap; // funcs to threads to changed isEvaluated and upply evaluation uncertainty
    TVector<DirectedElements> funcOutThreadMap; // to add evaluation uncertainty
    TVector<TVector<IElementSPtr> > funcInThreadMap;  // and to log the evaluation

    for(int i=0; i<static_cast<int>(m_problem->functionVec().size()); i++) {
        if(!d->m_isFuncEvaluated[i]) {
            IFunctionSPtr func = m_problem->functionVec()[i];

            // Fetch the inuts and outputs
            TVector<IElementSPtr> inputs = fetchInputs(i);
            if(inputs.empty()) {
                return -1;
            }
            DirectedElements outputs = fetchOutputs(i);
            if(outputs.elems.empty()) {
                return -1;
            }

            if(func->parallelizable()) {
                // Evaluate the function in parallel(update the outputs),
                // will wait for all the evaluations at the end.
                evalThreads.push_back(std::async(launch::async,
                                                     &IFunction::evaluate,
                                                 func.get(),
                                                     inputs, outputs.elems));
                funcIndThreadMap.push_back(i);
                funcThreadMap.push_back(func);
                funcOutThreadMap.push_back(outputs);
                funcInThreadMap.push_back(inputs);

            } else {
                // Function cannot be parallelized, block all other
                // unparallelizable Imapping functions here and serialize
                ms_mutex.lock();

                // This will block all unparallelizable function evaluations,
                // not just this specific function. If there are two blocking
                // functions that the IMapping uses, they will block each other.
                // This needs to happen because the IMappings in each thread are
                // sharing a single instance of each function with no way to
                // communicate the function that is currently being used.

                func->evaluate(inputs, outputs.elems);
                if(log()->isLogEvaluation()) {
                    log()->logEvaluation(func, inputs, outputs.elems);
                }

                applyEvaluationUncertainty(i, outputs.elems);

                // Update evaluation status (these are local)
                defineFuncEvaluated(i, true);
                evalCount++;

                ms_mutex.unlock();
            }
        }
    }

    for(size_t i = 0; i < evalThreads.size(); i++){
        evalThreads[i].wait();   // Wait for all threads to finish
        ms_mutex.lock();
        if(log()->isLogEvaluation()) {
            log()->logEvaluation(funcThreadMap[i], funcInThreadMap[i],
                                 funcOutThreadMap[i].elems);
        }
        ms_mutex.unlock();
        applyEvaluationUncertainty(funcIndThreadMap[i], funcOutThreadMap[i].elems);

        // Negate the maximization outputs
        for(size_t j = 0; j < funcOutThreadMap[i].elems.size(); j++) {
            if(funcOutThreadMap[i].optTypes[j] == Maximization) {
                funcOutThreadMap[i].elems[j]->negate();
            }
        }

        defineFuncEvaluated(funcIndThreadMap[i], true);
        evalCount++;
        //evalThreads[i].cancel(); // Make sure the thread has been killed
    }

    return evalCount;
}

void IMapping::registerForBatchEvaluation(BatchSolveRegister *batchReg)
{
    for(int i=0; i<static_cast<int>(m_problem->functionVec().size()); i++) {
        if(!d->m_isFuncEvaluated[i]) {
            IFunctionSPtr func = m_problem->functionVec()[i];

            // Fetch the inputs and outputs
            TVector<IElementSPtr> inputs = fetchInputs(i);
            if(inputs.empty()) {
                return;
            }
            DirectedElements outputs = fetchOutputs(i);
            if(outputs.elems.empty()) {
                return;
            }

            // Register the function
            batchReg->registerForBatchSolve(func, inputs, outputs.elems);
            batchReg->setOptimizationType(func, outputs.optTypes);
        }
    }
}

void IMapping::registerForBatchEvaluation(BatchSolveRegister *batchReg, int funcIdx)
{
    if(!d->m_isFuncEvaluated[funcIdx]) {
        IFunctionSPtr func = m_problem->functionVec()[funcIdx];

        // Fetch the inputs and outputs
        TVector<IElementSPtr> inputs = fetchInputs(funcIdx);
        if(inputs.empty()) {
            return;
        }
        DirectedElements outputs = fetchOutputs(funcIdx);
        if(outputs.elems.empty()) {
            return;
        }

        // Register the function
        batchReg->registerForBatchSolve(func, inputs, outputs.elems);
        batchReg->setOptimizationType(func, outputs.optTypes);
    }
}

void IMapping::defineLogManager(Log::LogManagerSPtr log)
{
    m_log = log;
}

Log::LogManagerSPtr IMapping::log() const
{
    return m_log;
}

/// Optimization / Generic mapping check
bool IMapping::isOptimizationMapping() const
{
    return d->m_isOptMap;
}

/// Protected methods
void IMapping::defineOptimizationMapping(bool status)
{
    d->m_isOptMap = status;
}

/// Distributed

void IMapping::setRunNumber(int run)
{
    d->m_runNumber = run;
}

int IMapping::runNumber() const
{
    return d->m_runNumber;
}

TString IMapping::className()
{
    return TString("Tigon::Representation::IMapping");
}

void IMapping::copyMapping(const IMapping &mapping)
{
    m_problem.reset();
    m_problem = mapping.m_problem;
    m_log.reset();
    m_log = mapping.m_log;

    d->m_isOptMap         = mapping.d->m_isOptMap;
    d->m_wvec.clear();
    d->m_wvec             = mapping.d->m_wvec;
    d->m_cost             = mapping.d->m_cost->clone();
    d->m_isFuncEvaluated  = mapping.d->m_isFuncEvaluated;
    d->m_isObjEvaluated   = mapping.d->m_isObjEvaluated;
    d->m_isConstEvaluated = mapping.d->m_isConstEvaluated;
    d->m_isScalarised     = mapping.d->m_isScalarised;
    d->m_isValidated      = mapping.d->m_isValidated;
    d->m_isSuccessfulEval = mapping.d->m_isSuccessfulEval;
    d->m_runNumber        = mapping.d->m_runNumber;

    clearVector(d->m_dvec);
    int sz = mapping.d->m_dvec.size();
    d->m_dvec.resize(sz);
    for(int i = 0; i < sz; i++) {
        d->m_dvec[i] = mapping.d->m_dvec[i]->clone();
    }

    clearVector(d->m_ovec);
    int oSz = mapping.d->m_ovec.size();
    d->m_ovec.resize(oSz);
    for(int i = 0; i < oSz; i++) {
        d->m_ovec[i] = mapping.d->m_ovec[i]->clone();
    }

    clearVector(d->m_cvec);
    sz = mapping.d->m_cvec.size();
    d->m_cvec.resize(sz);
    for(int i = 0; i < sz; i++) {
        // Consider shared objectives and constraints
        int shared(-1);
        for(int j = 0; j < oSz; j++) {
            if(mapping.d->m_cvec[i] == mapping.d->m_ovec[j]) {
                shared = j;
                break;
            }
        }
        if(shared >= 0) {
            d->m_cvec[i] = d->m_ovec[shared];
        } else {
            d->m_cvec[i] = mapping.d->m_cvec[i]->clone();
        }
    }

    clearVector(d->m_uvec);
    sz = mapping.d->m_uvec.size();
    d->m_uvec.resize(sz);
    for(int i=0; i<sz; i++) {
        d->m_uvec[i] = mapping.d->m_uvec[i]->clone();
    }

    clearVector(d->m_pvec);
    sz = mapping.d->m_pvec.size();
    d->m_pvec.resize(sz);
    for(int i = 0; i < sz; i++) {

        if(m_problem->isExternalParameters()[i]) {
            if(m_problem->paramConnectIsConnected(i)) {
                size_t idx = m_problem->paramConnectOutputIdx(i);
                switch(m_problem->paramConnectOutputType(i)) {
                case OutputType::Objective:
                    d->m_pvec[i] = d->m_ovec[idx];
                    break;
                case OutputType::Constraint:
                    d->m_pvec[i] = d->m_cvec[idx];
                    break;
                case OutputType::Unused:
                    d->m_pvec[i] = d->m_uvec[idx];
                    break;
                }
            }
        }
        else {
            d->m_pvec[i] = mapping.d->m_pvec[i]->clone();
        }
    }
}
/*!
 * \brief IMapping::createDecisionVariables
 * Constructs a vector of IElementSPtr for the decision variables in the centre
 * point of the feasible domain.
 */
void IMapping::createDecisionVariables()
{
    if(m_problem->isProblemDefined() == FullyDefined) {
        d->m_dvec.resize(m_problem->dPrpts().size());
        BoxConstraintsDataSPtr box = m_problem->boxConstraints();
        for(int i=0; i<static_cast<int>(d->m_dvec.size()); i++) {
            double initVal((box->lowerBound(i).value() +
                            box->upperBound(i).value()) / 2.0);
            d->m_dvec[i] = IElementSPtr(new IElement(
                             m_problem->dPrpts().at(i).type(), initVal));
        }
    } else {
        d->m_dvec = createIElementSPtrVector(m_problem->dPrpts());
    }
}

void IMapping::createParameters()
{
    if(m_problem->isProblemDefined() == FullyDefined) {

        d->m_pvec.resize(m_problem->pPrpts().size());

        for(size_t i=0; i<m_problem->pPrpts().size(); i++) {

            if(m_problem->isExternalParameters()[i]) {
                if(m_problem->paramConnectIsConnected(i)) {
                    size_t idx = m_problem->paramConnectOutputIdx(i);
                    switch(m_problem->paramConnectOutputType(i)) {
                    case OutputType::Objective:
                        d->m_pvec[i] = d->m_ovec[idx];
                        break;
                    case OutputType::Constraint:
                        d->m_pvec[i] = d->m_cvec[idx];
                        break;
                    case OutputType::Unused:
                        d->m_pvec[i] = d->m_uvec[idx];
                        break;
                    }
                }
            }
            else {
                d->m_pvec[i] = m_problem->parameterVector()[i]->clone();
            }
        }
        //        d->m_pvec.clear();
        //        for(const IElementSPtr &ptr : m_problem->parameterVector()) {
        //            d->m_pvec.push_back(ptr->clone());
        //        }
    }
}

/*!
 * \brief IMapping::createOutputVectors
 * Creates vectors of IElementSPtr according to the function outputs, and maps
 * them to the objectives and cosnstraints vectors. A function's output can be
 * shared between an objective and a constraint. Since the same pointer
 * describes both, the shared pointer is passed to the function for evaluation.
 */
void IMapping::createOutputVectors()
{
    if(m_problem->isProblemDefined() == FullyDefined) {
        d->m_ovec.resize(m_problem->oPrpts().size());
        d->m_cvec.resize(m_problem->cPrpts().size());
        d->m_uvec.resize(m_problem->uPrpts().size());

        for(size_t i=0; i<m_problem->functionVec().size(); i++) {
            for(int j=0; j<m_problem->functionVec().at(i)->TP_nOutputs(); j++) {
                IElementSPtr elem(new IElement(m_problem->functionVec().at(i)->
                                               outputPrpts().at(j).type()));
                int idx = vectorIndexOf(m_problem->f2oMap().at(i), j);
                if(idx >= 0) {
                    d->m_ovec[idx] = elem;
                }

                idx = vectorIndexOf(m_problem->f2cMap().at(i), j);
                if(idx >= 0) {
                    d->m_cvec[idx] = elem;
                }

                idx = vectorIndexOf(m_problem->f2uMap().at(i), j);
                if(idx >= 0) {
                    d->m_uvec[idx] = elem;
                }
            }
        }
    } else {
        d->m_ovec = createIElementSPtrVector(m_problem->oPrpts());
        d->m_cvec = createIElementSPtrVector(m_problem->cPrpts());
        d->m_uvec = createIElementSPtrVector(m_problem->uPrpts());
    }
}

void IMapping::defineFuncEvaluated(int idx, bool status)
{
    if(isInRange(idx, d->m_isFuncEvaluated.size())) {
        if(d->m_isFuncEvaluated[idx] != status) {
            d->m_isFuncEvaluated[idx] = status;

            TVector<int> omap = m_problem->f2oMap().at(idx);
            for(size_t i=0; i<omap.size(); i++) {
                if(omap[i] >= 0) {
                    d->m_isObjEvaluated[i] = status;
                }
            }

            TVector<int> cmap = m_problem->f2cMap().at(idx);
            for(size_t i=0; i<cmap.size(); i++) {
                if(cmap[i] >= 0) {
                    d->m_isConstEvaluated[i] = status;
                }
            }
        }
    } else {
        // ERROR
    }
}

void IMapping::applyEvaluationUncertainty(int funcIdx, TVector<IElementSPtr> funcOutputs)
{
    TVector<UncertaintyMapping*> umaps =
            m_problem->funcOutUncertainties().at(funcIdx);
    IElementSPtr uncertainOutput(new IElement(RealType));
    for(size_t i=0; i<funcOutputs.size(); i++) {
        if(umaps[i] != nullptr) {
            uncertainOutput->defineValue(funcOutputs[i]->value());
            umaps[i]->evaluateUncertainty(uncertainOutput);
            funcOutputs[i]->defineValue(uncertainOutput->sample());
        }
    }
}

} // namespace Representation
} // namespace Tigon
