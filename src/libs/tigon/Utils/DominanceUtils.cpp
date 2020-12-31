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
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Mappings/IMappingOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {

tribool weakDominance(const TVector<double> &a, const TVector<double> &b)
{
    if(a.size() != b.size()) {
        // ERROR
        return incomparable;
    }

    bool aDominate(false);
    bool bDominate(false);
    for(size_t i=0; i<a.size(); i++)
    {
        if(a[i] < b[i]) {
            if(bDominate) {
                return incomparable;
            }
            aDominate = true;
        } else if(b[i] < a[i]) {
            if(aDominate) {
                return incomparable;
            }
            bDominate = true;
        }
    }
    if(aDominate) {
        return true;
    } else if(bDominate) {
        return false;
    } else {
        return incomparable;
    }
}

tribool strongDominance(const TVector<double> &a, const TVector<double> &b)
{
    if(a.size() != b.size()) {
        // ERROR
        return incomparable;
    }

    bool aDominate(false);
    bool bDominate(false);
    for(int i=0; i<a.size(); i++)
    {
        if(a[i] == b[i]) {
            return incomparable;
        } else if(a[i] < b[i]) {
            if(bDominate) {
                return incomparable;
            }
            aDominate = true;
        } else {
            if(aDominate) {
                return incomparable;
            }
            bDominate = true;
        }
    }
    if(aDominate) {
        return true;
    } else if(bDominate) {
        return false;
    } else {
        return incomparable;
    }
}

tribool epsilonDominance(const TVector<double> &a, const TVector<double> &b,
                         double epsilon)
{
    int asize, bsize;

    asize = a.size();
    bsize = b.size();

    if(asize != bsize) {
        // ERROR
        return incomparable;
    }

    bool aDominate(false);
    bool bDominate(false);

    for(int i=0; i<asize; i++)
    {
        if((a[i]+epsilon) < b[i]) {
            if(bDominate) {
                return incomparable;
            }
            aDominate = true;
        } else if((b[i]+epsilon) < a[i]) {
            if(aDominate) {
                return incomparable;
            }
            bDominate = true;
        }
    }

    if(aDominate) {
        return true;
    } else if(bDominate) {
        return false;
    } else {
        return incomparable;
    }
}


TVector<int> dominanceCount(const ISet* set, bool weakDom)
{
    return dominanceCount(set->all(), weakDom);
}

TVector<int> dominanceCount(const TVector<IMappingSPtr>& set, bool weakDom)
{
    int N = set.size();
    TVector<int> domCount(N, 0);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            // normal domination
            if(weakDom){
                dom = *a <= *b;
            } else {
                dom = *a < *b;
            }

            if(dom == true) {
                domCount[j]++;
            } else if(dom == false) {
                domCount[i]++;
            }
        }
    }

    return domCount;
}

