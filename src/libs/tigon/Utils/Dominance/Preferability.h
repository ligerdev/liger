/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef PREFERABILITY_H
#define PREFERABILITY_H

#include <tigon/Utils/Dominance/DominanceRelation.h>

namespace Tigon {
namespace Representation {

/**
 * @brief The Preferability class
 * Note that this class uses virtual inheritance from DominanceRelation.
 * This ensures that there will be only one instance of DominanceRelation in a
 * diamond inheritance case. Currently the classes DominanceRelation, Preferability,
 * ConstrDomRelation and PreferabilityConstraintHandling are organised in a
 * diamond inheritance structure. When a pointer or reference to DominanceRelation
 * is initialised with PreferabilityConstraintHandling, there will be only one
 * instance of DominanceRelation, as opposed to two in case virtual inheritance
 * wasn't used.
 */
class LIGER_TIGON_EXPORT Preferability : virtual public DominanceRelation
{
public:
    Preferability();
    ~Preferability();

    tribool isBetterThan(const TVector<double> &a, const TVector<double> &b) const;

    tribool preferability(const TVector<double> &a,
                          const TVector<double> &b,
                          const TVector<double> &g,
                          const TVector<bool> &s) const;

    tribool preferability(const TVector<double> &a,
                          const TVector<double> &b,
                          const TVector<double> &g,
                          const TVector<bool> &s,
                          const TVector<int> &p) const;

    // Parameters
    void defineGoalVec(const TVector<double>& g);
    TVector<double> goalVec()              const;

    void defineSetGoals(const TVector<bool>& gSet);
    TVector<bool> setGoals()                 const;

    void definePriorityVec(const TVector<int>& p);
    TVector<int> priorityVec()              const;

private:
    TVector<double> m_goalVec;
    TVector<bool>   m_setGoals;
    TVector<int>    m_priorityVec;

};

} // namespace Representation
} // namespace Tigon

#endif // PREFERABILITY_H
