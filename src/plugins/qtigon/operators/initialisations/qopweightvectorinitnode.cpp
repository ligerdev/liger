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
#include <qtigon/operators/initialisations/qopweightvectorinitnode.h>

#include <designer/iengine/iport.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>

#include <QtGui>
#include <QDir>
#include <QColor>
#include <QMessageBox>
#include <QString>

#include <QDebug>

using namespace QTigon;
using namespace Designer;

QOpWeightVectorInitNode::QOpWeightVectorInitNode()
{
    m_text         = QString("C");
    m_procType     = OPERATOR_NODE;
    m_textColor    = Qt::darkGreen;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;
    m_numberOfOutputs = 1;
    m_numberOfInputs  = 1;
    m_numberOfConnectedOutputs = 0;
    m_numberOfConnectedInputs  = 0;

    QFile img_file(":/qtigon/images/qopweightvectorinitnode.svg");
    if(img_file.exists()) {
        setSvg(":/qtigon/images/qopweightvectorinitnode.svg");
    } else {
        setSvg(":/qtigon/images/default_qopnode.svg");
    }
    img_file.close();

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();
}

void QOpWeightVectorInitNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    Tigon::Operators::WeightVectorInit* thisOp =
            static_cast<Tigon::Operators::WeightVectorInit*>(m_data);
    if(thisOp) {
        // TODO set parent to ldesignwidget
        m_dialog = new QOperatorDiag();
        connect(m_dialog, SIGNAL(opDataChanged()),
                this, SIGNAL(nodePropertiesChangedSignal()));
        m_dialog->setup(thisOp);
        m_dialog->show();
    }
}
