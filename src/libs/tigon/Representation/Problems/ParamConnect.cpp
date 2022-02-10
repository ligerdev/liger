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
#include <tigon/Representation/Problems/ParamConnect.h>

namespace Tigon {
namespace Representation {

ParamConnect::ParamConnect()
    : m_isConnected(false), m_outputIdx(0),
      m_outputType(OutputType::Objective), m_isMaximization(false)
{

}

ParamConnect::~ParamConnect()
{

}

void ParamConnect::setIsConnected(bool isConnected)
{
    m_isConnected = isConnected;
}
void ParamConnect::setOutputIdx(size_t outputIdx)
{
    m_outputIdx = outputIdx;
}
void ParamConnect::setOutputType(OutputType outputType)
{
    m_outputType = outputType;
}
void ParamConnect::setIsMaximization(bool isMaximization)
{
    m_isMaximization = isMaximization;
}

bool ParamConnect::isConnected() const
{
    return m_isConnected;
}
size_t ParamConnect::outputIdx() const
{
    return m_outputIdx;
}
OutputType ParamConnect::outputType() const
{
    return m_outputType;
}
bool ParamConnect::isMaximization() const
{
    return m_isMaximization;
}

} // namespace Representation
} // namespace Tigon
