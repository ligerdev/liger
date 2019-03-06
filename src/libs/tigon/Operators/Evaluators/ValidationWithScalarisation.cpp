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
#include <tigon/Operators/Evaluators/ValidationWithScalarisation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/tigonengineregistry.h>

#include <future>
using namespace std;

namespace Tigon {
namespace Operators {

ValidationWithScalarisation::ValidationWithScalarisation()
{
    initialise();
}

ValidationWithScalarisation::ValidationWithScalarisation(
        Tigon::Representation::IPSet *ipset)
    : Validation(ipset)
{
    initialise();
}

ValidationWithScalarisation::~ValidationWithScalarisation() {

}

void ValidationWithScalarisation::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    int N = TP_nEvaluations();        // number of times to sample
    int nObj = objectiveVecSize();    // number of objectives
    int nCstr = constraintVecSize();  // number of constraints
    int nUnused = unusedVecSize();    // number of ununsed ouputs

    TVector<double> weights;
    switch(TP_weightScopeApproach()) {
    case LocalWeight:
        // do nothing
        break;
    case GlobalWeight: // construct global weight
    default:

        weights = dirVec();
        if(m_currentDirVec.empty()) { // happens during first iteration
            m_currentDirVec = weights;
            m_isNewDirVec=true;
        } else {
            if(!areVectorsEqual(m_currentDirVec, weights)) {
                m_isNewDirVec=true;
                m_currentDirVec = weights;
            }
            else {
                m_isNewDirVec=false;
            }
        }

        std::transform(weights.begin(), weights.end(), weights.begin(),
                       [](double w) { return 1.0/(w+0.01); });
        toUnitVec(weights, 1.0);
        break;
    }

    while(hasNextOutputSet()) {

        ISet* oSet = nextOutputSet(); // current output set

        // select the solutions for validation
        ISet* setForValidation = new ISet();
        for(auto imap : oSet->all()) {

            switch(TP_weightScopeApproach()) {
            case LocalWeight:
                if(imap->isValidated() != SampledValidation) {
                    setForValidation->append(imap);
                }
                break;
            case GlobalWeight:
            default:
                if(m_isNewDirVec) {
                    setForValidation->append(imap);
                }
                break;
            }
        }

        // container of objectives used by the scalarisation
        TMap<int,TVector<TVector<double>>> objContainer;

        TVector<TVector<double>> objSamples(nObj, TVector<double>(N));
        TVector<TVector<double>> cstrSamples(nCstr, TVector<double>(N));
        TVector<TVector<double>> unusedSamples(nUnused, TVector<double>(N));

        int evalCost = 0;

        // for all solutions to be validated
        for(int i=0; i<setForValidation->size(); i++) {

            IMappingSPtr imap = setForValidation->at(i);

            int evaluationCost=0;

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
            objContainer[i] = objSamples;

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

        /// *************
        /// Scalarization
        /// *************

        TVector<double> ideal = IElementVecToRealVec(idealVec());
        TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());
        TVector<double> costSamples(N);

        for(int i=0; i<setForValidation->size(); i++) {

            IMappingSPtr imap = setForValidation->at(i);
            TVector<TVector<double>> objSample = objContainer.at(i);

            for(int s=0; s<N; s++) { // for each evalution sample

                // retrieve the objective vector of the sth sample
                TVector<double> cObj(nObj);
                for(int k=0; k<nObj; k++) {
                    cObj[k] = objSample[k][s];
                }

                TVector<double> normObjs = normaliseToUnitBox(cObj, ideal,
                                                              antiIdeal);

                switch(TP_weightScopeApproach()) {
                case LocalWeight: // construct local weight
                    weights = imap->weightingVec();
                    std::transform(weights.begin(),weights.end(),weights.begin(),
                                   [](double w) { return 1.0/(w+0.01); });
                    toUnitVec(weights, 1.0);
                    break;
                case GlobalWeight:
                default:
                    // do nothing
                    break;
                }

                double cost;
                switch(TP_scalarisingFunction()) {
                case (Tigon::WeightedSum):
                    cost = weightedSum(weights, normObjs);
                    break;
                case (Tigon::WeightedChebyshev):
                    cost = weightedChebyshev(weights, normObjs);
                    break;
                case (Tigon::WeightedChebyshevAugmented):
                    cost = weightedChebyshevAugmented(weights, normObjs);
                    break;
                case (Tigon::WeightedLp):
                    cost = weightedLp(weights, normObjs, pNorm());
                    break;
                default:
                    throw TException(this->className(),
                                     UnrecognisedScalarisationFunction);
                }
                costSamples[s] = cost;
            }

            SampledDistribution* dist;

            dist = new SampledDistribution(costSamples);
            imap->cost()->defineDist(dist);
        }

        decrementBudget(evalCost);
    }
}

void ValidationWithScalarisation::TP_defineScalarisingFunction(
        Tigon::ScalarisationType t, double p)
{
    m_scalarisingFunc = t;
    if(p > 0.0) {
        m_pNorm = p;
    } else {
        m_pNorm = 1.0;
    }
}

Tigon::ScalarisationType
ValidationWithScalarisation::TP_scalarisingFunction() const
{
    return m_scalarisingFunc;
}

void ValidationWithScalarisation::TP_defineIsGeneralisedDecomposition(bool b)
{
    m_isGeneralisedDecomposition=b;
}

bool ValidationWithScalarisation::TP_isGeneralisedDecomposition() const
{
    return m_isGeneralisedDecomposition;
}

void ValidationWithScalarisation::TP_defineWeightScopeApproach(WeightScope s)
{
    m_weightScopeApproach = s;
}

Tigon::WeightScope ValidationWithScalarisation::TP_weightScopeApproach() const
{
    return m_weightScopeApproach;
}

double ValidationWithScalarisation::pNorm() const
{
    return m_pNorm;
}

TString ValidationWithScalarisation::name()
{
    return TString("Validation with Scalarisation");
}

TString ValidationWithScalarisation::description()
{
    return TString("Evaluates every IMapping flagged for validation N times.\n"
                   "The multiple evaluations are used to construct a sampled "
                   "distributed for each objective, constraint and ununsed "
                   "ouput.\n"
                   "A sample distribution is also constructed for the "
                   "scalarisation of the objective vectors.");
}

void ValidationWithScalarisation::initialise()
{
    addProperty("IsGeneralisedDecomposition"
                , TString("Flag that indicates if the weight vectors follow the"
                          " principles of generalised decomposition or if the"
                          " original weight vectors are to be used instead.\n"
                          "The default is false\n")
                ,getType(bool));

    TP_defineIsGeneralisedDecomposition(true);
    TP_defineScalarisingFunction(Tigon::DefaultScalarisationFunction);
    TP_defineWeightScopeApproach(Tigon::GlobalWeight);
}

} // namespace Operators
} // namespace Tigon

REGISTER_IPSET_FACTORY(Operators, ValidationWithScalarisation)
