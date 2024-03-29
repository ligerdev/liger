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
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {

TException::TException()
    : m_className(""),
      m_typeOfError("")
{

}

TException::TException(const TString &className, const TString &typeOfError)
    : m_className(className),
      m_typeOfError(typeOfError)
{

}

const char* TException::what() const noexcept
{
    TString msg = className() + ": " + typeOfError();

    char* p = new char[msg.length()+1];
    //strcpy(p,msg.c_str());
    std::copy(msg.begin(), msg.end(), p);
    p[msg.size()] = '\0';

    return p;
}

TString TException::className() const
{
    return m_className;
}

TString TException::typeOfError() const
{
    return m_typeOfError;
}

void TException::setClassName(const TString &className)
{
    m_className = className;
}

void TException::setTypeOfError(const TString &typeOfError)
{
    m_typeOfError = typeOfError;
}

} // namespace Tigon
