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
#include <qtigon/algorithms/smsemoa/qalgosmsemoanodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>

#include <QFile>

using namespace QTigon;
using namespace Designer;

QAlgoSMSEMOANodeFactory::QAlgoSMSEMOANodeFactory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(QString(":/qtigon/images/qalgosmsemoanode.svg"));
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("SMSEMOA algorithm node."),
                          QString(":/qtigon/images/qalgosmsemoanode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("SMSEMOA algorithm node."),
                          QString(":/qtigon/images/default_qalgonode.svg"));
    }    
}

void QAlgoSMSEMOANodeFactory::processButtonTriggered() 
{

}

Designer::IProcessNode* QAlgoSMSEMOANodeFactory::createProcessNode()
{
    return new QAlgoSMSEMOANode();
}

bool QAlgoSMSEMOANodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QAlgoSMSEMOANodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

