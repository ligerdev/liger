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
#include <qtigon/masterstartnodefactory.h>
#include <qtigon/masterstartnode.h>

namespace QTigon {

MasterStartNodeFactory::MasterStartNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = ExtensionSystem::PluginManager::instance()
                                        ->getObject<Designer::DesignerToolBox>();
    m_toolBoxButton = dtoolBox->registerOperator(this, QString("Master start node."),
                      QString(":/qtigon/images/master_start_node.svg"));
}

void MasterStartNodeFactory::processButtonTriggered() 
{

}

Designer::IProcessNode* MasterStartNodeFactory::createProcessNode()
{
    return new MasterStartNode();
}

bool MasterStartNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void MasterStartNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

} // namespace QTigon
