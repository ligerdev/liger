/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Representation {

IElement operator- (const IElement& m)
{
    ElementType mType = m.type();

    switch(mType) {
    case RealType:
        return IElement(mType, -m.value<double>());

    case IntegerType:
    case OrdinalType:
        return IElement(mType, -m.value<int64>());

    case NominalType:
        throw TException("IElementOperations::operator-", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator-", UnrecognisedTypeException);
    }
}

// The order of checking is important as the most frequently
// visited types are first, e.g. real real etc.

IElement operator*(const IElement& m, const IElement& c)
{
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    {
        double mVal = m.value<double>();

        switch(cType) {
        case RealType:
            return IElement(RealType, mVal * c.value<double>());

        case IntegerType:
            return IElement(RealType, mVal * c.value<int64>());

        case OrdinalType:
            throw TException("IElementOperations::operator*", InvalidOrdinalOpException);

        case NominalType:
            throw TException("IElementOperations::operator*", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator*", UnrecognisedTypeException);
        }
    }

    case IntegerType:
    {
        int64 mVal = m.value<int64>();

        switch(cType) {
        case IntegerType:
            return IElement(IntegerType, mVal * c.value<int64>());

        case RealType:
            return IElement(RealType, mVal * c.value<double>());

        case OrdinalType:
            throw TException("IElementOperations::operator*", InvalidOrdinalOpException);

        case NominalType:
            throw TException("IElementOperations::operator*", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator*", UnrecognisedTypeException);
        }
    }

    case OrdinalType:
        throw TException("IElementOperations::operator*", InvalidOrdinalOpException);

    case NominalType:
        throw TException("IElementOperations::operator*", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator*", UnrecognisedTypeException);
    }
}

IElement operator/(const IElement& m, const IElement& c)
{
    if(c == 0.0) {
        throw TException("IElementOperations::operator/", DivisionByZeroException);
    }

    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    {
        double mVal = m.value<double>();

        switch(cType) {
        case RealType:
            return IElement(RealType, mVal / c.value<double>());

        case IntegerType:
            return IElement(RealType, mVal / c.value<int64>());

        case OrdinalType:
            throw TException("IElementOperations::operator/", InvalidOrdinalOpException);

        case NominalType:
            throw TException("IElementOperations::operator/", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator/", UnrecognisedTypeException);
        }
    }

    case IntegerType:
    {
        int64 mVal = m.value<int64>();

        switch(cType) {
        case IntegerType:
            return IElement(IntegerType, mVal / c.value<int64>());

        case RealType:
            return IElement(RealType, mVal / c.value<double>());

        case OrdinalType:
            throw TException("IElementOperations::operator/", InvalidOrdinalOpException);

        case NominalType:
            throw TException("IElementOperations::operator/", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator/", UnrecognisedTypeException);
        }
    }

    case OrdinalType:
        throw TException("IElementOperations::operator/", InvalidOrdinalOpException);

    case NominalType:
        throw TException("IElementOperations::operator/", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator/", UnrecognisedTypeException);
    }
}

IElement operator+(const IElement& m, const IElement& c)
{
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    {
        double mVal = m.value<double>();

        switch(cType) {
        case RealType:
            return IElement(RealType, mVal + c.value<double>());

        case IntegerType:
        case OrdinalType:
            return IElement(RealType, mVal + c.value<int64>());

        case NominalType:
            throw TException("IElementOperations::operator+", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator+", UnrecognisedTypeException);
        }
    }

    case IntegerType:
    {
        int64 mVal = m.value<int64>();

        switch(cType) {
        case IntegerType:
        case OrdinalType:
            return IElement(IntegerType, mVal + c.value<int64>());

        case RealType:
            return IElement(RealType, mVal + c.value<double>());

        case NominalType:
            throw TException("IElementOperations::operator+", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator+", UnrecognisedTypeException);
        }
    }

    case OrdinalType:
    {
        int64 mVal = m.value<int64>();

        switch(cType) {
        case OrdinalType:
            return IElement(OrdinalType, mVal + c.value<int64>());

        case IntegerType:
            return IElement(IntegerType, mVal + c.value<int64>());

        case RealType:
            return IElement(RealType, mVal + c.value<double>());

        case NominalType:
            throw TException("IElementOperations::operator+", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator+", UnrecognisedTypeException);
        }
    }

    case NominalType:
        throw TException("IElementOperations::operator+", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator+", UnrecognisedTypeException);
    }
}

IElement operator-(const IElement& m, const IElement& c)
{
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    {
        double mVal = m.value<double>();

        switch(cType) {
        case RealType:
            return IElement(RealType, mVal - c.value<double>());

        case IntegerType:
        case OrdinalType:
            return IElement(RealType, mVal - c.value<int64>());

        case NominalType:
            throw TException("IElementOperations::operator-", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator-", UnrecognisedTypeException);
        }
    }

    case IntegerType:
    {
        int64 mVal = m.value<int64>();

        switch(cType) {
        case IntegerType:
        case OrdinalType:
            return IElement(IntegerType, mVal - c.value<int64>());

        case RealType:
            return IElement(RealType, mVal - c.value<double>());

        case NominalType:
            throw TException("IElementOperations::operator-", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator-", UnrecognisedTypeException);
        }
    }

    case OrdinalType:
    {
        int64 mVal = m.value<int64>();

        switch(cType) {
        case OrdinalType:
            return IElement(OrdinalType, mVal - c.value<int64>());

        case IntegerType:
            return IElement(IntegerType, mVal - c.value<int64>());

        case RealType:
            return IElement(RealType, mVal - c.value<double>());

        case NominalType:
            throw TException("IElementOperations::operator-", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator-", UnrecognisedTypeException);
        }
    }

    case NominalType:
        throw TException("IElementOperations::operator-", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator-", UnrecognisedTypeException);
    }
}

tribool operator< (const IElement& m, const IElement& c)
{
    tribool     ret;
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    case IntegerType:
    case OrdinalType:
        switch(cType) {
        case RealType:
        case IntegerType:
        case OrdinalType:
            ret = (m.value<double>() < c.value<double>());
            break;

        case NominalType:
            throw TException("IElementOperations::operator<", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator<", UnrecognisedTypeException);
        }
        break;

    case NominalType:
        throw TException("IElementOperations::operator<", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator<", UnrecognisedTypeException);
    }

    return ret;
}

tribool operator> (const IElement& m, const IElement& c)
{
    tribool     ret;
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    case IntegerType:
    case OrdinalType:
        switch(cType) {
        case RealType:
        case IntegerType:
        case OrdinalType:
            ret = (m.value<double>() > c.value<double>());
            break;

        case NominalType:
            throw TException("IElementOperations::operator>", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator>", UnrecognisedTypeException);
        }
        break;

    case NominalType:
        throw TException("IElementOperations::operator>", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator>", UnrecognisedTypeException);
    }

    return ret;
}

tribool operator<= (const IElement& m, const IElement& c)
{
    tribool     ret;
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    case IntegerType:
    case OrdinalType:
        switch(cType) {
        case RealType:
        case IntegerType:
        case OrdinalType:
            ret = (m.value<double>() <= c.value<double>());
            break;

        case NominalType:
            throw TException("IElementOperations::operator<=", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator<=", UnrecognisedTypeException);
        }
        break;

    case NominalType:
        throw TException("IElementOperations::operator<=", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator<=", UnrecognisedTypeException);
    }

    return ret;
}

tribool operator>= (const IElement& m, const IElement& c)
{
    tribool     ret;
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    case IntegerType:
    case OrdinalType:
        switch(cType) {
        case RealType:
        case IntegerType:
        case OrdinalType:
            ret = (m.value<double>() >= c.value<double>());
            break;

        case NominalType:
            throw TException("IElementOperations::operator>=", InvalidNominalOpException);

        default:
            throw TException("IElementOperations::operator>=", UnrecognisedTypeException);
        }
        break;

    case NominalType:
        throw TException("IElementOperations::operator>=", InvalidNominalOpException);

    default:
        throw TException("IElementOperations::operator>=", UnrecognisedTypeException);
    }

    return ret;
}

tribool operator== (const IElement& m, const IElement& c)
{
    tribool     ret;
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    case IntegerType:
    case OrdinalType:
    case NominalType:
        switch(cType) {
        case RealType:
        case IntegerType:
        case OrdinalType:
        case NominalType:
            ret = (m.value<double>() == c.value<double>());
            break;

        default:
            throw TException("IElementOperations::operator==", UnrecognisedTypeException);
        }
        break;

    default:
        throw TException("IElementOperations::operator==", UnrecognisedTypeException);
    }

    return ret;
}

tribool operator!= (const IElement& m, const IElement& c)
{
    tribool     ret;
    ElementType mType = m.type();
    ElementType cType = c.type();

    switch(mType) {
    case RealType:
    case IntegerType:
    case OrdinalType:
    case NominalType:
        switch(cType) {
        case RealType:
        case IntegerType:
        case OrdinalType:
        case NominalType:
            ret = (m.value<double>() != c.value<double>());
            break;

        default:
            throw TException("IElementOperations::operator!=", UnrecognisedTypeException);
        }
        break;

    default:
        throw TException("IElementOperations::operator!=", UnrecognisedTypeException);
    }

    return ret;
}

IElement tMin(const IElement &m, const IElement &c)
{
    if((m.type()==NominalType) ||(c.type()==NominalType)) {
        throw TException("IElementOperations::tMin", InvalidNominalOpException);
    }
    if(m <= c) {
        return IElement(m);
    } else {
        return IElement(c);
    }
}

IElement tMax(const IElement &m, const IElement &c)
{
    if((m.type()==NominalType) ||(c.type()==NominalType)) {
        throw TException("IElementOperations::tMax", InvalidNominalOpException);
    }
    if(m >= c) {
        return IElement(m);
    } else {
        return IElement(c);
    }
}

std::ostream &operator<<(std::ostream &os, const IElement &el)
{
    Tigon::ElementType eType = el.type();
    switch(eType) {
    case RealType:
        os << el.value<double>();
        break;
    case IntegerType:
    case OrdinalType:
    case NominalType:
        os << el.value<int64>();
        break;
    default:
        throw TException("IElementOperations::operator<<", UnrecognisedTypeException);
    }
    return os;
}

} // namespace Representation
} // namespace Tigon
