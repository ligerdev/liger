/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef HYPERVOLUMEUTILS_H
#define HYPERVOLUMEUTILS_H
#include <tigon/tigon_global.h>


namespace Tigon {
namespace Representation {
class ISet;
}
}

namespace Tigon {
using namespace Tigon::Representation;

LIGER_TIGON_EXPORT double hypervolume(ISet* set);
LIGER_TIGON_EXPORT double hypervolume(ISet*                set,
                                     const TVector<double> ref);

LIGER_TIGON_EXPORT double hypervolume(TVector<TVector<double > > set);
LIGER_TIGON_EXPORT double hypervolume(TVector<TVector<double > > set,
                                     const TVector<double>      ref);

} // namespace Tigon
#endif // HYPERVOLUMEUTILS_H
