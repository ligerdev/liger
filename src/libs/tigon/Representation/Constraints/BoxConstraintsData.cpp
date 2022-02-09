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
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Representation {

BoxConstraintsData::BoxConstraintsData()
{

}

BoxConstraintsData::BoxConstraintsData(const BoxConstraintsData& data)
{
    m_lowerBounds = copyVector(data.m_lowerBounds);
    m_upperBounds = copyVector(data.m_upperBounds);
}

BoxConstraintsData::BoxConstraintsData(int size)
{
    for(int i=0; i<size; i++) {
        IElement lb;
        lb.resetToMinValue();
        m_lowerBounds.push_back(lb);

        IElement ub;
        ub.resetToMaxValue();
        m_upperBounds.push_back(ub);
    }
}

BoxConstraintsData::BoxConstraintsData(const TVector<ElementType> &t)
{
    for(ElementType type : t) {
        IElement lb(type);
        lb.resetToMinValue();
        m_lowerBounds.push_back(lb);

        IElement ub(type);
        ub.resetToMaxValue();
        m_upperBounds.push_back(ub);
    }
}

BoxConstraintsData::BoxConstraintsData(const TVector<ElementProperties> &t)
{
    for(ElementProperties pt : t) {
        IElement lb(pt.type());
        lb.resetToMinValue();
        m_lowerBounds.push_back(lb);

        IElement ub(pt.type());
        ub.resetToMaxValue();
        m_upperBounds.push_back(ub);
    }
}

BoxConstraintsData::~BoxConstraintsData()
{
    clear();
}

BoxConstraintsDataSPtr BoxConstraintsData::clone()
{
    return BoxConstraintsDataSPtr(new BoxConstraintsData(*this));
}

TVector<IElement> BoxConstraintsData::lowerBounds() const
{
    return m_lowerBounds;
}

TVector<IElement> BoxConstraintsData::upperBounds() const
{
    return m_upperBounds;
}

IElement BoxConstraintsData::lowerBound(int idx) const
{
    if(isInRange(idx, m_lowerBounds.size())) {
        return m_lowerBounds.at(idx);
    } else {
        // ERROR
        return IElement(PROTOTYPE_REAL_ERROR_CODE);
    }
}

IElement BoxConstraintsData::upperBound(int idx) const
{
    if(isInRange(idx, m_upperBounds.size())) {
        return m_upperBounds.at(idx);
    } else {
        // ERROR
        return IElement(PROTOTYPE_REAL_ERROR_CODE);
    }
}

int BoxConstraintsData::size() const

{
    int lbs = m_lowerBounds.size();
    int ubs = m_upperBounds.size();
    if(lbs == ubs) {
        return lbs;
    } else {
        // ERROR
        return PROTOTYPE_INT_ERROR_CODE;
    }
}

void BoxConstraintsData::appendBounds(Tigon::ElementType t)
{
    IElement lb(t);
    lb.resetToMinValue();
    m_lowerBounds.push_back(lb);

    IElement ub(t);
    ub.resetToMaxValue();
    m_upperBounds.push_back(ub);
}

void BoxConstraintsData::appendBounds(const IElement& lb, const IElement& ub)
{
    m_lowerBounds.push_back(lb);
    m_upperBounds.push_back(ub);
}

void BoxConstraintsData::appendLowerBound(const IElement &lb)
{
    m_lowerBounds.push_back(lb);
}

void BoxConstraintsData::appendUpperBound(const IElement &ub)
{
    m_lowerBounds.push_back(ub);
}

void BoxConstraintsData::defineLowerBounds(const TVector<IElement>& lb)
{
    m_lowerBounds = lb;
}

void BoxConstraintsData::defineUpperBounds(const TVector<IElement>& ub)
{
    m_upperBounds = ub;
}

void BoxConstraintsData::defineBounds(const TVector<IElement> &lb,
                                      const TVector<IElement> &ub)
{
    m_lowerBounds = lb;
    m_upperBounds = ub;
}

void BoxConstraintsData::defineLowerBound(int idx, const IElement& lb)
{
    if(isInRange(idx, m_lowerBounds.size())) {
        m_lowerBounds[idx].defineValue(lb);
    } else {
        // ERROR
    }
}

void BoxConstraintsData::defineUpperBound(int idx, const IElement& ub)
{
    if(isInRange(idx, m_upperBounds.size())) {
        m_upperBounds[idx].defineValue(ub);
    } else {
        // ERROR
    }
}

void BoxConstraintsData::replaceLowerBound(int idx, const IElement& lb)
{
    if(isInRange(idx, m_lowerBounds.size())) {
        m_lowerBounds[idx] = lb;
    } else {
        // ERROR
    }
}

void BoxConstraintsData::replaceUpperBound(int idx, const IElement& ub)
{
    if(isInRange(idx, m_upperBounds.size())) {
        m_upperBounds[idx] = ub;
    } else {
        // ERROR
    }
}

void BoxConstraintsData::clear()
{
    m_lowerBounds.clear();
    m_upperBounds.clear();
}
} // namespace Representation
} // namespace Tigon
