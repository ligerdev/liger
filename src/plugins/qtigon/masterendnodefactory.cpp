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
#include <qtigon/masterendnodefactory.h>

namespace QTigon {

MasterEndNodeFactory::MasterEndNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = ExtensionSystem::PluginManager::instance()
            ->getObject<Designer::DesignerToolBox>();
    m_toolBoxButton = dtoolBox->registerOperator(this, QString("Master end node."),
                                                 QString(":/qtigon/images/master_end_node.svg"));
}

void MasterEndNodeFactory::processButtonTriggered()
{

}

Designer::IProcessNode* MasterEndNodeFactory::createProcessNode()
{
    return new MasterEndNode();
}

bool MasterEndNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void MasterEndNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

} // namespace QTigon
