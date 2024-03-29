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
#include <tigon/Representation/Properties/FunctionPropertiesData.h>

namespace Tigon {
namespace Representation {

FunctionPropertiesData::FunctionPropertiesData()
    : m_funcID("")
    , m_funcName("")
    , m_funcDescription("")
    , m_functionType(FunctionType::Other)
    , m_functionPath("")
{

}

FunctionPropertiesData::~FunctionPropertiesData()
{

}


} // namespace Representation
} // namespace Tigon

