/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Distributions/IDistribution.h>

namespace Tigon {
namespace Representation {

IElement::IElement()
    : m_type(RealType),
      m_value(0.0),
      m_variate(nullptr)
{

}

IElement::IElement(const IElement &el)
    : m_type(el.m_type),
      m_value(el.m_value),
      m_variate(nullptr)
{
    if(el.m_variate != nullptr) {
        m_variate = el.m_variate->clone();
    }
}

IElement::IElement(double val)
    : m_type(RealType),
      m_value(val),
      m_variate(nullptr)
{

}

IElement::IElement(int val)
    : m_type(IntegerType),
      m_value(static_cast<int64>(val)),
      m_variate(nullptr)
{

}

IElement::IElement(int64 val)
    : m_type(IntegerType),
      m_value(val),
      m_variate(nullptr)
{

}

IElement::IElement(ElementType type)
    : m_type(type),
      m_value(0.0),
      m_variate(nullptr)
{
    enforceType();
}

IElement::IElement(ElementType type, double val)
    : m_type(type),
      m_value(val),
      m_variate(nullptr)
{
    enforceType();
}

IElement::IElement(ElementType type, int val)
    : m_type(type),
      m_value(static_cast<int64>(val)),
      m_variate(nullptr)
{
    enforceType();
}

IElement::IElement(ElementType type, int64 val)
    : m_type(type),
      m_value(val),
      m_variate(nullptr)
{
    enforceType();
}

IElement::~IElement()
{
    if(m_variate != nullptr) {
        delete m_variate;
    }
}

IElement& IElement::operator=(const IElement &el)
{
    // Check self-assignment
    if(this == &el) {
        return *this;
    }

    m_type  = el.m_type;
    m_value = el.m_value;

    if(m_variate != nullptr) {
        delete m_variate;
        m_variate = 0;
    }

    if(el.m_variate != nullptr) {
        m_variate = el.m_variate->clone();
    }

    return *this;
}

IElementSPtr IElement::clone()
{
    return IElementSPtr(new IElement(*this));
}

ElementType IElement::type() const
{
    return m_type;
}

void IElement::defineType(ElementType type)
{
    if(type == m_type) {
        return;
    }

    m_type = type;

    enforceType();

}

void IElement::defineValue(const IElement &val)
{
    m_value = val.m_value;
    enforceType();
}

IDistribution* IElement::dist() const
{
    return m_variate;
}

void IElement::defineDist(IDistribution* dist)
{
    if(m_variate != nullptr) {
        delete m_variate;
    }
    m_variate = dist;
}

double IElement::sample() const
{
    if(m_variate != nullptr) {
        return m_variate->sample();
    } else {
        return value<double>();
    }
}

void IElement::negate()
{
    m_value = -value<double>();
    enforceType();
    if(m_variate != nullptr) {
        m_variate->negate();
    }
}

void IElement::resetToMaxValue()
{
    switch(m_type) {
    case RealType:
        m_value = Tigon::Highest;
        break;
    case IntegerType:
    case OrdinalType:
    case NominalType:
        m_value = Tigon::HighestInteger;
        break;
    default:
        m_value = Tigon::Highest;
        break;
    }
    enforceType();
}

void IElement::resetToMinValue()
{
    switch(m_type) {
    case RealType:
        m_value = Tigon::Lowest;
        break;
    case IntegerType:
    case OrdinalType:
    case NominalType:
        m_value = Tigon::LowestInteger;
        break;
    default:
        m_value = Tigon::Lowest;
        break;
    }
    enforceType();
}

IElement IElement::minValue() const
{
    IElement ret(*this);
    ret.resetToMinValue();
    return ret;
}

IElement IElement::maxValue() const
{
    IElement ret(*this);
    ret.resetToMaxValue();
    return ret;
}

// Private method
void IElement::enforceType()
{
    switch(m_type) {
    case RealType:
        m_value = value<double>();
        break;

    case IntegerType:
    case OrdinalType:
    case NominalType:
        m_value = value<int64>();
        break;

    default:
        // ERROR
        m_value = 0.0;
        break;
    }
}

} // namespace Representation
} // namespace Tigon
