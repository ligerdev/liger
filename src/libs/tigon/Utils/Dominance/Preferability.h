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
#ifndef PREFERABILITY_H
#define PREFERABILITY_H

#include <tigon/Utils/Dominance/DominanceRelation.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT Preferability : public DominanceRelation
{
public:
    Preferability();
    ~Preferability();

    tribool isBetterThan(const TVector<double> &a, const TVector<double> &b) const;

    tribool preferability(const TVector<double> &a,
                          const TVector<double> &b) const;

    // Parameters
    void defineParameters(const TVector<TVector<double>>& c);

    void defineGoalVec(const TVector<double>& g);
    TVector<double> goalVec()              const;

    void defineSetGoals(const TVector<bool>& gSet);
    TVector<bool> setGoals()                 const;

private:
    TVector<double> m_goalVec;
    TVector<bool>   m_setGoals;
};

} // namespace Representation
} // namespace Tigon

#endif // PREFERABILITY_H
