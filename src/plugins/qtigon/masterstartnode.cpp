/****************************************************************************
**
** Copyright (C) 2012-2017 The University of Sheffield (www.sheffield.ac.uk)
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
#include <qtigon/masterstartnode.h>
#include <qtigon/dialogs/startnodedialog.h>

#include <QMessageBox>
#include <QString>

#include <QDebug>

namespace QTigon {

MasterStartNode::MasterStartNode()
    : m_dialog(new StartNodeDialog)
    , m_userDefinedSeed(false)
{
    m_text         = QString("MSN");
    m_procType     = Designer::MASTER_START_NODE;

    m_outlineColor = Qt::darkBlue;
    setNumberOfOutputs(1);
    setNumberOfInputs(0);
    setData(0);

    setSvg(":/qtigon/images/master_start_node.svg");

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();

    connect(m_dialog, SIGNAL(accepted()), this, SLOT(acceptChangesSlot()));
}

MasterStartNode::~MasterStartNode()
{
    delete m_dialog;
}

void MasterStartNode::acceptChangesSlot()
{
    m_userDefinedSeed = m_dialog->useCustomSeed();
    if(m_userDefinedSeed) {
        m_seed = m_dialog->seed();
    }
    emit nodePropertiesChangedSignal();
}

void MasterStartNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    m_dialog->show();
    m_dialog->raise();
}

void MasterStartNode::readDataProperties(QXmlStreamReader &xmlReader)
{

}

void MasterStartNode::writeDataProperties(QXmlStreamWriter &xmlWriter)
{
    xmlWriter.writeAttribute("ProcessType", "MASTER_START_NODE");
    if(m_userDefinedSeed) {
        xmlWriter.writeTextElement("randomSeed", QString::number(m_seed));
    }
}

uint MasterStartNode::seed() const
{
    return m_seed;
}

void MasterStartNode::setSeed(uint seed)
{
    m_seed = seed;
    if(m_userDefinedSeed) {
        m_dialog->setSeed(seed);
    }
}

bool MasterStartNode::userDefinedSeed() const
{
    return m_userDefinedSeed;
}

void MasterStartNode::setUserDefinedSeed(bool userDefinedSeed)
{
    m_userDefinedSeed = userDefinedSeed;
    m_dialog->setUseUserDefinedSeedCheckbox(userDefinedSeed);
}

} // namespace QTigon
