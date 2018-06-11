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
#pragma once

#include <typeinfo>
#include <tigon/tigon_global.h>
#include <tigon/Core/TTuple.h>

namespace Tigon {

typedef std::size_t TTypeHash;
typedef TTuple<TString, TString, TTypeHash> TPropertyInfo;

#define getType(t) typeid(t).hash_code()

namespace Constants {
const TString TypeConversionFailure("Cannot convert string to required type");
}

/// Macros
/*! \brief DECLARE_CLASS
 * \param ClassName the name of the class, including namespaces.
 * \example
 * \code
 *     DECLARE_CLASS(Tigon::Representation::IFunction)
 * \endcode
 */
#define DECLARE_CLASS(ClassName) \
    public: \
    virtual TString className() const {return #ClassName;}

/*! \brief WRITE declare a way to set the property
 * \param Name name of the property
 * \param Type type of the propert, e.g. int, bool, TString
 * \param Setter the public function used to set the property
 * \example
 * \code
 *     WRITE(NumberOfInputs, int, TP_defineNInputs)
 * \endcode
 */
#define WRITE(Name, Type, Setter) \
    if (name == #Name) {Type val = convertStringTo<Type>(value); Setter(val); return;}

/*! \brief READ declare a way to read the property value
 * \param Name name of the property
 * \param Getter the public function used to read the property
 * \example
 * \code
 *      READ(NumberOfInputs, TP_nInputs)
 * \endcode
 */
#define READ(Name, Getter) \
    if (name == #Name) return convertToString(Getter());

#define HANDLE_READ_PROPERTIES_BEGIN(Base) \
    public: \
    virtual TString propertyValue(const TString &name) { \
        TString res = Base::propertyValue(name); \
            if (res.empty()) {

#define HANDLE_READ_PROPERTIES_END \
    } return res; }

#define HANDLE_WRITE_PROPERTIES_BEGIN(Base) \
    public: \
    virtual void setProperty(const TString &name, const TString &value) { \
        Base::setProperty(name, value);

#define HANDLE_WRITE_PROPERTIES_END \
    }

/*!
 * \brief The TObject class.
 * Inherit from this class for property reading and writing.
 */
class LIGER_TIGON_EXPORT TObject
{
public:
    TObject();
    TObject(const TObject& other) = delete;
    virtual ~TObject() {}
    /*!
     * \brief className
     * \return the name of the class. By default it returns the name deduced by
     * typeid.name
     */
    virtual TString className() const;
    TStringList propertyNames() const;
    TStringList propertyDescriptions() const;
    TVector<TTypeHash> propertyTypes() const;

    /*!
     * \brief setProperty
     * \param name Name of the property
     * \param value Value of the property in string format
     */
    virtual void setProperty(const TString& name,
                             const TString& value) {T_UNUSED(name) T_UNUSED(value) }
    /*!
     * \brief propertyValue
     * \param name Name of the property
     * \return the value of the property in string format
     */
    virtual TString propertyValue(const TString& name) {T_UNUSED(name); return "";}

    int propertyCount() const;

protected:
    void addProperty(const TString& name,
                     const TString& description,
                     TTypeHash type);
private:
    TVector<TPropertyInfo> m_propertiesInfo;
};

}
