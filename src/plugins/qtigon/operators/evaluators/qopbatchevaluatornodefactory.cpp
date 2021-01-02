/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <qtigon/operators/evaluators/qopbatchevaluatornodefactory.h>

#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/widgets/designertoolbox.h>
#include <designer/designeditorplugin.h>

#include <QFile> 

using namespace QTigon;

QOpBatchEvaluatorNodeFactory::QOpBatchEvaluatorNodeFactory()
{
    Designer::DesignerToolBox* dtoolBox = 
            Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(":/qtigon/images/qopbatchevaluatornode.svg");
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerOperator(this, QString("BatchEvaluator operator node."),
                          QString(":/qtigon/images/qopbatchevaluatornode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerOperator(this, QString("BatchEvaluator operator node."),
                          QString(":/qtigon/images/default_qopnode.svg"));
    }
    img_file.close();    
}

Designer::IProcessNode* QOpBatchEvaluatorNodeFactory::createProcessNode()
{
    return new QOpBatchEvaluatorNode();
}

bool QOpBatchEvaluatorNodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QOpBatchEvaluatorNodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

void QOpBatchEvaluatorNodeFactory::processButtonTriggered()
{

}
