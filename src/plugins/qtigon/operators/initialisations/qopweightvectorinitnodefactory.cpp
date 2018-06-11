/****************************************************************************
**
** Copyright (C) 2012-2016 The University of Sheffield (www.sheffield.ac.uk)
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
#include <qtigon/operators/initialisations/qopweightvectorinitnodefactory.h>

#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/widgets/designertoolbox.h>
#include <designer/designeditorplugin.h>

#include <QFile> 

using namespace QTigon;

QOpWeightVectorInitNodeFactory::QOpWeightVectorInitNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(":/qtigon/images/qopweightvectorinitnode.png");
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerOperator(this, QString("WeightVectorInit operator node."),
                          QString(":/qtigon/images/qopweightvectorinitnode.png"));
    } else {
        m_toolBoxButton = dtoolBox->registerOperator(this, QString("WeightVectorInit operator node."),
                          QString(":/qtigon/images/default_qopnode.png"));
    }
    img_file.close();    
}

Designer::IProcessNode* QOpWeightVectorInitNodeFactory::createProcessNode()
{
    return new QOpWeightVectorInitNode();
}

bool QOpWeightVectorInitNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QOpWeightVectorInitNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

void QOpWeightVectorInitNodeFactory::processButtonTriggered()
{

}
