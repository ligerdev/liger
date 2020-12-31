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
#ifndef DOMINANCERELATION_H
#define DOMINANCERELATION_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT DominanceRelation
{
public:
    DominanceRelation();
    virtual ~DominanceRelation();

    virtual tribool isBetterThan(const TVector<double> &a,
                                 const TVector<double> &b) const;

    tribool dominates(const TVector<double> &a, const TVector<double> &b) const;

    // Parameters
    void defineWeakDom(bool c);
    bool weakDom() const;

    // Exposed methods from derived classes
    virtual void defineGoalVec(const TVector<double>& g);
    virtual void defineSetGoals(const TVector<bool>& gSet);
    virtual void definePriorityVec(const TVector<int>& p);
    virtual void defineThresholdVec(const TVector<double>& t);
    virtual void defineConstraintVecA(const TVector<double>& ag);
    virtual void defineConstraintVecB(const TVector<double>& bg);

private:
    bool m_weakDom;
};

} // namespace Representation
} // namespace Tigon

#endif // DOMINANCERELATION_H
