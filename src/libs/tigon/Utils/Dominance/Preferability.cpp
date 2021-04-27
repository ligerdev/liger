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
#include <tigon/Utils/Dominance/Preferability.h>

namespace Tigon {
namespace Representation {

Preferability::Preferability()
{

}

Preferability::~Preferability()
{

}

tribool Preferability::isBetterThan(const TVector<double> &a,
                                    const TVector<double> &b) const
{
    if( (goalVec().empty()==true) || (setGoals().empty()==true) ) {

        // Do normal dominance
        return dominates(a,b);
    }

    return preferability(a, b, goalVec(), setGoals(), priorityVec());
}

tribool Preferability::preferability(const TVector<double> &a,
                                     const TVector<double> &b,
                                     const TVector<double> &g,
                                     const TVector<bool> &s) const
{
    size_t Nobj = a.size();

    /// Classification of the solutions
    /// 0: solution meets all goals
    /// 1: solution does not meet all the goals
    int aclass=0;
    int bclass=0;

    for(size_t k=0; k<Nobj; k++) {
        if(s[k]) { // only for set goals
            if(a[k] > g[k]) {
                aclass=1;
                break;
            }
        }
    }

    for(size_t k=0; k<Nobj; k++) {
        if(s[k]) { // only for set goals
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
        if(s[k]) { // only for set goals
            if(a[k] > g[k]) { // a does not meet the goal
                if(weakDom()) {
                    AbetterB *= (a[k] <= b[k]);
                } else {
                    AbetterB *= (a[k] < b[k]);
                }
                AequalB  *= (a[k] == b[k]);
            }
            if(b[k] > g[k]) { // b does not meet the goal
                if(weakDom()) {
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
        if(weakDom()) {
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

tribool Preferability::preferability(const TVector<double> &a,
                                     const TVector<double> &b,
                                     const TVector<double> &g,
                                     const TVector<bool> &s,
                                     const TVector<int> &p) const
{
    int maxElem = *min_element(p.begin(), p.end());
    int minElem = *max_element(p.begin(), p.end());
    for(size_t i=0; i<p.size(); i++) {
        if(s[i]) { // only for set goals
            if(p[i] > maxElem) {
                maxElem = p[i];
            }
            if(p[i] < minElem) {
                minElem = p[i];
            }
        }
    }

    if ((maxElem==minElem) && (maxElem==1)){ // if there is only one priority level
        return preferability(a,b,g,s);
    }
    else { // if there is more than one priority level

         // Store the indices of the objectives with the highest priority
         TVector<size_t> mp_idx;
         for(size_t i = 0; i<p.size(); i++) {
             if(s[i]) { // only for set goals
                if(p[i]==maxElem) {
                    mp_idx.push_back(i);
                }
             }
         }

         int aclass=0;
         for(auto idx : mp_idx) {
             if(s[idx]) { // only for set goals
                 if(a[idx] > g[idx]) {
                     aclass=1;
                     break;
                 }
             }
         }

         int bclass=0;
         for(auto idx : mp_idx) {
             if(s[idx]) { // only for set goals
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

         int AequalB, BequalA, AbetterB, BbetterA;
         AbetterB = AequalB = BbetterA = BequalA = 1;

         for(auto idx : mp_idx) {
             if(s[idx]) { // only for set goals
                 if(a[idx] > g[idx]) { // A does not meet the goal
                     if(weakDom()) {
                         AbetterB *= (a[idx] <= b[idx]);
                     } else {
                         AbetterB *= (a[idx] < b[idx]);
                     }
                     AequalB  *= (a[idx] == b[idx]);
                 }
                 if(b[idx] > g[idx]) { // B does not meet the goal
                     if(weakDom()) {
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
         TVector<bool> s2;
         TVector<int> p2;

         // Select objectives and corresponding goals with priority level lower
         // than the current maximum
         for(size_t k=0; k<Nobj; k++) {
             if(p[k] < maxElem) {
                 a2.push_back(a[k]);
                 b2.push_back(b[k]);
                 g2.push_back(g[k]);
                 s2.push_back(s[k]);
                 p2.push_back(p[k]);
             }
         }

         return preferability(a2,b2,g2,s2,p2);
     }
}

void Preferability::defineGoalVec(const TVector<double>& g)
{
    m_goalVec = g;
}

TVector<double> Preferability::goalVec() const
{
    return m_goalVec;
}

void Preferability::defineSetGoals(const TVector<bool> &gSet)
{
    m_setGoals = gSet;
}

TVector<bool> Preferability::setGoals() const
{
    return m_setGoals;
}

void Preferability::definePriorityVec(const TVector<int>& p)
{
    m_priorityVec = p;
}

TVector<int> Preferability::priorityVec() const
{
    return m_priorityVec;
}

} // namespace Representation
} // namespace Tigon