TVector<ISet*> nonDominanceSort(const ISet* set, bool weakDom)
{
    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSort(IMappings, weakDom);
    TVector<ISet*> ranks;
    ranks.reserve(ranksVec.size());

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }

    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSort(
        const TVector<IMappingSPtr>& set, bool weakDom)
{
    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int>> dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            // normal domination
            if(weakDom){
                dom = *a <= *b;
            } else {
                dom = *a < *b;
            }

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks =
            sortRanksNonDomOriginal(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

TVector<ISet*> nonDominanceSortParametric(const ISet* set,
                                          const TVector<int>& solIds,
                                          bool weakDom)
{
    if(set->size() != solIds.size()) {
        return TVector<ISet*>{};
    }
    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSortParametric(IMappings, solIds, weakDom);
    TVector<ISet*> ranks;
    ranks.reserve(ranksVec.size());

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }

    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSortParametric(
        const TVector<IMappingSPtr>& set, const TVector<int>& solIds,
        bool weakDom)
{
    if(solIds.size()!=set.size()) {
        return TVector<TVector<IMappingSPtr>>{};
    }

    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int>> dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            if(solIds[i]!=solIds[j]) {
                // solutions are incomparable if their ids are different
                dom = incomparable;
            } else {
                // normal domination
                if(weakDom){
                    dom = *a <= *b;
                } else {
                    dom = *a < *b;
                }
            }

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks =
            sortRanksNonDomOriginal(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

ISet* nonDominatedSet(const ISet* set, bool weakDom)
{
    ISet* ret = new ISet(nonDominatedSet(set->all(), weakDom));
    return ret;
}

TVector<IMappingSPtr> nonDominatedSet(const TVector<IMappingSPtr> &set,
                                      bool weakDom)
{
    int ia = 0;
    tribool dom;

    TVector<IMappingSPtr> res = set;
    while(ia < res.size()-1){
        int ib = ia+1;
        while(ib < res.size()) {
            if(weakDom){
                dom = *res.at(ia) <= *res.at(ib);
            } else {
                dom = *res.at(ia) < *res.at(ib);
            }
            if(dom.value == tribool::true_value) {
                res.erase(res.begin()+ib);
            } else if(dom.value == tribool::false_value) {
                res.erase(res.begin()+ia);
                break;
            } else {
                ib++;
            }
        }
        if(dom.value != tribool::false_value) {
            ia++;
        }
    }
    return res;
}

TVector<TVector<int>> sortRanksNonDomOriginal(TVector<int> domCount,
                                      const TVector<TVector<int>>& dominatedBy)
{
    // Sort into ranks of non-dominance
    int N = domCount.size();
    TVector<TVector<int>> ranks;
    int count = 0;
    int noOfRanks = -1;
    while(count < N) {
        // iterate once to add non-dominated solutions to the rank
        TVector<int> rank;
        for(int i=0; i<N; i++) {
            if(domCount.at(i)==0) {
                count++;
                rank.push_back(i);
                //eliminate non-dominated solutions by setting their count to -1
                domCount[i] = noOfRanks;
            }
        }

        // iterate again to update the counters
        for(int i=0; i<N; i++) {
            if(domCount.at(i) == noOfRanks) {
                // decrement the count of the dominated solutions
                for(auto dI : dominatedBy.at(i)) {
                    domCount[dI]--;
                }
            }
        }

        ranks.push_back(rank);
        noOfRanks--;
    }

    return ranks;
}

TVector<TVector<int>> sortRanksNonDom(TVector<int> domCount,
                                      const TVector<TVector<int>>& dominatedBy)
{
    // Sort into ranks of non-dominance
    int N = domCount.size();
    TVector<TVector<int>> ranks;
    int count = 0;
    int noOfRanks = -1;
    int minVal=0;
    while(count < N) {
        // iterate once to add non-dominated solutions to the rank
        TVector<int> rank;
        for(int i=0; i<N; i++) {
            if(domCount.at(i)==minVal) {
                count++;
                rank.push_back(i);
                //eliminate non-dominated solutions by setting their count to -1
                domCount[i] = noOfRanks;
            }
        }

        // iterate again to update the counters
        for(int i=0; i<N; i++) {
            if(domCount.at(i) == noOfRanks) {
                // decrement the count of the dominated solutions
                for(auto dI : dominatedBy.at(i)) {
                    domCount[dI]--;
                }
            }
        }

        // this accounts for the fact that some rank numbers might be missing
        minVal=vectorMax(domCount);
        for(auto dC : domCount) {
            if( (dC>-1) && (dC<minVal) ) {
                minVal = dC;
            }
        }

        ranks.push_back(rank);
        noOfRanks--;
    }

    return ranks;
}

/// *******************
/// Constraint handling
/// *******************

/*
 * Determines the constraint violation score of a solution based on the
 * difference between the constraint values and the input threshold vector.
 * */
double solutionConstraintViolation(IMappingSPtr x,
                                   const TVector<IElementSPtr>& threshVec)
{
    TVector<IElementSPtr> constr = x->constraintVec();

    double constrViolation = 0.0;
    if(!x->isFeasible()) { // not feasible solution

        TVector<double> r;
        r.reserve(constr.size());

        // determines the difference between the solution constraint and
        // the constraint violation threshold
        std::transform(constr.begin(), constr.end(),
                       threshVec.begin(), std::back_inserter(r),
                       [](IElementSPtr a, IElementSPtr b)
        {return a->value<double>() - b->value<double>(); });

        for(auto e : r) {
            if(e>0.0) {
                constrViolation += e;
            }
        }
    }

    return constrViolation;
}

/*
 * Determines the constraint violation score of a solution based on the
 * difference between the constraint values and the input threshold vector.
 * */
double solutionConstraintViolation(IMappingSPtr x,
                                   const TVector<double>& threshVec)
{
    TVector<double> constr = x->doubleConstraintVec();

    double constrViolation = 0.0;
    if(!x->isFeasible()) { // not feasible solution

        TVector<double> r;
        r.reserve(constr.size());

        // determines the difference between the solution constraint and
        // the constraint violation threshold
        std::transform(constr.begin(), constr.end(),
                       threshVec.begin(), std::back_inserter(r),
                       std::minus<double>());

        for(auto e : r) {
            if(e>0.0) constrViolation += e;
        }
    }

    return constrViolation;
}

TVector<ISet*> nonDominanceSortConstraintHandling(
        const ISet* set,
        const TVector<double> &threshVec,
        bool weakDom)
{
    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSortConstraintHandling(IMappings, threshVec, weakDom);
    TVector<ISet*> ranks;

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }
    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSortConstraintHandling(
        const TVector<IMappingSPtr> &set,
        const TVector<double> &threshVec,
        bool weakDom)
{
    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int> > dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            // both infeasible
            if( (!a->isFeasible()) && (!b->isFeasible()) ) {

                /// calculating the constraint violation for 'a'
                double constrViolationA =
                        solutionConstraintViolation(a, threshVec);
                /// calculating the constraint violation for 'b'
                double constrViolationB =
                        solutionConstraintViolation(b, threshVec);

                // a violates more than b
                if(constrViolationA > constrViolationB) {
                    dom = false; // b dominates
                }
                else {
                    // b violates more than a
                    if(constrViolationA < constrViolationB) {
                        dom = true; // a dominates
                    }
                    else {
                        // non-dominated since both equally violate
                        dom = incomparable;
                    }
                }
            }
            else {

                // a violates but b doesn't
                if( (!a->isFeasible()) && (b->isFeasible()) ) {
                    dom = false; // b dominates
                }
                else {
                    // b violates but a doesn't
                    if( (a->isFeasible()) && (!b->isFeasible()) ) {
                        dom = true; // a dominates
                    }
                    else {

                        // normal domination
                        if(weakDom){
                            dom = *a <= *b;
                        } else {
                            dom = *a < *b;
                        }
                    }
                }
            }


            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks =
            sortRanksNonDomOriginal(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

TVector<ISet*> nonDominanceSortParametricConstraintHandling(
        const ISet* set,
        const TVector<double> &threshVec,
        const TVector<int>& solIds,
        bool weakDom)
{
    if(set->size()!=solIds.size()) {
        return TVector<ISet*>{};
    }

    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSortParametricConstraintHandling(IMappings, threshVec,
                                                         solIds, weakDom);
    TVector<ISet*> ranks;

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }
    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSortParametricConstraintHandling(
        const TVector<IMappingSPtr> &set, const TVector<double> &threshVec,
        const TVector<int>& solIds, bool weakDom)
{
    if(solIds.size()!=set.size()) {
        return TVector<TVector<IMappingSPtr>>{};
    }

    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int> > dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            // both infeasible
            if( (!a->isFeasible()) && (!b->isFeasible()) ) {

                /// calculating the constraint violation for 'a'
                double constrViolationA =
                        solutionConstraintViolation(a, threshVec);
                /// calculating the constraint violation for 'b'
                double constrViolationB =
                        solutionConstraintViolation(b, threshVec);

                // a violates more than b
                if(constrViolationA > constrViolationB) {
                    dom = false; // b dominates
                }
                else {
                    // b violates more than a
                    if(constrViolationA < constrViolationB) {
                        dom = true; // a dominates
                    }
                    else {
                        // non-dominated since both equally violate
                        dom = incomparable;
                    }
                }
            }
            else {

                // a violates but b doesn't
                if( (!a->isFeasible()) && (b->isFeasible()) ) {
                    dom = false; // b dominates
                }
                else {
                    // b violates but a doesn't
                    if( (a->isFeasible()) && (!b->isFeasible()) ) {
                        dom = true; // a dominates
                    }
                    else {

                        if(solIds[i]!=solIds[j]) {
                            // solutions are incomparable
                            // if their ids are different
                            dom = incomparable;
                        } else {
                            // normal domination
                            if(weakDom){
                                dom = *a <= *b;
                            } else {
                                dom = *a < *b;
                            }
                        }
                    }
                }
            }

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks =
            sortRanksNonDomOriginal(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

/// ***********
/// Goal vector
/// ***********

LIGER_TIGON_EXPORT
tribool preferability(const TVector<double> &a,
                      const TVector<double> &b,
                      const TVector<double> &g,
                      bool weakDom)
{
    size_t Nobj = a.size();

    /// Classification of the solutions
    /// 0: solution meets all goals
    /// 1: solution does not meet all the goals
    int aclass=0;
    int bclass=0;

    for(size_t k=0; k<Nobj; k++) {
        if(!areDoublesEqual(g[k],Tigon::Lowest)) { // only for set goals
            if(a[k] > g[k]) {
                aclass=1;
                break;
            }
        }
    }

    for(size_t k=0; k<Nobj; k++) {
        if(!areDoublesEqual(g[k],Tigon::Lowest)) { // only for set goals
            if(b[k] > g[k]) {
                bclass=1;
                break;
            }
        }
    }

    if(aclass < bclass) {
        return true;    // A is preferred
    } else if(aclass > bclass) {
        return false;   // B is preferred
    }

    int AequalB, BequalA, AbetterB, BbetterA;

    AbetterB = AequalB = BbetterA = BequalA = 1;

    for(size_t k=0; k<Nobj; k++) {
        if(!areDoublesEqual(g[k],Tigon::Lowest)) { // only for set goals
            if(a[k] > g[k]) { // a does not meet the goal
                if(weakDom) {
                    AbetterB *= (a[k] <= b[k]);
                } else {
                    AbetterB *= (a[k] < b[k]);
                }
                AequalB  *= (a[k] == b[k]);
            }
            if(b[k] > g[k]) { // b does not meet the goal
                if(weakDom) {
                    BbetterA *= (b[k] <= a[k]);
                } else {
                    BbetterA *= (b[k] < a[k]);
                }
                BequalA  *= (b[k] == a[k]);
            }
        }
    }

    // check if A and B are totally different
    if (!(AequalB && BequalA)) {

        // check if A dominates B with respect to the components that A
        // does not meet the goal
        if ((AbetterB && !AequalB) || (AequalB && !BequalA))
            return true;

        // check if B dominates A with respect to the components that B
        // does not meet the goal
        if ((BbetterA && !BequalA) || (BequalA && !AequalB))
            return false;

        return incomparable;
    }

    /* this point is only reached in case the decision is  *
     * based on those objectives which satisfy their goals */

    AbetterB = AequalB = BbetterA = 1;
    for (size_t k=0; k<Nobj; k++) {
        if(weakDom) {
            AbetterB *= (a[k] <= b[k]);
            BbetterA *= (b[k] <= a[k]);
        } else {
            AbetterB *= (a[k] < b[k]);
            BbetterA *= (b[k] < a[k]);
        }
        AequalB  *= (a[k] == b[k]);
    }

    // check if A dominates B with respect to the components that A
    // meets the goal
    if (AbetterB && !AequalB)
        return true;

    // check if B dominates A with respect to the components that B
    // meets the goal
    if (BbetterA && !AequalB)
        return false;

    return incomparable;
}

LIGER_TIGON_EXPORT
tribool preferability(const TVector<double> &a,
                      const TVector<double> &b,
                      const TVector<double> &g,
                      const TVector<int> &p,
                      bool weakDom)
{
    int maxElem = *min_element(p.begin(), p.end());
    int minElem = *max_element(p.begin(), p.end());
    for(size_t i=0; i<p.size(); i++) {
        if(!areDoublesEqual(g[i],Tigon::Lowest)) { // only for set goals
            if(p[i] > maxElem) {
                maxElem = p[i];
            }
            if(p[i] < minElem) {
                minElem = p[i];
            }
        }
    }

    if ((maxElem==minElem) && (maxElem==1)){ // if there is only one priority level
        return preferability(a,b,g);
    }
    else { // if there is more than one priority level

        // Store the indices of the objectives with the highest priority
        TVector<size_t> mp_idx;
        for(size_t i = 0; i<p.size(); i++) {
            if(!areDoublesEqual(g[i],Tigon::Lowest)) { // only for set goals
                if(p[i]==maxElem) {
                    mp_idx.push_back(i);
                }
            }
        }

        int aclass=0;
        for(auto idx : mp_idx) {
            if(!areDoublesEqual(g[idx],Tigon::Lowest)) { // only for set goals
                if(a[idx] > g[idx]) {
                    aclass=1;
                    break;
                }
            }
        }

        int bclass=0;
        for(auto idx : mp_idx) {
            if(!areDoublesEqual(g[idx],Tigon::Lowest)) { // only for set goals
                if(b[idx] > g[idx]) {
                    bclass=1;
                    break;
                }
            }
        }

        if(aclass < bclass) {
            return true;    // A is preferred
        } else if(aclass > bclass) {
            return false;   // B is preferred
        }

        // it reaches this point if both solutions have objectives that violate
        // the goals, or both totally satisfy the goals

        int AequalB, BequalA, AbetterB, BbetterA;
        AbetterB = AequalB = BbetterA = BequalA = 1;

        for(auto idx : mp_idx) {
            if(!areDoublesEqual(g[idx],Tigon::Lowest)) { // only for set goals
                if(a[idx] > g[idx]) { // A does not meet the goal
                    if(weakDom) {
                        AbetterB *= (a[idx] <= b[idx]);
                    } else {
                        AbetterB *= (a[idx] < b[idx]);
                    }
                    AequalB  *= (a[idx] == b[idx]);
                }
                if(b[idx] > g[idx]) { // B does not meet the goal
                    if(weakDom) {
                        BbetterA *= (b[idx] <= a[idx]);
                    } else {
                        BbetterA *= (b[idx] < a[idx]);
                    }
                    BequalA  *= (b[idx] == a[idx]);
                }
            }
        }

        // check if A and B are totally different
        if (!(AequalB && BequalA)) {

            // check if A dominates B with respect to the components that A
            // does not meet the goal
            if ((AbetterB && !AequalB) || (AequalB && !BequalA))
                return true;

            // check if B dominates A with respect to the components that B
            // does not meet the goal
            if ((BbetterA && !BequalA) || (BequalA && !AequalB))
                return false;

            return incomparable;
        }

        if( (maxElem == minElem) && (maxElem>1)) {
            // Constraint satisfaction special case
            return incomparable;
        }

        /* this point is only reached in case the decision is *
                * based on objectives with lower priority levels */

        size_t Nobj = a.size();

        TVector<double> a2;
        TVector<double> b2;
        TVector<double> g2;
        TVector<int> p2;

        // Select objectives and corresponding goals with priority level lower
        // than the current maximum
        for(size_t k=0; k<Nobj; k++) {
            if(p[k] < maxElem) {
                a2.push_back(a[k]);
                b2.push_back(b[k]);
                g2.push_back(g[k]);
                p2.push_back(p[k]);
            }
        }

        return preferability(a2,b2,g2,p2);
    }
}

TVector<ISet*> nonDominanceSort(const ISet* set, const TVector<double>& goal,
                                bool weakDom)
{
    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSort(IMappings, goal, weakDom);
    TVector<ISet*> ranks;
    ranks.reserve(ranksVec.size());

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }

    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSort(
        const TVector<IMappingSPtr>& set, const TVector<double>& goal,
        bool weakDom)
{
    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int>> dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            dom = preferability(a->doubleObjectiveVec(),
                                b->doubleObjectiveVec(), goal, weakDom);

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks = sortRanksNonDom(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

TVector<ISet*> nonDominanceSortParametric(const ISet* set,
                                          const TVector<double>& goal,
                                          const TVector<int>& solIds,
                                          bool weakDom)
{
    if(set->size() != solIds.size()) {
        return TVector<ISet*>{};
    }

    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSortParametric(IMappings, goal, solIds, weakDom);
    TVector<ISet*> ranks;
    ranks.reserve(ranksVec.size());

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }

    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSortParametric(
        const TVector<IMappingSPtr>& set, const TVector<double>& goal,
        const TVector<int>& solIds, bool weakDom)
{
    if(solIds.size()!=set.size()) {
        return TVector<TVector<IMappingSPtr>>{};
    }

    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int>> dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            if(solIds[i]!=solIds[j]) {
                // solutions are incomparable if their ids are different
                dom = incomparable;
            } else {
                dom = preferability(a->doubleObjectiveVec(),
                                    b->doubleObjectiveVec(), goal, weakDom);
            }

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks = sortRanksNonDom(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

TVector<ISet*> nonDominanceSortConstraintHandling(
        const ISet* set, const TVector<double> &threshVec,
        const TVector<double> &goal, bool weakDom)
{
    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSortConstraintHandling(IMappings, threshVec, goal,
                                               weakDom);
    TVector<ISet*> ranks;

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }
    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSortConstraintHandling(
        const TVector<IMappingSPtr>& set,
        const TVector<double>& threshVec,
        const TVector<double>& goal,
        bool weakDom)
{
    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int> > dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        /// calculating the constraint violation for 'a'
        double constrViolationA = solutionConstraintViolation(a, threshVec);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            /// calculating the constraint violation for 'b'
            double constrViolationB = solutionConstraintViolation(b, threshVec);

            if( (!a->isFeasible()) && (!b->isFeasible()) ) { // both infeasible

                // a violates more than b
                if(constrViolationA > constrViolationB) {
                    dom = false; // b dominates
                }
                else {
                    // b violates more than a
                    if(constrViolationA < constrViolationB) {
                        dom = true; // a dominates
                    }
                    else {
                        // non-dominated since both equally violate
                        dom = incomparable;
                    }
                }
            }
            else {

                // a violates but b doesn't
                if( (!a->isFeasible()) && (b->isFeasible()) ) {
                    dom = false; // b dominates
                }
                else {
                    // b violates but a doesn't
                    if( (a->isFeasible()) && (!b->isFeasible()) ) {
                        dom = true; // a dominates
                    }
                    else {
                        dom = preferability(a->doubleObjectiveVec(),
                                            b->doubleObjectiveVec(),
                                            goal, weakDom);
                    }
                }
            }

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks = sortRanksNonDom(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

TVector<ISet*> nonDominanceSortParametricConstraintHandling(
        const ISet* set, const TVector<double> &threshVec,
        const TVector<double> &goal, const TVector<int>& solIds, bool weakDom)
{
    if(set->size() != solIds.size()) {
        return TVector<ISet*>{};
    }

    TVector<IMappingSPtr> IMappings = set->all();
    TVector<TVector<IMappingSPtr>> ranksVec =
            nonDominanceSortParametricConstraintHandling(IMappings,
                                                         threshVec, goal,
                                                         solIds, weakDom);
    TVector<ISet*> ranks;

    for(auto rv : ranksVec) {
        ranks.push_back(new ISet(rv));
    }
    return ranks;
}

TVector<TVector<IMappingSPtr>> nonDominanceSortParametricConstraintHandling(
        const TVector<IMappingSPtr>& set,
        const TVector<double>& threshVec,
        const TVector<double>& goal,
        const TVector<int>& solIds,
        bool weakDom)
{
    if(solIds.size()!=set.size()) {
        return TVector<TVector<IMappingSPtr>>{};
    }

    int N = set.size();
    TVector<int> domCount(N, 0);
    TVector<TVector<int> > dominatedBy(N);

    // Compare all solutions
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set.at(i);

        /// calculating the constraint violation for 'a'
        double constrViolationA = solutionConstraintViolation(a, threshVec);

        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set.at(j);

            /// calculating the constraint violation for 'b'
            double constrViolationB = solutionConstraintViolation(b, threshVec);

            if( (!a->isFeasible()) && (!b->isFeasible()) ) { // both infeasible

                // a violates more than b
                if(constrViolationA > constrViolationB) {
                    dom = false; // b dominates
                }
                else {
                    // b violates more than a
                    if(constrViolationA < constrViolationB) {
                        dom = true; // a dominates
                    }
                    else {
                        // non-dominated since both equally violate
                        dom = incomparable;
                    }
                }
            }
            else {

                // a violates but b doesn't
                if( (!a->isFeasible()) && (b->isFeasible()) ) {
                    dom = false; // b dominates
                }
                else {
                    // b violates but a doesn't
                    if( (a->isFeasible()) && (!b->isFeasible()) ) {
                        dom = true; // a dominates
                    }
                    else {

                        if(solIds[i]!=solIds[j]) {
                            // solutions are incomparable
                            // if their ids are different
                            dom = incomparable;
                        }

                        dom = preferability(a->doubleObjectiveVec(),
                                            b->doubleObjectiveVec(),
                                            goal, weakDom);
                    }
                }
            }

            if(dom == true) {
                dominatedBy[i].push_back(j);
                domCount[j]++;
            } else if(dom == false) {
                dominatedBy[j].push_back(i);
                domCount[i]++;
            }
        }
    }

    TVector<TVector<IMappingSPtr>> ranks;
    TVector<TVector<int>> iRanks = sortRanksNonDom(domCount, dominatedBy);
    for(auto iRank : iRanks) {
        TVector<IMappingSPtr> rank;
        for(auto pos : iRank) {
            rank.push_back(set.at(pos));
        }
        ranks.push_back(rank);
    }

    return ranks;
}

/// ******
/// Others
/// ******

TVector<TVector<double>> preferredSet(const TVector<TVector<double>>& refSet,
                                      const TVector<double>&         goalVec)
{
    TVector<TVector<double> > prefSet;

    if((refSet.empty()) || (goalVec.empty()) ||
            (refSet[0].size() != goalVec.size())) {
        //ERROR
        return prefSet;
    }

    // calculate the goal vector sum
    double goalVecSum = std::accumulate(goalVec.begin(), goalVec.end(), 0.0);

    if(areDoublesEqual(goalVecSum, 1.0)) {
        prefSet.push_back(goalVec);
    } else {
        prefSet = refSet;
        double scaleFactor = 1.0 - goalVecSum;
        for(int i=0; i<prefSet.size(); i++) {
            for(int j=0; j<goalVec.size(); j++) {
                prefSet[i][j] *= scaleFactor;
                prefSet[i][j] += goalVec[j];
            }
        }

        if(goalVecSum > 1.0) {

            // Project to the simplex and remove vectors with negative components
            int nNotInBounds = 0;
            TVector<bool> inBoundIdx(prefSet.size(), true);
            for(int i=0; i<prefSet.size(); i++) {
                toUnitVec(prefSet[i], 1.0);
                for(int j=0; j<goalVec.size(); j++) {
                    if(prefSet[i][j] < 0.0) {
                        nNotInBounds++;
                        inBoundIdx[i] = false;
                        break;
                    }
                }
            }
            if(nNotInBounds > 0) {
                TVector<TVector<double>> inBoundsSet(prefSet.size()-nNotInBounds);
                int c = 0;
                for(int i=0; i<prefSet.size(); i++) {
                    if(inBoundIdx[i]) {
                        inBoundsSet[c++].swap(prefSet[i]);
                    }
                }
                prefSet.swap(inBoundsSet);
            }
        }
    }

    return prefSet;
}


} // namespace Tigon
