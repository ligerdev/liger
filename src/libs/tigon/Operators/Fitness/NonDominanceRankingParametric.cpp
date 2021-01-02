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
#include <tigon/Operators/Fitness/NonDominanceRankingParametric.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Operators {

NonDominanceRankingParametric::NonDominanceRankingParametric()
{
    initialise();
}

NonDominanceRankingParametric::NonDominanceRankingParametric(
        Tigon::Representation::IPSet* ipset) : NonDominanceRanking(ipset)
{
    initialise();
}

NonDominanceRankingParametric::~NonDominanceRankingParametric()
{

}

void NonDominanceRankingParametric::initialise()
{
    addProperty("NumberOfBins"
                , TString("Number of bins used to partition the parameter "
                          "space.\n"
                          "The default value is 20.")
                , getTType(int));

    TP_defineNumberOfBins(Tigon::DefaultNumberBins);
}

void NonDominanceRankingParametric::evaluateNode()
{
    clearOutputSets();
    ISet* iSet = inputSet(0);

    TVector<IElementSPtr> goals = goalVec();
    TVector<IElementSPtr>::iterator iter =
        std::find_if(goals.begin(),goals.end(),
        [](IElementSPtr g){
            tribool rs = *g==g->minValue();
            return (rs.value == false);
        });

    bool         isGoalVectorUsed = ((iter != goals.end()) &&
                                     TP_isPreferabilityUsed());
    bool isThresholdVectorDefined = ((constraintVecSize() > 0) &&
                                    TP_isConstrainedHandlingUsed());

    int nExt = numberExternalParameters();
    TVector<bool> isExternalParam = isExternalParameters();
    // build vector that stores the indices of the external parameters
    TVector<size_t> ExtParamIdx;
    ExtParamIdx.reserve(nExt);
    for(size_t i=0; i<isExternalParam.size(); i++) {
        if(isExternalParam[i]) {
            ExtParamIdx.push_back(i);
        }
    }

    // determine the unique ids of the solutions
    TVector<int> solIds;
    solIds.reserve(iSet->size());
    for(auto sol: iSet->all()) {

        TVector<double> param = sol->doubleParameterVec();

        // select only the external parameters of solution 'sol' into pSub
        TVector<double> pSub;
        pSub.reserve(nExt);
        std::transform(ExtParamIdx.begin(), ExtParamIdx.end(),
                       std::back_inserter(pSub),
                       [&param](int idx){return param[idx];});

        // determine the unique id for the external parameters of solution sol
        solIds << uniqueCellIdentificationNumber(pSub, TP_numberOfBins());
    }

    TVector<ISet*> ranks;

    if(isGoalVectorUsed && isThresholdVectorDefined) {
        // goal and constraints defined
        TVector<double> threshVec = IElementVecToRealVec(thresholdVec());
        TVector<double> goalVector = IElementVecToRealVec(goalVec());
        ranks = nonDominanceSortParametricConstraintHandling(iSet,
                                                             threshVec,
                                                             goalVector,
                                                             solIds);

    } else if(!isGoalVectorUsed && isThresholdVectorDefined) {
        // constraints defined
        TVector<double> threshVec = IElementVecToRealVec(thresholdVec());
        ranks = nonDominanceSortParametricConstraintHandling(iSet, threshVec,
                                                             solIds);

    } else if(isGoalVectorUsed && !isThresholdVectorDefined) {
        // goal is defined but not constraints
        TVector<double> goalVector = IElementVecToRealVec(goalVec());
        ranks = nonDominanceSortParametric(iSet, goalVector, solIds);

    } else {
        // no goal or constraints defined
        ranks = nonDominanceSortParametric(iSet, solIds);
    }

    ISet* oSet;

    for(size_t r=0; r<ranks.size(); r++) {
        ISet* cRank = ranks[r];
        oSet = appendOutputSet(cRank);

        for(int idx=0; idx<oSet->size(); idx++) {
            oSet->at(idx)->defineCost(IElement((int)r));
        }
    }
    clearPointerVector(ranks);
}

void NonDominanceRankingParametric::TP_defineNumberOfBins(int b)
{
    m_numberOfBins = b;
}

int NonDominanceRankingParametric::TP_numberOfBins() const
{
    return m_numberOfBins;
}

TString NonDominanceRankingParametric::name()
{
    return TString("Non-dominance Ranking Parametric Optimization");
}

TString NonDominanceRankingParametric::description()
{
    return TString("Sort the input set into ranks of non-dominance, "
                   "and adds the rank of each IMapping to its cost.\n"
                   "This operator is specific for parametric multiobjective "
                   "problems, where the Pareto optimal solutions are functions "
                   "from the parameter space to the decision space (and "
                   "objective space)\n"
                   "If constraints are defined, the infeasible solutions "
                   "have an higher rank than all the feasible solutions. "
                   "Ranks betwen the infeasible solutions is determined by "
                   "their infeasibility score.\n"
                   "When a goal vector is specified, the preferability "
                   "operator is used to rank the solutions.\n"
                   "When constraints and a goal vector have been specified, "
                   "the preferability operator is only applied to feasible "
                   "solutions. This means that solutions are initially "
                   "ranked based on their infeasibility, and only feasible "
                   "solutions are ranked based on preferability.");
}

} // namespace Operators
} // namespace Tigon
