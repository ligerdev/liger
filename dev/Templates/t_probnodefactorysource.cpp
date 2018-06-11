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
#include <{{ plugin_name }}/problems/{% filter lower %}{{ ClassName }}{% endfilter %}factory.h>
#include <QFile>
#include <designer/designeditorplugin.h>

using namespace {{ Namespace }};

{{ ClassName }}Factory::{{ ClassName }}Factory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();
    
    QFile img_file(":/{{plugin_name}}/images/{% filter lower %}{{ClassName}}{% endfilter %}.svg");
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerProblem(this, QString("{{StringName}} problem node."),
                          QString(":/{{plugin_name}}/images/{% filter lower %}{{ClassName}}{% endfilter %}.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerProblem(this, QString("{{StringName}} problem node."),
                          QString(":/{{plugin_name}}/images/default_qprobnode.svg"));
    }
    img_file.close();
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

