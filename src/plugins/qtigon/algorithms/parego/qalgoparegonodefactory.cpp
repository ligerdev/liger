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
#include <qtigon/algorithms/parego/qalgoparegonodefactory.h>
#include <designer/designeditorplugin.h>
#include <designer/widgets/designertoolbox.h>

#include <QFile>

using namespace QTigon;
using namespace Designer;

QAlgoParEGONodeFactory::QAlgoParEGONodeFactory()
    : m_selected(false)
{
    Designer::DesignerToolBox* dtoolBox = Designer::Internal::DesignEditorPlugin::instance()->designerToolBox();

    QFile img_file(QString(":/qtigon/images/qalgoparegonode.svg"));
    if(img_file.exists()) {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("ParEGO algorithm node."),
                          QString(":/qtigon/images/qalgoparegonode.svg"));
    } else {
        m_toolBoxButton = dtoolBox->registerAlgorithm(this, QString("ParEGO algorithm node."),
                          QString(":/qtigon/images/default_qalgonode.svg"));
    }    
}

void QAlgoParEGONodeFactory::processButtonTriggered()
{

}

Designer::IProcessNode* QAlgoParEGONodeFactory::createProcessNode()
{
    return new QAlgoParEGONode();
}

bool QAlgoParEGONodeFactory::isSelected()
{
    return m_toolBoxButton->isChecked();
}

void QAlgoParEGONodeFactory::unSelect()
{
    m_toolBoxButton->setChecked(false);
}

