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
#ifndef PREFERABILITYCONSTRAINTHANDLING_H
#define PREFERABILITYCONSTRAINTHANDLING_H

#include <tigon/Utils/Dominance/Preferability.h>
#include <tigon/Utils/Dominance/ConstrDomRelation.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT PreferabilityConstraintHandling :
        public Preferability,
        public ConstrDomRelation
{
public:
    PreferabilityConstraintHandling();
    ~PreferabilityConstraintHandling();

    tribool isBetterThan(const TVector<double> &a, const TVector<double> &b) const;

    tribool preferability_constraint(const TVector<double> &a,
                                     const TVector<double> &b) const;
};

} // namespace Representation
} // namespace Tigon

#endif // PREFERABILITYCONSTRAINTHANDLING_H
