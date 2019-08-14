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

    return preferability(a, b);
}

tribool Preferability::preferability(const TVector<double> &a,
                                     const TVector<double> &b) const
{
    int Nobj = a.size();

    /// Classification of the solutions
    /// 0: solution meets all goals
    /// 1: solution does not meet all the goals
    int aclass=0;
    int bclass=0;

    for(int k=0; k<Nobj; k++) {
        if(setGoals()[k]) { // only for set goals
            if(a[k] > goalVec()[k]) {
                aclass=1;
                break;
            }
        }
    }

    for(int k=0; k<Nobj; k++) {
        if(setGoals()[k]) { // only for set goals
            if(b[k] > goalVec()[k]) {
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

    for(int k=0; k<Nobj; k++) {
        if(setGoals()[k]) { // only for set goals
            if(a[k] > goalVec()[k]) { // a does not meet the goal
                if(weakDom()) {
                    AbetterB *= (a[k] <= b[k]);
                } else {
                    AbetterB *= (a[k] < b[k]);
                }
                AequalB  *= (a[k] == b[k]);
            }
            if(b[k] > goalVec()[k]) { // b does not meet the goal
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
    for (int k=0; k<Nobj; k++) {
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



void Preferability::defineParameters(const TVector<TVector<double> > &c)
{
    if( c.size()==2 ) {
        defineGoalVec(c[0]);

        TVector<bool> p1;
        for(auto p : c[1]) {
            p1.push_back((bool)p);
        }

        defineSetGoals(p1);
    }
}

void Preferability::defineGoalVec(const TVector<double> &g)
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


} // namespace Representation
} // namespace Tigon
