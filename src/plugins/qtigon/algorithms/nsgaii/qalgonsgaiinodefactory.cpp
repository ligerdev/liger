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
#include <qtigon/algorithms/nsgaii/qalgonsgaiinodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>

#include <QFile>

using namespace QTigon;
using namespace Designer;

QAlgoNSGAIINodeFactory::QAlgoNSGAIINodeFactory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(QString(":/qtigon/images/qalgonsgaiinode.svg"));
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("NSGAII algorithm node."),
                          QString(":/qtigon/images/qalgonsgaiinode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("NSGAII algorithm node."),
                          QString(":/qtigon/images/default_qalgonode.svg"));
    }    
}

void QAlgoNSGAIINodeFactory::processButtonTriggered() 
{

}

Designer::IProcessNode* QAlgoNSGAIINodeFactory::createProcessNode()
{
    return new QAlgoNSGAIINode();
}

bool QAlgoNSGAIINodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QAlgoNSGAIINodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

