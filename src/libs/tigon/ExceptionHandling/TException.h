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
#ifndef TEXCEPTION_H
#define TEXCEPTION_H
#include <tigon/tigon_global.h>
#include <exception>

namespace Tigon {

class LIGER_TIGON_EXPORT TException: public std::exception
{

public:

    TException();
    TException(const TString &className, const TString &typeOfError);

    const char* what() const noexcept;

    TString className() const;
    TString typeOfError() const;

    void setClassName(const TString &className);
    void setTypeOfError(const TString &typeOfError);

private:
    TString m_className;
    TString m_typeOfError;
};

} // namespace Tigon


#endif // TEXCEPTION_H
