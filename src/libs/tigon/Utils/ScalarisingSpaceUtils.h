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
#ifndef SCALARISINGSPACEUTILS_H
#define SCALARISINGSPACEUTILS_H
#include <tigon/tigon_global.h>

using namespace Tigon::Representation;

namespace Tigon {

LIGER_TIGON_EXPORT double scalarisingSpace(const TVector<TVector<double> >& objectiveSet,
                                          const TVector<TVector<double> >& refSet,
                                          const TVector<double>& ideal,
                                          const TVector<double>& antiIdeal);

LIGER_TIGON_EXPORT double scalarisingSpaceRobust(const TVector<TVector<double> >& objectiveSet,
        const TVector<TVector<double> >& decisionSet,
        const TVector<TVector<double> >& refSet,
        const TVector<double>& ideal,
        const TVector<double>& antiIdeal,
        const TVector<TVector<double> >& decBound,
        double maxDist, double confidenceLevel);

LIGER_TIGON_EXPORT double scalarisingSpaceRobust(const TVector<TVector<double> >& objectiveSet,
        const TVector<TVector<double> >& decisionSet,
        const TVector<TVector<double> >& refSet,
        const TVector<double>& ideal,
        const TVector<double>& antiIdeal,
        BoxConstraintsDataSPtr box,
        double maxDist, double confidenceLevel);

LIGER_TIGON_EXPORT double scalarisingSpaceRandom(
        const TVector<TVector<TVector<double> > >& objectiveSet,
        const TVector<TVector<double> >& refSet,
        const TVector<double>& ideal,
        const TVector<double>& antiIdeal,
        double confidenceLevel);

} // namespace Tigon
#endif // SCALARISINGSPACEUTILS_H
