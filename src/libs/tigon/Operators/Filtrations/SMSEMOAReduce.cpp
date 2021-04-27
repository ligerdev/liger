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
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Operators/Filtrations/SMSEMOAReduce.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Utils/HypervolumeUtils.h>

namespace Tigon {
namespace Operators {

SMSEMOAReduce::SMSEMOAReduce()
{
    initialise();
}

SMSEMOAReduce::SMSEMOAReduce(Tigon::Representation::IPSet *ipset)
    :IFiltration(ipset)
{
    initialise();
}

SMSEMOAReduce::~SMSEMOAReduce()
{

}

void SMSEMOAReduce::initialise()
{
    addInputTag(Tigon::TReducePopulation);
    addOutputTag(Tigon::TForNextIteration);
    addOutputTag(Tigon::TForSelection);

    isFirstIteration = true;
}

void SMSEMOAReduce::evaluateNode()
{
    if(!hasNextInputSet()) {
        return;
    }

    clearOutputSets();
    ISet* oSet= appendOutputSet();

    // In first iteration just get the MainOptimizationSet and pass it over
    if (isFirstIteration) {
        ISet* iSet = setsUnion(inputSets());
        oSet->define(iSet->all());
        isFirstIteration = false;
        return;
    }

    TVector<ISet*> ranks;
    TVector<double> rank_score;
    while(hasNextInputSet()) {
        ISet* rank = nextInputSet();
        ranks.push_back(rank);
        if(rank->size() > 0) {
            rank_score.push_back(rank->at(0)->doubleCost());
        }
    }

    // Determine the worst rank index
    int worstRankIdx;
    if(ranks.size()==1) {
        worstRankIdx = 0;
    } else {
        worstRankIdx = std::max_element(rank_score.begin(), rank_score.end()) -
                       rank_score.begin();
    }
    ISet* worstRankSet = ranks[worstRankIdx];

    // if there is at least two solutions in the worst rank set
    int worstSolutionIdx;
    if(worstRankSet->size() > 1) {

        // Determine the nadir with respect to the worst rank
        // The nadir will be used as a reference point for the hypervolume computation
        TVector<double> nadir = worstRankSet->at(0)->doubleObjectiveVec();
        for(int i=1; i<worstRankSet->size(); i++) {
            TVector<double> newNadir =  worstRankSet->at(i)->doubleObjectiveVec();
            for(int j=0; j<objectiveVecSize(); j++) {
                if(newNadir[j] > nadir[j]) {
                    nadir[j] = newNadir[j];
                }
            }
        }
        // penalise the nadir to ensure it is strongly dominated by all points
        for(int j=0; j<objectiveVecSize(); j++) {
            nadir[j] = nadir[j] + nadir[j]*0.1;
        }

        // Calculate hypervolume of the worst rank set
        double hvOrg = hypervolume(worstRankSet, nadir);

        // Remove a solution from the set and calculate hypervolume to find out
        // which contributes the least
        TVector<double> v;
        v.reserve(worstRankSet->size());
        for (int i=0; i<worstRankSet->size(); i++) {
            ISet* newSet = new ISet(worstRankSet->all());
            newSet->remove(i);
            double r = hypervolume(newSet, nadir);
            v.push_back(hvOrg - r);
            delete newSet;
        }
        worstSolutionIdx = std::min_element(v.begin(), v.end()) - v.begin();
    } else {
        worstSolutionIdx = 0;
    }

    for(int i=0; i<ranks.size(); i++) {
        if(i==worstRankIdx) {
            for(int j=0; j<worstRankSet->size(); j++) {
                if(j != worstSolutionIdx) {
                    oSet->append(worstRankSet->at(j));
                }
            }
        }
        else {
            ISet* rank = ranks[i];
            for(IMappingSPtr sol : rank->all()) {
                oSet->append(sol);
            }
        }
    }
}

TString SMSEMOAReduce::name()
{
    return TString("SMS-EMOA Reduce operator");
}
TString SMSEMOAReduce::description()
{
    return TString("Combines all input sets into a single set of solutions, and "
                   "copies all solutions to the output set apart from one. "
                   "The solution omitted is taken from the worst-ranked front as "
                   "determined by the nondominance-sort procedure from NSGA-II, "
                   "and has the worst contribution to the hypervolume measure "
                   "amonsgt all solutions found in the worst-ranked front.");
}

} // namespace Operators
} // namespace Tigon
