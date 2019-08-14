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
#include <tigon/Factories/DominanceRelationFactory.h>
#include <tigon/Utils/Dominance/DominanceRelation.h>
#include <tigon/Utils/Dominance/ConstrDomRelation.h>
#include <tigon/Utils/Dominance/Preferability.h>
#include <tigon/Utils/Dominance/PreferabilityConstraintHandling.h>

namespace Tigon {
namespace Representation {

DominanceRelationFactory& DominanceRelationFactory::instance()
{
    static DominanceRelationFactory singleFactory;
    return singleFactory;
}

DominanceRelation *DominanceRelationFactory::create(DominanceRelationType type) const
{
    DominanceRelation* rel = nullptr;

    switch(type) {
    case NormalDominanceRelation:
        rel = new DominanceRelation();
        break;
    case ConstraintHandlingDominanceRelation:
        rel = new ConstrDomRelation();
        break;
    case PreferabilityRelation:
        rel = new Preferability();
        break;
    case PreferabilityConstraintHandlingRelation:
        rel = new PreferabilityConstraintHandling();
        break;
    default:
        break;
    }

    return rel;
}

DominanceRelationFactory::DominanceRelationFactory()
{

}

DominanceRelationFactory::~DominanceRelationFactory()
{

}

} // namespace Representation
} // namespace Tigon
