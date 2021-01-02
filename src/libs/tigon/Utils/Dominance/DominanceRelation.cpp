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
#include <tigon/Utils/Dominance/DominanceRelation.h>
#include <tigon/Utils/DominanceUtils.h>

namespace Tigon {
namespace Representation {

DominanceRelation::DominanceRelation()
    : m_weakDom(true)
{

}

DominanceRelation::~DominanceRelation()
{

}

tribool DominanceRelation::isBetterThan(const TVector<double> &a,
                                        const TVector<double> &b) const
{
    return dominates(a,b);
}

tribool DominanceRelation::dominates(const TVector<double> &a,
                                     const TVector<double> &b) const
{
    tribool dom;
    if(weakDom()){
        dom = weakDominance(a,b);
    } else {
        dom = strongDominance(a,b);
    }

    return dom;
}

void DominanceRelation::defineWeakDom(bool c)
{
    m_weakDom = c;
}

bool DominanceRelation::weakDom() const
{
    return m_weakDom;
}

void DominanceRelation::defineGoalVec(const TVector<double>& g)
{
    T_UNUSED(g);
}


void DominanceRelation::defineSetGoals(const TVector<bool> &gSet)
{
    T_UNUSED(gSet);
}


void DominanceRelation::definePriorityVec(const TVector<int>& p)
{
    T_UNUSED(p);
}

void DominanceRelation::defineThresholdVec(const TVector<double> &t)
{
    T_UNUSED(t);
}

void DominanceRelation::defineConstraintVecA(const TVector<double> &ag)
{
    T_UNUSED(ag);
}

void DominanceRelation::defineConstraintVecB(const TVector<double> &bg)
{
    T_UNUSED(bg);
}

} // namespace Representation
} // namespace Tigon
