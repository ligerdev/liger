/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Fitness/ConstraintPenalty.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Operators {


ConstraintPenalty::ConstraintPenalty()
{
    initialise();
}

ConstraintPenalty::ConstraintPenalty(Tigon::Representation::IPSet* ipset)
    : IFitness(ipset)
{
    initialise();
}

ConstraintPenalty::~ConstraintPenalty()
{

}

void ConstraintPenalty::evaluateNode()

{
    int nConstraints = constraintVecSize();
    if(nConstraints < 1) {
        return;
    }

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();

        int sSize = oSet->size();

        /// Evaluate normalised total constraint violation of every solution

        // calculate constraint violation of every solution
        // and maximum violation of each constraint
        TVector<TVector<double> > cViolation(sSize, TVector<double>(nConstraints));
        TVector<double> threshVec  = IElementVecToRealVec(thresholdVec());
        TVector<double> maxCViolation(nConstraints, 0.0);


        for(int i = 0; i < sSize; i++) {
            cViolation[i] = oSet->at(i)->doubleConstraintVec();
            for(int j = 0; j < nConstraints; j++) {
                cViolation[i][j] = std::max(cViolation[i][j] - threshVec[j], 0.0);
                if(cViolation[i][j] > maxCViolation[j]) {
                    maxCViolation[j] = cViolation[i][j];
                }
            }
        }

        // ignore constraints that are not violated by any solution
        TVector<bool> isViolated(nConstraints, false);
        bool allFeasible = true;
        for(int j = 0; j < nConstraints; j++) {
            if(maxCViolation[j] > Tigon::Epsilon) {
                isViolated[j] = true;
                allFeasible = false;
            }
        }

        // if all solutions are feasible - continue to next output set
        if(allFeasible) {
            continue;
        }

        // normalise and sum the constraint violation vector
        TVector<double> infeasibility(sSize, 0.0);
        for(int j = 0; j < nConstraints; j++) {
            if(isViolated[j]) {
                for(int i = 0; i < sSize; i++) {
                    infeasibility[i] += (cViolation[i][j] / maxCViolation[j]);
                }
            }
        }
        for(int i = 0; i < sSize; i++) {
            infeasibility[i] /= nConstraints;
        }

        /// Distinguish feasible from infeasible solutions
        TVector<bool> feasibleInd(sSize, false);
        int nFeasible = 0;
        for(int i = 0; i < sSize; i++) {
            if(infeasibility[i] < Tigon::Epsilon) {
                feasibleInd[i] = true;
                nFeasible++;
            }
        }

        /// Apply first penalty - cost of all infeasible solution with lower
        /// cost from the 'best' solution becomes the cost of the 'best' solution.

        // Find cost of 'best' solution and bounds for cost and infeasibility
        double bestCost(Highest), minInfeas(Highest);

        // Best solution is the infeasible solution with the lowest cost.
        // If no feasible solutions exsist, it is the least infeasible solution.
        if(nFeasible > 0) {
            minInfeas = 0.0;
            for(int i = 0; i < sSize; i++) {
                if(feasibleInd[i] && (oSet->at(i)->doubleCost() < bestCost)) {
                    bestCost = oSet->at(i)->doubleCost();
                }
            }
        } else {
            int bestInd(0);
            for(int i = 0; i < sSize; i++) {
                if(infeasibility[i] < minInfeas) {
                    minInfeas = infeasibility[i];
                    bestInd = i;
                }
            }
            bestCost = oSet->at(bestInd)->doubleCost();
        }

        // Other bounds
        double minCost(Highest), maxCost(Lowest), maxInfeas(Lowest);
        for(int i = 0; i < sSize; i++) {
            double iCost = oSet->at(i)->doubleCost();
            if(iCost < minCost) {
                minCost = iCost;
            }
            if(iCost > maxCost) {
                maxCost = iCost;
            }
            if(!(feasibleInd[i])) {
                if(infeasibility[i] > maxInfeas) {
                    maxInfeas = infeasibility[i];
                }
            }
        }

        // Ensure the cost of the 'best' solution is the minimum
        for(int i = 0; i < sSize; i++) {
            if(!feasibleInd[i] && oSet->at(i)->doubleCost() < bestCost) {
                oSet->at(i)->defineCost(bestCost);
            }
        }

        /// Apply second penalty according to cost and constraints violation
        // adaptive scaling factor
        double gamma;
        if(maxCost > bestCost) {
            gamma = maxCost - bestCost;
        } else {
            gamma = maxCost - minCost;
        }

        double denom = std::exp(2.0) - 1.0;
        for(int i = 0; i < sSize; i++) {
            if(!feasibleInd[i]) {
                double normCost = (oSet->at(i)->doubleCost() - minCost) /
                        (maxCost - minCost);
                double normInfeas = (infeasibility[i] - minInfeas) /
                        (maxInfeas - minInfeas);
                double newCost = oSet->at(i)->doubleCost() +
                        (std::exp(2.0 * (normCost + normInfeas) - 1.0) / denom);
                oSet->at(i)->defineCost(newCost);
            }
        }
    }
}

TString ConstraintPenalty::name()
{
    return TString("Constraint Penalty");
}

TString ConstraintPenalty::description()
{
    return TString("Penalises the cost function of infeasible solutions, "
                   "according to the degree of infeasibility and the cost "
                   "function of the entire population.\n"
                   "A modified version of the method on \"Fermani R. and "
                   "Wright J., Self-adaptive fitness formulation for constraint "
                   "optimization,\nIEEE Trans. Evol. Comput., vol. 7, no. 5, 2003.\"\n"
                   "Penalty is applied in two stages. At the first, cost of all "
                   "infeasible solutions that are better than the \'best\' "
                   "solution.\nThe best solution is the feasible solution with "
                   "lowest cost function.\nIf no feasible solutions exist in "
                   "the population, the best solution is the one with least "
                   "constraints violation.\nOn the second stage, all infeasible "
                   "solutions are further penalised according to their original cost function "
                   "and the degree of constraint violation.");
}

void ConstraintPenalty::initialise()
{
    clearAdditionalOutputTags();
    defineOutputTags(TStringList({Tigon::TFitness}));
}

} // namespace Operators
} // namespace Tigon
