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
#ifndef FUNCTIONPROPERTIESDATA_H
#define FUNCTIONPROPERTIESDATA_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT FunctionPropertiesData
{
public:
    FunctionPropertiesData();
    FunctionPropertiesData(const FunctionPropertiesData& data)
          : m_funcID(data.m_funcID)
          , m_funcName(data.m_funcName)
          , m_funcDescription(data.m_funcDescription)
          , m_functionType(data.m_functionType)
          , m_functionPath(data.m_functionPath)
    {}
    ~FunctionPropertiesData();

    TString m_funcID;
    TString m_funcName;
    TString m_funcDescription;
    FunctionType m_functionType;
    TString m_functionPath;
};

} // namespace Representation
} // namespace Tigon

#endif // FUNCTIONPROPERTIESDATA_H
