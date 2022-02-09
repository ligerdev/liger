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
#ifndef BOXCONSTRAINTSVIOLATIONCORRECTIONS_H
#define BOXCONSTRAINTSVIOLATIONCORRECTIONS_H
#include <tigon/tigon_global.h>

namespace Tigon {
using namespace Tigon::Representation;

LIGER_TIGON_EXPORT void reflectIntoFeasibleDomain(IMappingSPtr imap,
                                                  BoxConstraintsDataSPtr box);

LIGER_TIGON_EXPORT void truncateIntoFeasibleDomain(IMappingSPtr imap,
                                                  BoxConstraintsDataSPtr box);

LIGER_TIGON_EXPORT void mapIntoToroidalDomain(IMappingSPtr imap,
                                                  BoxConstraintsDataSPtr box);

} // namespace Tigon
#endif // BOXCONSTRAINTSVIOLATIONCORRECTIONS_H
