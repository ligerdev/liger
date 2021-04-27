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
#ifndef NODEID_H
#define NODEID_H

#include <designer/designer_global.h>
#include <designer/designerconstants.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <QString>

namespace Designer {

class DESIGNER_EXPORT NodeID
{

public:
    NodeID()
        : m_currentId(boost::uuids::random_generator()())
    {}

    NodeID(const NodeID& n)
    {
        m_currentId = n.m_currentId;
    }

    boost::uuids::uuid    nodeIDRaw();
    QString nodeID();
    void setID(boost::uuids::uuid id);
    void setID(QString id);
private:
    boost::uuids::uuid m_currentId;
};

} // namespace Designer

#endif // NODEID_H
