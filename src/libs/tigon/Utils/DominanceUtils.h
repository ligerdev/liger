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
#ifndef DOMINANCEUTILS_H
#define DOMINANCEUTILS_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>


namespace Tigon {
namespace Representation {
class ISet;
}
}

namespace Tigon {
using namespace Tigon::Representation;

LIGER_TIGON_EXPORT
tribool weakDominance(const TVector<double> &a, const TVector<double> &b);
LIGER_TIGON_EXPORT
tribool strongDominance(const TVector<double> &a, const TVector<double> &b);
LIGER_TIGON_EXPORT
tribool epsilonDominance(const TVector<double> &a, const TVector<double> &b,
                         double epsilon=Epsilon);


LIGER_TIGON_EXPORT
TVector<int> dominanceCount(const ISet* set, bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<int> dominanceCount(const TVector<IMappingSPtr>& set, bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSort(const ISet* set, bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSort(
        const TVector<IMappingSPtr> &set, bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSortParametric(const ISet* set,
                                          const TVector<int>& solIds,
                                          bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSortParametric(
        const TVector<IMappingSPtr>& set, const TVector<int>& solIds,
        bool weakDom=true);

LIGER_TIGON_EXPORT
ISet* nonDominatedSet(const ISet* set, bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<IMappingSPtr> nonDominatedSet(const TVector<IMappingSPtr> &set,
                                      bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<TVector<int>> sortRanksNonDomOriginal(TVector<int> domCount,
                                      const TVector<TVector<int>>& dominatedBy);
LIGER_TIGON_EXPORT
TVector<TVector<int>> sortRanksNonDom(TVector<int> domCount,
                                      const TVector<TVector<int>>& dominatedBy);


/// *******************
/// Constraint handling
/// *******************

LIGER_TIGON_EXPORT
double solutionConstraintViolation(IMappingSPtr x,
                                   const TVector<IElementSPtr>& threshVec);

LIGER_TIGON_EXPORT
double solutionConstraintViolation(IMappingSPtr x,
                                   const TVector<double>& threshVec);
LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSortConstraintHandling(
        const ISet* set, const TVector<double> &threshVec, bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSortConstraintHandling(
        const TVector<IMappingSPtr> &set, const TVector<double> &threshVec,
        bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSortParametricConstraintHandling(
        const ISet* set, const TVector<double> &threshVec,
        const TVector<int>& solIds, bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSortParametricConstraintHandling(
        const TVector<IMappingSPtr> &set, const TVector<double> &threshVec,
        const TVector<int>& solIds, bool weakDom=true);


/// ***********
/// Goal vector
/// ***********

LIGER_TIGON_EXPORT
tribool preferability(const TVector<double> &a, const TVector<double> &b,
                      const TVector<double> &g, bool weakDom=true);
LIGER_TIGON_EXPORT
tribool preferability(const TVector<double> &a, const TVector<double> &b,
                      const TVector<double> &g, const TVector<int> &p,
                      bool weakDom=true);
LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSort(const ISet* set, const TVector<double>& goal,
                                bool weakDom = true);
LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSort(
        const TVector<IMappingSPtr> &set, const TVector<double>& goal,
        bool weakDom = true);

LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSortParametric(const ISet* set,
                                          const TVector<double>& goal,
                                          const TVector<int>& solIds,
                                          bool weakDom = true);
LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSortParametric(
        const TVector<IMappingSPtr> &set, const TVector<double>& goal,
        const TVector<int>& solIds, bool weakDom = true);


/// *********************
/// Goals and Constraints
/// *********************

LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSortConstraintHandling(
        const ISet* set, const TVector<double> &threshVec,
        const TVector<double> &goal, bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSortConstraintHandling(
        const TVector<IMappingSPtr> &set,
        const TVector<double> &threshVec,
        const TVector<double> &goal,
        bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<ISet*> nonDominanceSortParametricConstraintHandling(
        const ISet* set, const TVector<double> &threshVec,
        const TVector<double> &goal, const TVector<int>& solIds,
        bool weakDom=true);

LIGER_TIGON_EXPORT
TVector<TVector<IMappingSPtr>> nonDominanceSortParametricConstraintHandling(
        const TVector<IMappingSPtr> &set,
        const TVector<double> &threshVec,
        const TVector<double> &goal,
        const TVector<int>& solIds,
        bool weakDom=true);


/// ******
/// Others
/// ******

// The preferred set is a set with all the reference vectors that either
// dominate the goal vector or dominated by it. If all vectors in the reference
// set are non-dominated, only the goal vector is returned.
LIGER_TIGON_EXPORT
TVector<TVector<double>> preferredSet(const TVector<TVector<double>> &refSet,
                                      const TVector<double> &goalVec);

} // namespace Tigon
#endif // _H
