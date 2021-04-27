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
#include <tigon/Algorithms/SMSEMOA.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Directions/DiscreteCrossover.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/IntegerMutation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/Operators/Fitness/NonDominanceRanking.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/Operators/Filtrations/SMSEMOAReduce.h>
#include <tigon/Operators/Filtrations/TruncateSets.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

SMSEMOA::SMSEMOA()
{
    initialise();
}
SMSEMOA::SMSEMOA(Representation::IPSet* ipset): IAlgorithm(ipset)
{
    initialise();
}

SMSEMOA::~SMSEMOA()
{

}

void SMSEMOA::initialise()
{
    NonDominanceRanking*       ranking = new NonDominanceRanking(this);
    SMSEMOAReduce*           selection = new SMSEMOAReduce(ranking);
    TournamentFiltrationForDirection*
                    children  = new TournamentFiltrationForDirection(selection);
    SBXCrossOver*            crossOver = new SBXCrossOver(children);
    DiscreteCrossover*         disOver = new DiscreteCrossover(crossOver);
    TruncateSets*               resize = new TruncateSets(disOver);
    PolynomialMutation*             pm = new PolynomialMutation(resize);
    IntegerMutation*                im = new IntegerMutation(pm);
    CategoricalPerturpation*     cPert = new CategoricalPerturpation(im);
    MergeForNextIteration*      newpop = new MergeForNextIteration(cPert);

    appendOperator(ranking);
    appendOperator(selection);
    appendOperator(children);
    appendOperator(crossOver);
    appendOperator(disOver);
    appendOperator(resize);
    appendOperator(pm);
    appendOperator(im);
    appendOperator(cPert);
    appendOperator(newpop);

    ranking->defineOutputTags(TStringList({Tigon::TReducePopulation}));

    children->TP_defineMappingsPerSet(2);
    children->TP_defineNumberOfMappings(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addOutputTag(Tigon::TForPerturbation);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForResize);

    crossOver->TP_defineSolutionCrossoverProbability(1.0);
    disOver->TP_defineSolutionCrossoverProbability(1.0);

    resize->TP_defineSetSize(1);

    if (this->decisionVecSize() > 0){
        pm->TP_defineVariableMutationProbability(
                    1.0/(static_cast<int>(this->decisionVecSize())));
    }
}

TString SMSEMOA::name()
{
    return TString("SMSEMOA");
}

TString SMSEMOA::description()
{
    return TString("SMS-EMOA: Multiobjective selection based on dominated hypervolume:\n"
                   "SMS-EMOA, N. Beume, B. Naujoks, and M. Emmerich\n"
                   "European Journal of Operational Research, vol. 181, 1653–1669, 2007");
}

} //namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, SMSEMOA)
