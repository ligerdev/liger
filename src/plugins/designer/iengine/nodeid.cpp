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
#include <designer/iengine/nodeid.h>

using namespace boost::uuids;

namespace Designer {

uuid NodeID::nodeIDRaw()
{
    return m_currentId;
}

QString NodeID::nodeID()
{
    return QString(to_string(m_currentId).c_str());
}

void NodeID::setID(uuid id)
{
    m_currentId = id;
}

void NodeID::setID(QString id)
{
    string_generator str_gen;
    m_currentId = str_gen(id.toStdString());
}

} // namespace Designer
