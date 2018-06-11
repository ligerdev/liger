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
#include <tigon/Algorithms/SMSEMOA.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
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
    TruncateSets*               resize = new TruncateSets(crossOver);
    PolynomialMutation*             pm = new PolynomialMutation(resize);
    MergeForNextIteration*      newpop = new MergeForNextIteration(pm);

    appendOperator(ranking);
    appendOperator(selection);
    appendOperator(children);
    appendOperator(crossOver);
    appendOperator(resize);
    appendOperator(pm);
    appendOperator(newpop);

    ranking->defineOutputTags(TStringList({Tigon::TReducePopulation}));

    children->TP_defineMappingsPerSet(2);
    children->TP_defineNumberOfMappings(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addOutputTag(Tigon::TForPerturbation);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForResize);

    crossOver->TP_defineSolutionCrossoverProbability(1);
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
