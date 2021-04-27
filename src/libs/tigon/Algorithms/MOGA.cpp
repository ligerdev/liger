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
#include <tigon/Algorithms/MOGA.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Fitness/MOGANonDominanceRanking.h>
#include <tigon/Operators/Fitness/AverageFitness.h>
#include <tigon/Operators/Fitness/SharedFitness.h>
#include <tigon/Operators/Filtrations/StochasticUniversalSampling.h>
#include <tigon/Operators/Filtrations/RandFiltrationForDirection.h>
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Operators/Directions/DiscreteCrossover.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Perturbations/IntegerMutation.h>
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/Operators/Filtrations/FreezeOperatorOutput.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

MOGA::MOGA()
{
    initialise();
}

MOGA::MOGA(Representation::IPSet* ipset): IAlgorithm(ipset)
{
    initialise();
}

MOGA::~MOGA()
{

}

void MOGA::initialise()
{
    MOGANonDominanceRanking* rank = new MOGANonDominanceRanking(this);
    AverageFitness*        avgfit = new AverageFitness(rank);
    SharedFitness*         shafit = new SharedFitness(avgfit);
    StochasticUniversalSampling*
                            elite = new StochasticUniversalSampling(shafit);
    RandFiltrationForDirection*
                         filtered = new RandFiltrationForDirection(elite);
    SBXCrossOver*       crossOver = new SBXCrossOver(filtered);
    DiscreteCrossover*    disOver = new DiscreteCrossover(crossOver);
    PolynomialMutation*        pm = new PolynomialMutation(disOver);
    IntegerMutation*           im = new IntegerMutation(pm);
    CategoricalPerturpation* cPert = new CategoricalPerturpation(im);
    MergeForNextIteration*    nxt = new MergeForNextIteration(cPert);

    appendOperator(rank);
    appendOperator(avgfit);
    appendOperator(shafit);
    appendOperator(elite);
    appendOperator(filtered);
    appendOperator(crossOver);
    appendOperator(disOver);
    appendOperator(pm);
    appendOperator(im);
    appendOperator(cPert);
    appendOperator(nxt);

    // Make sure that AverageFitness operates on the Main Optimization Set
    avgfit->defineOutputTags(TStringList{Tigon::TMainOptimizationSet});

    // Add outputTag TForSelection because RandFiltrationForDirection
    // looks for TForSelection as an input set
    elite->defineInputTags(TStringList{Tigon::TMainOptimizationSet});
    elite->addOutputTag(Tigon::TForSelection);
    // To ensure that RandFiltrationForDirection only finds the mating pool set
    filtered->addInputTag(Tigon::TMatingPool);

    filtered->addOutputTag(Tigon::TForNextIteration);
    filtered->addOutputTag(Tigon::TForPerturbation);
}

TString MOGA::name()
{
    return TString("MOGA");
}

TString MOGA::description()
{
    return TString("A Pareto-based Multiple Objective Genetic Algorithm (MOGA) "
                   "from Fonseca, C. M., & Fleming, P. J. (1998). Multiobjective "
                   "optimization and multiple constraint handling with evolutionary "
                   "algorithms. I. A unified formulation. IEEE Transactions on "
                   "Systems, Man, and Cybernetics, Part A: Systems and Humans, "
                   "28(1), 26-37.");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, MOGA)
