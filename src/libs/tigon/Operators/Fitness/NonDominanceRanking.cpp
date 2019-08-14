/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/Dominance/DominanceRelation.h>
#include <tigon/Factories/DominanceRelationFactory.h>

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
    delete m_domRelation;
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
                          "ranks.\n"
                          "The default value is true.")
                , typeid(bool).hash_code());

    addProperty("IsWeakDom"
                , TString("Indicates if either weak (true) or strong (false) "
                          "dominance relation is used.\n"
                          "The default value is true.")
                , typeid(bool).hash_code());


    TP_defineIsConstrainedHandlingUsed(true);
    TP_defineIsPreferabilityUsed(true);
    TP_defineIsWeakDom(true);

    addOutputTag(Tigon::TForSelection);
    addOutputTag(Tigon::TFitness);
    addInputTag(Tigon::TMainOptimizationSet);

    m_factory = &DominanceRelationFactory::instance();
    m_domRelationType = NormalDominanceRelation;
    m_domRelation = m_factory->create(m_domRelationType);
    m_domRelation->defineWeakDom(TP_isWeakDom());
}

void NonDominanceRanking::evaluateNode()
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

    m_domRelation->defineWeakDom(TP_isWeakDom());

    // build a vector that indicates if goals are used or not
    TVector<double> setGoals;
    if(isGoalVectorUsed) {
        for(IElementSPtr g : goalVec()) {
            if(*g==g->minValue()) {
                setGoals.push_back(false);
            }
            else {
                setGoals.push_back(true);
            }
        }
    }

    // build parameter vector
    TVector<TVector<double>> parameters;
    if(isGoalVectorUsed && isThresholdVectorDefined) {

        defineRelationType(PreferabilityConstraintHandlingRelation);

        parameters.resize(5);
        parameters[0] = IElementVecToRealVec(goalVec());
        parameters[1] = setGoals;
        parameters[2] = IElementVecToRealVec(thresholdVec());
    }
    else if(!isGoalVectorUsed && isThresholdVectorDefined) {

        defineRelationType(ConstraintHandlingDominanceRelation);

        parameters.resize(3);
        parameters[0] = IElementVecToRealVec(thresholdVec());
    }
    else if(isGoalVectorUsed && !isThresholdVectorDefined) {

        defineRelationType(PreferabilityRelation);

        parameters.resize(2);
        parameters[0] = IElementVecToRealVec(goalVec());
        parameters[1] = setGoals;
        m_domRelation->defineParameters(parameters);
    } else {

        defineRelationType(NormalDominanceRelation);
    }


    int N = iSet->size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int>> dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = iSet->at(i);
        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = iSet->at(j);

            // add parameters to the dominance relation for constraint handling
            if(isGoalVectorUsed && isThresholdVectorDefined) {

                parameters[3] = a->doubleConstraintVec();
                parameters[4] = b->doubleConstraintVec();
                m_domRelation->defineParameters(parameters);
            }
            else if(!isGoalVectorUsed && isThresholdVectorDefined) {

                parameters[1] = a->doubleConstraintVec();
                parameters[2] = b->doubleConstraintVec();
                m_domRelation->defineParameters(parameters);
            }
            else {
                // no need to define parameters, or already defined
            }

            dom = m_domRelation->isBetterThan(a->doubleObjectiveVec(),
                                              b->doubleObjectiveVec());

            if(dom == true) { // a dominates b
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) { // b dominates a
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<ISet*> ranks;
    TVector<TVector<int>> iRanks = sortRanksNonDomOriginal(domCount, dominatedBy);

    // for each rank
    int r = 0;
    for(TVector<int> iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(iSet->at(pos));
        }

        // set the cost of all solution from the rank
        for(IMappingSPtr sol : rank) {
            /// \todo: the fitness value is set to the solution rank. This
            /// overrides any other fitness score that the solution might have.
            /// Although this works for Pareto-based MOEAs, it might need a
            /// different approach if used by other type of MOEAs.
            sol->defineCost(IElement(r));
        }

        ranks.push_back(new ISet(rank));

        r++;
    }

    // Append the sets to the output set
    for(auto rank : ranks) {
        appendOutputSet(rank);
    }

    // Clear the memory allocated for the sets
    // Note that this does not clear the mappings, just the sets that
    // have been created above with the new operator
    clearPointerVector(ranks);
}

void NonDominanceRanking::defineRelationType(DominanceRelationType r)
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

void NonDominanceRanking::TP_defineIsWeakDom(bool b)
{
    m_isWeakDom = b;
}

bool NonDominanceRanking::TP_isWeakDom() const
{
    return m_isWeakDom;
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

