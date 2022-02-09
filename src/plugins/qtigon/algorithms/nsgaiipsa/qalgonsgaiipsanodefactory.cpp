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
#include <qtigon/algorithms/nsgaiipsa/qalgonsgaiipsanodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>

#include <QFile>

using namespace QTigon;
using namespace Designer;

QAlgoNSGAIIPSANodeFactory::QAlgoNSGAIIPSANodeFactory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(QString(":/qtigon/images/qalgonsgaiipsanode.svg"));
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("NSGAIIPSA algorithm node."),
                          QString(":/qtigon/images/qalgonsgaiipsanode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("NSGAIIPSA algorithm node."),
                          QString(":/qtigon/images/default_qalgonode.svg"));
    }    
}

void QAlgoNSGAIIPSANodeFactory::processButtonTriggered() 
{

}

Designer::IProcessNode* QAlgoNSGAIIPSANodeFactory::createProcessNode()
{
    return new QAlgoNSGAIIPSANode();
}

bool QAlgoNSGAIIPSANodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QAlgoNSGAIIPSANodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

