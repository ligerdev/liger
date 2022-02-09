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
#include <designer/iengine/nodeidfactory.h>
#include <designer/iengine/nodeid.h>
#include <QString>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

namespace Designer {

static NodeIDFactory* m_instance = 0;
static boost::mt19937_64 uuidran;
static boost::uuids::basic_random_generator<boost::mt19937_64> uuidgen(&uuidran);

NodeIDFactory::NodeIDFactory()
{
    m_instance = this;
    uuidran.seed(time(NULL));
}

NodeIDFactory::~NodeIDFactory()
{

}

NodeID* NodeIDFactory::createNodeID()
{
    NodeID* nodeID = new NodeID();
    nodeID->setID(uuidgen());
    return nodeID;
}

NodeIDFactory *NodeIDFactory::instance()
{
    return m_instance;
}

} // namespace Designer
