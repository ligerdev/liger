/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include <tigon/Operators/Evaluators/Validation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/tigonengineregistry.h>

#include <future>
using namespace std;

namespace Tigon {
namespace Operators {

Validation::Validation()
{
    initialise();
}

Validation::Validation(Tigon::Representation::IPSet *ipset)
    : IEvaluator(ipset)
{
    initialise();
}

Validation::~Validation() {

}

void Validation::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    int N = TP_nEvaluations();        // number of times to sample
    int nObj = objectiveVecSize();    // number of objectives
    int nCstr = constraintVecSize();  // number of constraints
    int nUnused = unusedVecSize();    // number of ununsed ouputs

    while(hasNextOutputSet()) {

        ISet* oSet = nextOutputSet(); // current output set

        // select the solutions for validation
        ISet* setForValidation = new ISet();
        for(auto imap : oSet->all()) {
            if(imap->isValidated() != SampledValidation) {
                setForValidation->append(imap);
            }
        }

        TVector<TVector<double>> objSamples(nObj, TVector<double>(N));
        TVector<TVector<double>> cstrSamples(nCstr, TVector<double>(N));
        TVector<TVector<double>> unusedSamples(nUnused, TVector<double>(N));

        int evalCost = 0;

        // for all solutions to be validated
        for(int i=0; i<setForValidation->size(); i++) {

            IMappingSPtr imap = setForValidation->at(i);

            int evaluationCost = 0;

            bool isEvaluated = imap->isEvaluated();

            for(int s=0; s<N; s++) { // for each evalution sample
                imap->defineEvaluated(false);
                evaluationCost = imap->evaluate();

                // Objectives
                for(int k=0; k<nObj; k++) {
                    objSamples[k][s] = imap->doubleObjectiveVar(k);
                }
                // Constraints
                if(nCstr > 0) {
                    for(int k=0; k<nCstr; k++) {
                        cstrSamples[k][s] = imap->doubleConstraintVar(k);
                    }
                }
                // Unused
                if(nUnused > 0) {
                    for(int k=0; k<nUnused; k++) {
                        unusedSamples[k][s] = imap->doubleUnusedVar(k);
                    }
                }

                if(evaluationCost > 0) {
                    if(TP_isCountEvaluations()) {
                        if(TP_isCountPerFunction()) {
                            evalCost += evaluationCost;
                        } else {
                            evalCost++;
                        }
                    }

                    updateIdealNadirVec(imap);
                }
            }

            SampledDistribution* dist;

            // Objectives
            for(int j=0; j<nObj; j++) {
                dist = new SampledDistribution(objSamples[j]);
                imap->objectiveVar(j)->defineDist(dist);
            }

            // Constraints
            if(nCstr > 0) {
                for(int j=0; j<nCstr; j++) {
                    dist = new SampledDistribution(cstrSamples[j]);
                    imap->constraintVar(j)->defineDist(dist);
                }
            }

            // Unused
            if(nUnused > 0) {
                for(int j=0; j<nUnused; j++) {
                    dist = new SampledDistribution(unusedSamples[j]);
                    imap->unusedVar(j)->defineDist(dist);
                }
            }

            if(TP_defineEvaluated()) {
                imap->defineEvaluated(true);
            } else {
                imap->defineEvaluated(isEvaluated);
            }

            imap->defineValidated(SampledValidation);
        }

        decrementBudget(evalCost);
    }
}

void Validation::TP_defineNEvaluations(int n)
{
    if(n >= 0) {
        m_nEvaluations = n;
    }
}

int Validation::TP_nEvaluations() const
{
    return m_nEvaluations;
}

void Validation::TP_defineOperateOnFinal(bool o) {
    m_operateOnFinal = o;
}

bool Validation::TP_operateOnFinal() const {
    return m_operateOnFinal;
}

void Validation::TP_defineDefineEvaluated(bool o)
{
    m_defineEvaluated = o;
}

bool Validation::TP_defineEvaluated() const
{
    return m_defineEvaluated;
}

TString Validation::name()
{
    return TString("Validation");
}

TString Validation::description()
{
    return TString("Evaluates every IMapping flagged for validation N times.\n"
                   "The multiple evaluations are used to construct a sampled "
                   "distributed for each objective, constraint and ununsed "
                   "ouput.");
}

void Validation::initialise()
{
    addProperty("NEvaluations"
                , TString("Number of repeated evaluations of every "
                          "IMapping.\nDefault is ")
                , typeid(int).hash_code());
    addProperty("OperateOnFinal"
                , TString("Disable the repeated evaluations during "
                          "the optimization process, and only validate "
                          "the final solutions.")
                , typeid(bool).hash_code());
    addProperty("DefineEvaluated"
                , TString("Indicates if the solutions are to be set as "
                          "evaluated or not.")
                , typeid(bool).hash_code());

    addAdditionalOutputTag(Tigon::TValidation);

    TP_defineNEvaluations(Tigon::DefaultMonteCarloSample);
    TP_defineCountEvaluations(false);
    TP_defineOperateOnFinal(true);
    TP_defineDefineEvaluated(true);
}

} // namespace Operators
} // namespace Tigon

REGISTER_IPSET_FACTORY(Operators, Validation)



///\todo resume parallel support
//if(TP_isParallel()) {
//            TVector<future<int>> results;
//            TVector<int> resIdx;

//            for(int idx=0; idx<oSet->size(); idx++) {
//                IMappingSPtr imap = oSet->at(idx);
//                // validate only non-validated imappings
//                if(imap->isValidated() != SampledValidation) {
//                    resIdx.push_back(idx);
//                    results.push_back(std::async(launch::async,
//                                                 &IMapping::multipleEvaluate,
//                                                 imap.get(), m_nEvaluations));
//                }
//            }

//            for (int i=0; i < results.size(); i++) {
//                // Blocks until all evaluations have returned a value
//                int res = results[i].get();
//                if(res < 0) {
//                    // Error!!
//                } else if(res > 0){
//                    if(TP_isCountEvaluations()) {
//                        if(TP_isCountPerFunction()) {
//                            evalCost += res;
//                        } else {
//                            evalCost++;
//                        }
//                    }

//                    oSet->at(resIdx[i])->defineValidated(SampledValidation);
//                    // TODO: if(!TP_isOffLine) {
//                    updateIdealNadirVec(oSet->at(resIdx[i]));
//                    // }
//                }
//            }
