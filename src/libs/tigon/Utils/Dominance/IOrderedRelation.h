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
#ifndef IIORDEREDRELATION_H
#define IIORDEREDRELATION_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT IOrderedRelation
{
public:
    IOrderedRelation();
    virtual ~IOrderedRelation();

    virtual tribool isBetterThan(const TVector<double> &a,
                                 const TVector<double> &b) const = 0;

    virtual void defineParameters(const TVector<TVector<double>>& c);
};

} // namespace Representation
} // namespace Tigon

#endif // IIORDEREDRELATION_H
