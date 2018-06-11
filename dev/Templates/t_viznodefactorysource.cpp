/****************************************************************************
**
** Copyright (C) 2012-2013 The University of Sheffield (www.sheffield.ac.uk)
**
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
#include <{% filter lower %}{{ Namespace }}{% endfilter %}/{% filter lower %}{{ ClassName }}{% endfilter %}factory.h>

using namespace {{ Namespace }};

{{ ClassName }}Factory::{{ ClassName }}Factory()
    : m_selected(false)
{
    Core::DesignerManager* dm = Core::DesignerManager::instance();
    Core::Internal::DesignerView* dview = dm->currentDesignerView();
    Core::DesignerToolBox* dtoolBox = dview->designerToolBox();
    m_toolBoxButton = dtoolBox->registerVisualization(this, QString("{{ StringName }} visualization node."),
                      QString(":/{% filter lower %}{{ Namespace }}{% endfilter %}/images/{% filter lower %}{{ ClassName }}{% endfilter %}.svg"));
}

void {{ ClassName }}Factory::processButtonTriggered() {
    if(m_selected)
        m_selected = false;
    else
        m_selected = true;
}

Designer::IProcessNode* {{ ClassName }}Factory::createProcessNode()
{
    return new {{ ClassName }}();
}

bool {{ ClassName }}Factory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

