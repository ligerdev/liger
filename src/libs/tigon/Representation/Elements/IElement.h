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
#ifndef IELEMENT_H
#define IELEMENT_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <boost/variant.hpp>

/*
 * This class has been intentionally created to be lightweight. Think twice
 * before introducing more complexity in its descendants since the
 * impact on performance can be quite severe. For example, introducing a TString
 * can result in 20x lower performance.
 **/

namespace Tigon {
namespace Representation {
class IDistribution;

using TVariant = boost::variant<int64, double>;

/*!
 * visitor_generic_value static_vistor class from boost, for type conversion.
 */
template<typename Out>
class visitor_generic_value : public boost::static_visitor<Out>
{
public:
    Out operator()(double d) const { return static_cast<Out>(boost::get<double>(d)); }
    Out operator()(int d) const {return static_cast<Out>(boost::get<int>(d)); }
    Out operator()(int64 d) const {return static_cast<Out>(boost::get<int64>(d)); }
    Out operator()(long double d) const {return static_cast<Out>(boost::get<long double>(d)); }
};

/*!
 * \brief The IElement class acts like a union for the most common numerical
 * data types used in Tigon.
 */
class LIGER_TIGON_EXPORT IElement
{
public:
    /// Default constructor
    IElement();

    /// Copy constructor
    IElement(const IElement &el);

    /// Allow automatic type conversions
    IElement(double  val);
    IElement(int    val);
    IElement(int64  val);

    /// Other constructors
    IElement(Tigon::ElementType type);
    IElement(Tigon::ElementType type, double val);
    IElement(Tigon::ElementType type, int val);
    IElement(Tigon::ElementType type, int64 val);

    /// Destructors
    ~IElement();

    /// Copy assignment operator
    IElement& operator=(const IElement &el);

    IElementSPtr clone();

    Tigon::ElementType type()          const;
    void defineType(Tigon::ElementType type);

    /*!
     * \brief Sets the value of the IElement according to the value of another
     * IElement. The type of the IElement is not affected, and the stored value
     * is converted to the existing type.
     */
    void defineValue(const IElement &val);

    /*! \brief Returns the IElement converted to the type specified.
     */
    template <typename T=double> T value() const
    {
        return boost::apply_visitor(visitor_generic_value<T>(), m_value);
    }

    IDistribution* dist()          const;
    void defineDist(IDistribution* dist);

    double sample()    const;

    void negate();

    void resetToMaxValue();
    void resetToMinValue();

    ///\brief returns an IElement of the same type with the min/max value for
    /// this type
    IElement minValue() const;
    IElement maxValue() const;

private:
    void enforceType();
    Tigon::ElementType    m_type;
    TVariant           m_value;
    IDistribution*     m_variate;
};
} // namespace Representation
} // namespace Tigon

#endif // IELEMENT_H
