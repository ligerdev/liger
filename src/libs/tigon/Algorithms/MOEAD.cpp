/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Algorithms/MOEAD.h>
#include <tigon/tigon_global.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Operators/Initialisation/WeightVectorInit.h>
#include <tigon/Operators/Filtrations/NeighbourhoodFiltration.h>
#include <tigon/Operators/Fitness/Scalarization.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Operators/Filtrations/RandSetReplacement.h>
#include <tigon/Operators/Filtrations/RandFiltrationForDirection.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Directions/DiscreteCrossover.h>
#include <tigon/Operators/Filtrations/TruncateSets.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/IntegerMutation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

MOEAD::MOEAD()
{
    initialise();
}

MOEAD::MOEAD(Representation::IPSet* ipset): IAlgorithm(ipset)
{
    initialise();
}

MOEAD::~MOEAD()
{

}

/// \todo Temporary fix for MOEA/D. Need to think how to define tags when the
/// algorithm is initialised
void MOEAD::evaluateNode()
{
    ISet* mainSet = setWithTag(Tigon::TMainOptimizationSet);
    if(mainSet) {
        if(!mainSet->isTagged(Tigon::TForNeighbourhoods)) {
            tagSet(mainSet,Tigon::TForNeighbourhoods);
        }
        if(!mainSet->isTagged(Tigon::TFitness)) {
            tagSet(mainSet,Tigon::TFitness);
        }
    }
    IAlgorithm::evaluateNode();
}

void MOEAD::initialise()
{
    WeightVectorInit*           wvector          = new WeightVectorInit(this);
//    Scalarization*              scalarization    = new Scalarization(wvector); // comment for generalised decomposition
    GeneralizedDecomposition*   scalarization    = new GeneralizedDecomposition(wvector); // uncomment for generalised decomposition
    NeighbourhoodFiltration*    neighbourhood    = new NeighbourhoodFiltration(scalarization);
    MOEADNeighbourhoodUpdate*   updateOperator   = new MOEADNeighbourhoodUpdate(neighbourhood);
    RandSetReplacement*         randreplacement  = new RandSetReplacement(updateOperator);
    RandFiltrationForDirection* filtForDirection = new RandFiltrationForDirection(randreplacement);
    SBXCrossOver*               crossOver        = new SBXCrossOver(filtForDirection);
    DiscreteCrossover*          disOver          = new DiscreteCrossover(crossOver);
    TruncateSets*               truncateSets     = new TruncateSets(disOver);
    PolynomialMutation*         pm               = new PolynomialMutation(truncateSets);
    IntegerMutation*            im               = new IntegerMutation(pm);
    CategoricalPerturpation*    cPert            = new CategoricalPerturpation(im);

    appendOperator(wvector);
    appendOperator(scalarization);
    appendOperator(neighbourhood);
    appendOperator(updateOperator);
    appendOperator(randreplacement);
    appendOperator(filtForDirection);
    appendOperator(crossOver);
    appendOperator(disOver);
    appendOperator(truncateSets); // remove second child
    appendOperator(pm);
    appendOperator(im);
    appendOperator(cPert);

    wvector->TP_defineRegularisationMethod(Tigon::CentroidBasedOrder);
    wvector->TP_defineDistanceMeasure(Tigon::AngleDistance);

    Tigon::ScalarisationType scalarisationFunction = Tigon::WeightedChebyshev;
    scalarization->TP_defineScalarisingFunction(scalarisationFunction);
    scalarization->TP_defineWeightScopeApproach(Tigon::LocalWeight);
    scalarization->defineOutputTags(TStringList({Tigon::TMainOptimizationSet}));
    scalarization->addAdditionalOutputTag(Tigon::TFitness);

    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineDistanceMeasure(Tigon::EuclideanDistance);
    neighbourhood->TP_defineIsClearOutputSets(false);
    neighbourhood->addOutputTag(Tigon::TForSetReplacement);

    updateOperator->TP_defineScalarisingFunction(scalarisationFunction);
    // uncomment for generalised decomposition
    //updateOperator->TP_defineIsGeneralisedDecomposition(false);
    // comment for generalised decomposition
    updateOperator->TP_defineIsGeneralisedDecomposition(true);

    randreplacement->defineReplacementSetTags(
                TStringList({Tigon::TMainOptimizationSet}));
    randreplacement->addOutputTag(Tigon::TForSelection);

    filtForDirection->TP_defineInputSetSize(2);
    filtForDirection->addOutputTag(Tigon::TForPerturbation);
    filtForDirection->addOutputTag(Tigon::TForResize);
    filtForDirection->addOutputTag(Tigon::TForEvaluation);
    filtForDirection->addOutputTag(Tigon::TForMOEADUpdate);
    updateOperator->defineInputTags(TStringList({Tigon::TForMOEADUpdate}));
    updateOperator->defineOutputTags(filtForDirection->inputTags());

    truncateSets->TP_defineSetSize(1);
}

void MOEAD::TP_defineRegularisationMethod(Tigon::RegularisationApproach r)
{
    m_regularisationMethod = r;

    switch(m_regularisationMethod) {
    case Tigon::InitialOrder:
        static_cast<WeightVectorInit*>(operators().at(0))->
                TP_defineRegularisationMethod(Tigon::InitialOrder);
        break;
    case Tigon::CentroidBasedOrder:
        static_cast<WeightVectorInit*>(operators().at(0))->
                TP_defineRegularisationMethod(Tigon::CentroidBasedOrder);
        break;
    default:
        throw TException(this->className(),
                         UnrecognisedRegularisationMethod);
    }
}

Tigon::RegularisationApproach MOEAD::TP_regularisationMethod() const
{
    return m_regularisationMethod;
}

TString MOEAD::name()
{
    return TString("MOEA/D");
}

TString MOEAD::description()
{
    return TString("Multi-objective evolutionary algorithm based on "
                   "decomposition:\n"
                   "MOEA/D, Q. Zhang, and H. Li\n"
                   "IEEE Transactions on Evolutionary Computation, vol. 11,\n"
                   "n. 6, 712-731, 2007");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, MOEAD)
