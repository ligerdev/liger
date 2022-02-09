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
#include <tigon/Representation/Properties/ElementProperties.h>
#include <tigon/Representation/Properties/ElementPropertiesData.h>
#include <tigon/Representation/Elements/IElement.h>

namespace Tigon {
namespace Representation {

ElementProperties::ElementProperties()
    : d(new ElementPropertiesData)
{

}

ElementProperties::ElementProperties(int elidx)
    : d(new ElementPropertiesData)
{
    d->m_elemIdx  = elidx;
}

ElementProperties::ElementProperties(int elidx, const TString &name)
    : d(new ElementPropertiesData)
{
    d->m_elemIdx = elidx;
    d->m_elemName = name;
}

ElementProperties::ElementProperties(int elidx, const TString &name,
                                     const TString &desc)
    : d(new ElementPropertiesData)
{
    d->m_elemIdx = elidx;
    d->m_elemName = name;
    d->m_elemDescription = desc;
}

ElementProperties::ElementProperties(int elidx, const TString &name,
                                     const TString &desc, ElementType type)
    : d(new ElementPropertiesData)
{
    d->m_elemIdx = elidx;
    d->m_elemName = name;
    d->m_elemDescription = desc;
    d->m_elemType = type;
}

ElementProperties::ElementProperties(int elidx, const TString &name,
                                     const TString &desc, ElementType type,
                                     OptimizationType otype)
    : d(new ElementPropertiesData)
{
    d->m_elemIdx = elidx;
    d->m_elemName = name;
    d->m_elemDescription = desc;
    d->m_elemType = type;
    d->m_optimizationType = otype;
}

void ElementProperties::setElementID(const TString &id)
{
    d->m_elemID = id;
}

ElementProperties::~ElementProperties()
{

}

void ElementProperties::setIdx(int idx)
{
    if(idx>=0) {
        d->m_elemIdx = idx;
    }
}

void ElementProperties::setType(ElementType type)
{
    d->m_elemType = type;
}

void ElementProperties::setUnits(const TString &units)
{
    d->m_elemUnit = units;
}

void ElementProperties::setOptimizationType(OptimizationType type)
{
    d->m_optimizationType = type;
}

int ElementProperties::idx() const
{
    return d->m_elemIdx;
}

TString ElementProperties::ID() const
{
    return d->m_elemID;
}

TString ElementProperties::name() const
{
    return d->m_elemName;
}

TString ElementProperties::unit() const
{
    return d->m_elemUnit;
}

TString ElementProperties::description() const
{
    return d->m_elemDescription;
}

ElementType ElementProperties::type() const
{
    return d->m_elemType;
}

OptimizationType ElementProperties::optimizationType() const
{
    return d->m_optimizationType;
}

} // namespace Representation
} // namespace Tigon
