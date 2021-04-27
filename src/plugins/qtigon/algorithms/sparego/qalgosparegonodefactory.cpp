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
#include <qtigon/algorithms/sparego/qalgosparegonodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>

#include <QFile>

using namespace QTigon;
using namespace Designer;

QAlgosParEGONodeFactory::QAlgosParEGONodeFactory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(QString(":/qtigon/images/qalgosparegonode.svg"));
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("sParEGO algorithm node."),
                          QString(":/qtigon/images/qalgosparegonode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("sParEGO algorithm node."),
                          QString(":/qtigon/images/default_qalgonode.svg"));
    }    
}

void QAlgosParEGONodeFactory::processButtonTriggered()
{

}

Designer::IProcessNode* QAlgosParEGONodeFactory::createProcessNode()
{
    return new QAlgosParEGONode();
}

bool QAlgosParEGONodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QAlgosParEGONodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

