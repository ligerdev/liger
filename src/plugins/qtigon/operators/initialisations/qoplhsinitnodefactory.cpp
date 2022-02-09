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
#include <qtigon/operators/initialisations/qoplhsinitnodefactory.h>

#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/widgets/designertoolbox.h>
#include <designer/designeditorplugin.h>

#include <QFile> 

using namespace QTigon;

QOpLHSInitNodeFactory::QOpLHSInitNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = 
            Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(":/qtigon/images/qoplhsinitnode.svg");
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerOperator(this, QString("LHSInit operator node."),
                          QString(":/qtigon/images/qoplhsinitnode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerOperator(this, QString("LHSInit operator node."),
                          QString(":/qtigon/images/default_qopnode.svg"));
    }
    img_file.close();    
}

Designer::IProcessNode* QOpLHSInitNodeFactory::createProcessNode()
{
    return new QOpLHSInitNode();
}

bool QOpLHSInitNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QOpLHSInitNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

void QOpLHSInitNodeFactory::processButtonTriggered()
{

}
