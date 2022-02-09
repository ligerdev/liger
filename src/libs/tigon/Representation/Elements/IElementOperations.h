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
#ifndef IELEMENTOPERATIONS_H
#define IELEMENTOPERATIONS_H

#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class IElement;

//// IElement Operations
IElement LIGER_TIGON_EXPORT operator- (const IElement& m);
// *
IElement LIGER_TIGON_EXPORT operator* (const IElement& m, const IElement& c);
// /
IElement LIGER_TIGON_EXPORT operator/ (const IElement& m, const IElement& c);
// +
IElement LIGER_TIGON_EXPORT operator+ (const IElement& m, const IElement& c);
// -
IElement LIGER_TIGON_EXPORT operator- (const IElement& m, const IElement& c);
// <
tribool  LIGER_TIGON_EXPORT operator< (const IElement& m, const IElement& c);
// >
tribool  LIGER_TIGON_EXPORT operator> (const IElement& m, const IElement& c);
// <=
tribool  LIGER_TIGON_EXPORT operator<=(const IElement& m, const IElement& c);
// >=
tribool  LIGER_TIGON_EXPORT operator>=(const IElement& m, const IElement& c);
// ==
tribool  LIGER_TIGON_EXPORT operator==(const IElement& m, const IElement& c);
// !=
tribool  LIGER_TIGON_EXPORT operator!=(const IElement& m, const IElement& c);

// <<
LIGER_TIGON_EXPORT std::ostream& operator<< (std::ostream& os, const IElement& el);

/// Math function
// min
IElement LIGER_TIGON_EXPORT tMin(const IElement& m, const IElement& c);
// max
IElement LIGER_TIGON_EXPORT tMax(const IElement& m, const IElement& c);

} // namespace Representation
} // namespace Tigon

#endif // IELEMENTOPERATIONS_H
