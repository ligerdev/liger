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
#include <tigon/Algorithms/NSGAIIPSA.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Fitness/NonDominanceRanking.h>
#include <tigon/Operators/Fitness/PSACrowding.h>
#include <tigon/Operators/Filtrations/PSAEliteSelection.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Operators/Directions/SinglePointCrossOver.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Filtrations/FreezeOperatorOutput.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

NSGAIIPSA::NSGAIIPSA()
{
    initialise();
}

NSGAIIPSA::NSGAIIPSA(Representation::IPSet* ipset)
    : IAlgorithm(ipset)
{
    initialise();
}

NSGAIIPSA::~NSGAIIPSA()
{

}

void NSGAIIPSA::initialise()
{
    NonDominanceRanking*  ranking = new NonDominanceRanking(this);
    PSACrowding*         crowding = new PSACrowding(ranking);
    PSAEliteSelection*      elite = new PSAEliteSelection(crowding);
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
    elite->addOutputTag(Tigon::TForModification);
    elite->addOutputTag(Tigon::TForSelection);
    children->TP_defineMappingsPerSet(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addInputTag(Tigon::TForModification);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForPerturbation);
}

TString NSGAIIPSA::name()
{
    return TString("NSGAII-PSA");
}

TString NSGAIIPSA::description()
{
    return TString("An elitist multiobjective evolutionary algorithm.\n"
                   "Based on Salomon, S. et al. \"Psa – a New Scalable "
                   "Space Partition Based Selection Algorithm for Moeas.\"\n"
                   "EVOLVE II. Springer Berlin Heidelberg, 2013. 137-151.");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, NSGAIIPSA)

