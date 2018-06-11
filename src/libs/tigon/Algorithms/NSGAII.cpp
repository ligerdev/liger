/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Algorithms/NSGAII.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Fitness/NonDominanceRanking.h>
#include <tigon/Operators/Fitness/NSGAIICrowding.h>
#include <tigon/Operators/Filtrations/NSGAIIEliteSelection.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForModification.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

NSGAII::NSGAII()
{
    initialise();
}

NSGAII::NSGAII(Representation::IPSet* ipset): IAlgorithm(ipset)
{
    initialise();
}

NSGAII::~NSGAII()
{

}

void NSGAII::initialise()
{
    NonDominanceRanking*  ranking = new NonDominanceRanking(this);
    NSGAIICrowding*      crowding = new NSGAIICrowding(ranking);
    NSGAIIEliteSelection*   elite = new NSGAIIEliteSelection(crowding);
    TournamentFiltrationForDirection*
                         children = new TournamentFiltrationForDirection(elite);
    SBXCrossOver*       crossOver = new SBXCrossOver(children);
    PolynomialMutation*        pm = new PolynomialMutation(crossOver);
    CategoricalPerturpation* cPert = new CategoricalPerturpation(pm);
    MergeForNextIteration*    nxt = new MergeForNextIteration(cPert);

    appendOperator(ranking);
    appendOperator(crowding);
    appendOperator(elite);
    appendOperator(children);
    appendOperator(crossOver);
    appendOperator(pm);
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

TString NSGAII::name()
{
    return TString("NSGA-II");
}

TString NSGAII::description()
{
    return TString("A fast and elitist multiobjective genetic algorithm:\n"
                   "NSGA-II, K. Deb, A. Pratap, S. Agarwal, and T. Meyarivan\n"
                   "IEEE Transactions on Evolutionary Computation, vol. 6,\n"
                   "n. 2, 182-197, 2002.");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, NSGAII)
