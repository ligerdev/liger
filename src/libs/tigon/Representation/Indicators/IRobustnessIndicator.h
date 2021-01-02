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
#ifndef IROBUSTNESSINDICATOR_H
#define IROBUSTNESSINDICATOR_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class IDistribution;

class LIGER_TIGON_EXPORT IRobustnessIndicator
{
public:
    IRobustnessIndicator();
    virtual ~IRobustnessIndicator();

    virtual double evaluate(IDistribution* dist)  const = 0;
};

} // namespace Representation
} // namespace Tigon

#endif // IROBUSTNESSINDICATOR_H
