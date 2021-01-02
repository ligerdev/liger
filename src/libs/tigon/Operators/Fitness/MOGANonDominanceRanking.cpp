/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Fitness/MOGANonDominanceRanking.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Mappings/IMappingOperations.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/Dominance/DominanceRelation.h>
#include <tigon/Factories/DominanceRelationFactory.h>

namespace Tigon {
namespace Operators {

MOGANonDominanceRanking::MOGANonDominanceRanking()
{
    initialise();
}

MOGANonDominanceRanking::MOGANonDominanceRanking(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    initialise();
}

MOGANonDominanceRanking::~MOGANonDominanceRanking()
{

}

void MOGANonDominanceRanking::initialise()
{
    clearOutputTags();
    clearAdditionalOutputTags();

    addProperty("IsConstrainedHandlingUsed"
                , TString("If set to true then the ranks of dominance are "
                          "influenced by the degree of infeasibility of the "
                          "solutions. Otherwise the constraints have no "
                          "influence in the ranks.\n"
                          "The default value is true.")
                , getTType(bool));

    addProperty("IsPreferabilityUsed"
                , TString("If set to true then the ranks of dominance are "
                          "influenced by general concept of preferability. "
                          "Otherwise any goal vector has no influence in the "
                          "ranks.\n"
                          "The default value is true.")
                , getTType(bool));

    addProperty("IsWeakDom"
                , TString("Indicates if either weak (true) or strong (false) "
                          "dominance relation is used.\n"
                          "The default value is true.")
                , getTType(bool));

    TP_defineIsConstrainedHandlingUsed(true);
    TP_defineIsPreferabilityUsed(true);
    TP_defineIsWeakDom(true);

    addOutputTag(Tigon::TFitness);
    addOutputTag(Tigon::TForSelection);
    addInputTag(Tigon::TMainOptimizationSet);

    m_factory = &DominanceRelationFactory::instance();
    m_domRelationType = NormalDominanceRelation;
    m_domRelation = m_factory->create(m_domRelationType);
    m_domRelation->defineWeakDom(TP_isWeakDom());
}

void MOGANonDominanceRanking::evaluateNode()
{
    clearOutputSets();
    ISet* iSet = inputSet(0);

    bool areGoalsDefined = false;
    TVector<bool> usedGoals = setGoalVec();
    for(auto val : usedGoals ) {
        if(val == true) {
            areGoalsDefined = true;
            break;
        }
    }

    bool         isGoalVectorUsed = (areGoalsDefined &&
                                     TP_isPreferabilityUsed());
    bool isThresholdVectorDefined = ((constraintVecSize() > 0) &&
                                    TP_isConstrainedHandlingUsed());

    m_domRelation->defineWeakDom(TP_isWeakDom());

    // build a vector that indicates if goals are used or not in double format
    TVector<double> setGoals; setGoals.reserve(usedGoals.size());
    if(isGoalVectorUsed) {
        for(auto val : usedGoals) {
            setGoals.push_back(val);
        }
    }

    TVector<int> priorities = priorityVec();
    TVector<double> priority; priority.reserve(priorities.size());
    for(auto elem : priorities) {
        priority.push_back(elem);
    }

    // build parameter vector
    TVector<TVector<double>> parameters;
    if(isGoalVectorUsed && isThresholdVectorDefined) {

        defineRelationType(PreferabilityConstraintHandlingRelation);
        m_domRelation->defineGoalVec(IElementVecToRealVec(goalVec()));
        m_domRelation->defineSetGoals(setGoalVec());
        m_domRelation->definePriorityVec(priorityVec());
        m_domRelation->defineThresholdVec(IElementVecToRealVec(thresholdVec()));
    }
    else if(!isGoalVectorUsed && isThresholdVectorDefined) {

        defineRelationType(ConstraintHandlingDominanceRelation);
        m_domRelation->defineThresholdVec(IElementVecToRealVec(thresholdVec()));
    }
    else if(isGoalVectorUsed && !isThresholdVectorDefined) {

        defineRelationType(PreferabilityRelation);
        m_domRelation->defineGoalVec(IElementVecToRealVec(goalVec()));
        m_domRelation->defineSetGoals(setGoalVec());
        m_domRelation->definePriorityVec(priorityVec());
    } else {

        defineRelationType(NormalDominanceRelation);
    }

    // Determine the rank of each solution

    int N = iSet->size();
    TVector<int> domCount(N, 0);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = iSet->at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = iSet->at(j);

            // add parameters to the dominance relation for constraint handling
            if(isThresholdVectorDefined) {
                m_domRelation->defineConstraintVecA(a->doubleConstraintVec());
                m_domRelation->defineConstraintVecB(b->doubleConstraintVec());
            }

            dom = m_domRelation->isBetterThan(a->doubleObjectiveVec(),
                                              b->doubleObjectiveVec());

            if(dom == true) {
                domCount[j]++;
            } else if(dom == false) {
                domCount[i]++;
            }
        }
    }

