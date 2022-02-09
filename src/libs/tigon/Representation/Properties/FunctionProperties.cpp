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
#include <tigon/Representation/Properties/FunctionProperties.h>

namespace Tigon {
namespace Representation {

FunctionProperties::FunctionProperties()
    : d(new FunctionPropertiesData)
{

}

FunctionProperties::FunctionProperties(const TString &name)
    : d(new FunctionPropertiesData)
{
    d->m_funcName = name;
}

FunctionProperties::FunctionProperties(const TString &name,
                                       const TString &desc,
                                       FunctionType type)
    : d(new FunctionPropertiesData)
{
    d->m_funcName = name;
    d->m_funcDescription = desc;
    d->m_functionType = type;
}

void FunctionProperties::setFunctionID(const TString &id)
{
    d->m_funcID = id;
}

FunctionProperties::~FunctionProperties()
{

}

TString FunctionProperties::ID() const
{
    return d->m_funcID;
}

TString FunctionProperties::name() const
{
    return d->m_funcName;
}

TString FunctionProperties::description() const
{
    return d->m_funcDescription;
}

FunctionType FunctionProperties::type() const
{
    return d->m_functionType;
}

TString FunctionProperties::typeStr() const
{
    TString type =  FuncTypeStrOther;
    switch(d->m_functionType) {
    case FunctionType::Internal:
        type = FuncTypeStrInternal;
        break;
    case FunctionType::Matlab:
        type = FuncTypeStrMatlab;
        break;
    case FunctionType::SharedLib:
        type = FuncTypeStrSharedLib;
        break;
    case FunctionType::Python:
        type = FuncTypeStrPython;
    default:
        break;
    }

    return type;
}

TString FunctionProperties::path() const
{
    return d->m_functionPath;
}

void FunctionProperties::setFunctionPath(const TString &path)
{
    d->m_functionPath = path;
}

} // namespace Representation
} // namespace Tigon

