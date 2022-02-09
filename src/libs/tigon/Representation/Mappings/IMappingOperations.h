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
#ifndef IMAPPINGOPERATIONS_H
#define IMAPPINGOPERATIONS_H

#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class IMapping;

// strong dominance relation
tribool  LIGER_TIGON_EXPORT operator< (const IMapping& m, const IMapping& c);
tribool  LIGER_TIGON_EXPORT operator> (const IMapping& m, const IMapping& c);
// weak dominance relation
tribool  LIGER_TIGON_EXPORT operator<=(const IMapping& m, const IMapping& c);
tribool  LIGER_TIGON_EXPORT operator>=(const IMapping& m, const IMapping& c);

tribool  LIGER_TIGON_EXPORT operator==(const IMapping& m, const IMapping& c);
tribool  LIGER_TIGON_EXPORT operator!=(const IMapping& m, const IMapping& c);

} // namespace Representation
} // namespace Tigon

#endif // IMAPPINGOPERATIONS_H