    // Create a set for each rank of solutions
    TVector<int> sortIdxDomCount = indSort(domCount);
    TVector<IMappingSPtr> cRank;
    int prevRank = domCount[sortIdxDomCount.at(0)];
    int currentRank;
    TVector<ISet*> ranks;
    for(auto i : sortIdxDomCount) {

        currentRank = domCount.at(i);

        if(currentRank != prevRank) {
            ranks.push_back(new ISet(cRank));
            cRank.clear(); // new rank
            prevRank = currentRank;
        }

        IMappingSPtr nextSol = iSet->at(i);
        nextSol->defineCost(IElement(currentRank)); // set the rank
        cRank.push_back(nextSol);
    }
    ranks.push_back(new ISet(cRank));

    // Append the sets to the output set
    for(auto rank : ranks) {
        appendOutputSet(rank);
    }

    // Clear the memory allocated for the sets
    // Note that this does not clear the mappings, just the sets that
    // have been created above with the new operator
    clearPointerVector(ranks);
}

void MOGANonDominanceRanking::defineRelationType(DominanceRelationType r)
{
    DominanceRelation* temp = m_factory->create(r);
    if(temp != nullptr) {
        m_domRelationType = r;
        delete m_domRelation;
        m_domRelation = temp;
        m_domRelation->defineWeakDom(TP_isWeakDom());
        temp = nullptr;
    }
    else {
        // ERROR - undefined type
    }
}

void MOGANonDominanceRanking::TP_defineIsConstrainedHandlingUsed(bool b)
{
    m_isConstrainedHandlingUsed = b;
}

bool MOGANonDominanceRanking::TP_isConstrainedHandlingUsed() const
{
    return m_isConstrainedHandlingUsed;
}

void MOGANonDominanceRanking::TP_defineIsPreferabilityUsed(bool b)
{
    m_isPreferabilityUsed = b;
}

bool MOGANonDominanceRanking::TP_isPreferabilityUsed() const
{
    return m_isPreferabilityUsed;
}

void MOGANonDominanceRanking::TP_defineIsWeakDom(bool b)
{
    m_isWeakDom = b;
}

bool MOGANonDominanceRanking::TP_isWeakDom() const
{
    return m_isWeakDom;
}

TString MOGANonDominanceRanking::name()
{
    return TString("MOGA Non-dominance Ranking");
}

TString MOGANonDominanceRanking::description()
{
    TString desc("Non-dominance ranking procedure used by the MOGA algorithm. "
                 "The procedure counts the number of times a solution gets "
                 "dominated by other solutions. The higher is the number of "
                 "solutions that dominate a solution, the worse is the rank of "
                 "the solution.\n Taken from Multiple Objective Genetic "
                 "Algorithm (MOGA) from Fonseca, C. M., & Fleming, P. J. (1998). "
                 "Multiobjective optimization and multiple constraint handling "
                 "with evolutionary algorithms. I. A unified formulation. IEEE "
                 "Transactions on Systems, Man, and Cybernetics, Part A: Systems "
                 "and Humans, 28(1), 26-37.");
    return desc;
}

} // namespace Operators
} // namespace Tigon
