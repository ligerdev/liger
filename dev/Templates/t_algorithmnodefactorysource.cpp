/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <{% filter lower %}{{ Namespace }}{% endfilter %}/algorithms/{{ operator_type }}/{% filter lower %}{{ ClassName }}{% endfilter %}factory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>

#include <QFile>

using namespace {{ Namespace }};
using namespace Designer;

{{ ClassName }}Factory::{{ ClassName }}Factory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(QString(":/{% filter lower %}{{ Namespace }}{% endfilter %}/images/{% filter lower %}{{ ClassName }}{% endfilter %}.svg"));
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("{{StringName}} algorithm node."),
                          QString(":/{% filter lower %}{{ Namespace }}{% endfilter %}/images/{% filter lower %}{{ ClassName }}{% endfilter %}.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("{{StringName}} algorithm node."),
                          QString(":/qtigon/images/default_qalgonode.svg"));
    }    
}

void {{ ClassName }}Factory::processButtonTriggered() 
{

}

Designer::IProcessNode* {{ ClassName }}Factory::createProcessNode()
{
    return new {{ ClassName }}();
}

bool {{ ClassName }}Factory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void {{ ClassName }}Factory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}


