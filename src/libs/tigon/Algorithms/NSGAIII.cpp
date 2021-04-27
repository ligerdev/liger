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
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Algorithms/NSGAIII.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Fitness/NonDominanceRanking.h>
#include <tigon/Operators/Fitness/NSGAIIINiching.h>
#include <tigon/Operators/Filtrations/NSGAIIEliteSelection.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Directions/DiscreteCrossover.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/IntegerMutation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

NSGAIII::NSGAIII()
{
    initialise();
}

NSGAIII::NSGAIII(Representation::IPSet* ipset): IAlgorithm(ipset)
{
    initialise();
}

NSGAIII::~NSGAIII()
{

}

void NSGAIII::initialise()
{
    NonDominanceRanking*  ranking = new NonDominanceRanking(this);
    NSGAIIINiching*       niching = new NSGAIIINiching(ranking);
    NSGAIIEliteSelection*   elite = new NSGAIIEliteSelection(niching);
    TournamentFiltrationForDirection*
                         children = new TournamentFiltrationForDirection(elite);
    SBXCrossOver*       crossOver = new SBXCrossOver(children);
    DiscreteCrossover*    disOver = new DiscreteCrossover(crossOver);
    PolynomialMutation*        pm = new PolynomialMutation(disOver);
    IntegerMutation*           im = new IntegerMutation(pm);
    CategoricalPerturpation* cPert = new CategoricalPerturpation(im);
    MergeForNextIteration*    nxt = new MergeForNextIteration(cPert);

    appendOperator(ranking);
    appendOperator(niching);
    appendOperator(elite);
    appendOperator(children);
    appendOperator(crossOver);
    appendOperator(disOver);
    appendOperator(pm);
    appendOperator(im);
    appendOperator(cPert);
    appendOperator(nxt);

    // This bit modifies the flow of information in a manner the children are
    // chosen from the elite population rather then the entire previous
    // genereation

    // Add outputTag TForSelection because TournamentFiltrationForDirection
    // looks for TForSelection as an input set
    elite->addOutputTag(Tigon::TForSelection);
    // To ensure that TournamentFiltrationForDirection only finds the Elite set
    elite->addOutputTag(Tigon::TForModification);
    children->TP_defineMappingsPerSet(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addInputTag(Tigon::TForModification);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForPerturbation);
}

TString NSGAIII::name()
{
    return TString("NSGA-III");
}

TString NSGAIII::description()
{
    return TString("An evolutionary many-objective optimization algorithm using "
                   "reference-point-based nondominated sorting approach, Part I: "
                   "Solving problems with box constraints, K. Deb and H. Jain, "
                   "IEEE Transactions on Evolutionary Computation, vol. 18, "
                   "no. 4, 577-601, 2014.");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, NSGAIII)
