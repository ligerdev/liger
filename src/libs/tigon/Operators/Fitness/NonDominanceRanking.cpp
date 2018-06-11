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
#include <tigon/Operators/Fitness/NonDominanceRanking.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Operators {

NonDominanceRanking::NonDominanceRanking()
{
    initialise();
}

NonDominanceRanking::NonDominanceRanking(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    initialise();
}

NonDominanceRanking::~NonDominanceRanking()
{

}

void NonDominanceRanking::initialise()
{
    clearOutputTags();
    clearAdditionalOutputTags();

    addProperty("IsConstrainedHandlingUsed"
                , TString("If set to true then the ranks of dominance are "
                          "influenced by the degree of infeasibility of the "
                          "solutions. Otherwise the constraints have no "
                          "influence in the ranks.\n"
                          "The default value is true.")
                , typeid(bool).hash_code());

    addProperty("IsPreferabilityUsed"
                , TString("If set to true then the ranks of dominance are "
                          "influenced by general concept of preferability. "
                          "Otherwise any goal vector has no influence in the "
                          "ranks."
                          "The default value is true.")
                , typeid(bool).hash_code());

    TP_defineIsConstrainedHandlingUsed(true);
    TP_defineIsPreferabilityUsed(true);

    addOutputTag(Tigon::TForSelection);
    addOutputTag(Tigon::TFitness);
    addInputTag(Tigon::TMainOptimizationSet);
}

void NonDominanceRanking::evaluateNode()
{
    clearOutputSets();
    ISet* iSet = inputSet(0);

    TVector<IElementSPtr> goals = goalVec();
    TVector<IElementSPtr>::iterator iter =
        std::find_if(goals.begin(),goals.end(),
        [](IElementSPtr g){
            return !areDoublesEqual(g->value(),Tigon::Lowest);
        });

    bool         isGoalVectorUsed = ((iter != goals.end()) &&
                                     TP_isPreferabilityUsed());
    bool isThresholdVectorDefined = ((constraintVecSize() > 0) &&
                                    TP_isConstrainedHandlingUsed());

    TVector<ISet*> ranks;

    if(isGoalVectorUsed && isThresholdVectorDefined) {
        // goal and constraints defined
        TVector<double> threshVec = IElementVecToRealVec(thresholdVec());
        TVector<double> goalVector = IElementVecToRealVec(goalVec());
        ranks = nonDominanceSortConstraintHandling(iSet, threshVec, goalVector);

    } else if(!isGoalVectorUsed && isThresholdVectorDefined) {
        // constraints defined
        TVector<double> threshVec = IElementVecToRealVec(thresholdVec());
        ranks = nonDominanceSortConstraintHandling(iSet, threshVec);

    } else if(isGoalVectorUsed && !isThresholdVectorDefined) {
        // goal is defined but not constraints
        TVector<double> goalVector = IElementVecToRealVec(goalVec());
        ranks = nonDominanceSort(iSet, goalVector);

    } else {
        // no goal or constraints defined
        ranks = nonDominanceSort(iSet);
    }

    ISet* oSet;

    for(size_t r=0; r<ranks.size(); r++) {
        ISet* cRank = ranks[r];
        oSet = appendOutputSet(cRank);

        for(int idx=0; idx<oSet->size(); idx++) {
            /// \todo: ranking sets the cost to zero, but this only works for
            /// dominance-based MOEAs
            oSet->at(idx)->defineCost(IElement((int)r));

//            double cost = oSet->at(idx)->cost() + r;
//            oSet->at(idx)->defineCost(cost);
        }
    }
    clearPointerVector(ranks);
}

void NonDominanceRanking::TP_defineIsConstrainedHandlingUsed(bool b)
{
    m_isConstrainedHandlingUsed = b;
}

bool NonDominanceRanking::TP_isConstrainedHandlingUsed() const
{
    return m_isConstrainedHandlingUsed;
}

void NonDominanceRanking::TP_defineIsPreferabilityUsed(bool b)
{
    m_isPreferabilityUsed = b;
}

bool NonDominanceRanking::TP_isPreferabilityUsed() const
{
    return m_isPreferabilityUsed;
}

TString NonDominanceRanking::name()
{
    return TString("Non-dominance Ranking");
}

TString NonDominanceRanking::description()
{
    return TString("Sort the input set into ranks of non-dominance, "
                   "and adds the rank of each IMapping to its cost.\n"
                   "The set of solutions with the lowest rank is known as "
                   "the non-dominated set.\n"
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

