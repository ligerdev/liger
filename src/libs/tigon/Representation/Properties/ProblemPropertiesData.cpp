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
#include <tigon/Representation/Properties/ProblemPropertiesData.h>

namespace Tigon {
namespace Representation {


ProblemPropertiesData::ProblemPropertiesData()
    : m_probID("")
    , m_probName("")
    , m_probDescription("")
    , m_probType(Tigon::NonConvex)
{

}


ProblemPropertiesData::ProblemPropertiesData(const ProblemPropertiesData& data)
    : m_probID(data.m_probID)
    , m_probName(data.m_probName)
    , m_probDescription(data.m_probDescription)
    , m_probType(data.m_probType)
{

}

ProblemPropertiesData::~ProblemPropertiesData()
{

}


} // namespace Represenatation
} // namespace Tigon
