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
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>

#include <extensionsystem/pluginmanager.h>
#include <extensionsystem/iplugin.h>

#include <designer/iengine/nodeid.h>
#include <designer/iengine/nodeidfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iport.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <designer/iengine/iportlink.h>

// Libs
#include <boost/uuid/uuid.hpp>

#include <QDir>
#include <QMessageBox>
#include <QtSvg/QGraphicsSvgItem>
#include <QImage>
#include <QPainter>
#include <QMetaProperty>

#include <QDebug>
using namespace boost::uuids;

namespace Designer {

IProcessNode::IProcessNode()
    : m_text(QString("IProcess Node")),
      m_textColor(Qt::darkGreen),
      m_backgroundColor(Qt::white),
      m_outlineColor(Qt::darkBlue),
      m_procType(ABSTRACT_NODE),
      m_numberOfOutputs(0),
      m_numberOfInputs(0),
      m_numberOfConnectedOutputs(0),
      m_numberOfConnectedInputs(0),
      m_data(0),
      m_ownData(true),
      m_nodeID(NodeIDFactory::instance()->createNodeID()),
      m_maxIterations(0),
      m_budget(0),
      m_pixmap(0),
      m_renderer(0)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
}

IProcessNode::~IProcessNode()
{
    delete m_nodeID;
    delete m_pixmap;
    delete m_renderer;
    if(m_ownData) {
        ///\note only delete the data initialsed by the process node
        delete m_data;
    }
}

/*! \brief This function is meant to be invoked only by derivative
 *  classes. It is implemented here in order to avoid code duplication
 *  and all the perils involved.
 */
void IProcessNode::initializeNode()
{
    // Check how many ports already exist.
    QList<QGraphicsItem*> children = this->childItems();
    // Count input ports.
    qint32 n_inputs = 0;
    qint32 n_outputs= 0;
    foreach(QGraphicsItem* c, children) {
        if(qgraphicsitem_cast<InputPort*>(c)) {
            n_inputs++;
        }
        if(qgraphicsitem_cast<OutputPort*>(c)) {
            n_outputs++;
        }
    }

    qint32 in_diff = m_numberOfInputs - n_inputs;
    qint32 out_diff= m_numberOfOutputs- n_outputs;
    if(in_diff < 0) {
        // This should never be called.
        qDebug() << "More inputs than specified.";
        return;
    }
    if(out_diff < 0) {
        // This should never be called.
        qDebug() << "More outputs than specified.";
        return;
    }
    if(in_diff == 0 && out_diff == 0)
        return;

    // TODO: The assumption here is that all inputs are of
    // the same type. If this type is generic enough this may
    // be reasonable, however eventually we'll have to handle
    // I/O ports of different types.
    for(int i=0; i<in_diff; i++) {
        addInputPort(this,i);
    }
    for(int i=0; i<out_diff; i++) {
        addOutputPort(this,i);
    }
}

void IProcessNode::updateProcessState(ProcessState state)
{
    setState(state);
    emit nodeStateChangedSignal(state);
}

/*!
 * \brief This function generates the Liger document block for a node
 * \param XML stream writer
 */
void IProcessNode::generateLigerDocumentContent(QXmlStreamWriter &xmlWriter)
{
    xmlWriter.writeStartElement("ProcessNode");
    xmlWriter.writeAttribute("ClassName", this->metaObject()->className());

    // Node gui properties
    xmlWriter.writeStartElement("GUIProperties");
    QObject* obj = static_cast<QObject*>(this);
    for(int i=0; i<obj->metaObject()->propertyCount(); i++) {
        const char* name = obj->metaObject()->property(i).name();
        QString nameStr(name);
        if(nameStr == "objectName") {
            continue;
        }
        QVariant value = obj->property(name);
        xmlWriter.writeTextElement(nameStr, value.toString());
    }
    xmlWriter.writeEndElement();

    // Data/Operator Propeties
    xmlWriter.writeStartElement("Properties");
    writeDataProperties(xmlWriter);
    xmlWriter.writeEndElement();

    xmlWriter.writeEndElement();
}

/*!
 * \brief This function read a liger document block for configuration of a node.
 * \param XML stream reader
 */
void IProcessNode::loadLigerDocumentContent(QXmlStreamReader &xmlReader)
{
    // Load GUI properties
    while(!(xmlReader.name() == "GUIProperties" && xmlReader.isEndElement())) {
        if(xmlReader.isStartElement()) {
            QString name = xmlReader.name().toString();
            if(!name.isEmpty()) {
                QObject *object = static_cast<QObject*>(this);
                QVariant::Type type = object->property(name.toStdString().c_str()).type();
                QString valueStr = xmlReader.readElementText();
                QVariant value = QVariant(valueStr);
                value.convert(type);
                object->setProperty(name.toStdString().c_str(), value);
                qDebug() << name << ": " << object->property(name.toStdString().c_str());
            }
        }
        xmlReader.readNext();
    }

    /// \todo Load operator specific properties
    //xmlReader.readNextStartElement();
    readDataProperties(xmlReader);
}

/*!
 * \brief Load saved properties from xml reader.
 * \param Xml Stream Reader
 */
void IProcessNode::readDataProperties(QXmlStreamReader &xmlReader)
{
    Q_UNUSED(xmlReader);
}

/*!
 * \brief Virtual function needs to be overtitten by subclasses.
 * This function genertes a list of properties for the current liger operator
 * These properties can then be loaded by loading function readDataProperties.
 *
 * Format:
 * <Properties ProcessType="Type-of-Your-Operator">
 *    <Property-Name1> Property-Value </Property-Name1>
 *    <Property-Name2> Property-Value </Property-Name2>
 *    <Property-Name3> Property-Value </Property-Name3>
 *  </Properties>
 * \param xmlWriter
 */
void IProcessNode::writeDataProperties(QXmlStreamWriter &xmlWriter)
{
    Q_UNUSED(xmlWriter)
}

void IProcessNode::setText(const QString &text)
{
    prepareGeometryChange();
    m_text = text;
    repaintNode();
}

QString IProcessNode::text() const
{
    return m_text;
}

void IProcessNode::setTextColor(const QColor &color)
{
    m_textColor = color;
    repaintNode();
}

QColor IProcessNode::textColor() const
{
    return m_textColor;
}

void IProcessNode::setOutlineColor(const QColor &color)
{
    m_outlineColor = color;
    repaintNode();
}

QColor IProcessNode::outlineColor() const
{
    return m_outlineColor;
}

void IProcessNode::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    repaintNode();
}

QColor IProcessNode::backgroundColor() const
{
    return m_backgroundColor;
}

int IProcessNode::numberOfConnectedInputs()
{
    m_numberOfConnectedInputs = 0;
    foreach(InputPort* p, m_inputPorts) {
        m_numberOfConnectedInputs += (p->isConnected() ? 1 : 0);
    }

    return m_numberOfConnectedInputs;
}

int IProcessNode::numberOfConnectedOutputs()
{
    m_numberOfConnectedOutputs = 0;
    foreach(OutputPort* p, m_outputPorts) {
        m_numberOfConnectedOutputs += (p->isConnected() ? 1 : 0);
    }

    return m_numberOfConnectedOutputs;
}

void IProcessNode::addInputPort(IProcessNode *parent)
{
    InputPort* p = new InputPort(parent);
    m_inputPorts.append(p);
    repaintNode();
}

void IProcessNode::addOutputPort(IProcessNode* parent)
{
    OutputPort* p = new OutputPort(parent);
    m_outputPorts.append(p);
    repaintNode();
}

void IProcessNode::addInputPort(IProcessNode *parent, qint32 id)
{
    InputPort* p = new InputPort(parent,id);
    m_inputPorts.append(p);
    repaintNode();
}

void IProcessNode::addOutputPort(IProcessNode* parent, qint32 id)
{
    OutputPort* p = new OutputPort(parent,id);
    m_outputPorts.append(p);
    repaintNode();
}

void IProcessNode::removeInputPort(InputPort* port)
{
    int idx = m_inputPorts.indexOf(port);
    if(idx != -1)
        m_inputPorts.removeAt(idx);
}

void IProcessNode::removeOutputPort(OutputPort* port)
{
    int idx = m_outputPorts.indexOf(port);
    if(idx != -1)
        m_outputPorts.removeAt(idx);
}

QList<IPortLink*> IProcessNode::portLinks()
{
    QList<IPortLink*> plinks;
    foreach(InputPort* p, m_inputPorts) {
        if(p->isConnected())
            plinks.append(p->portLink());
    }
    foreach(OutputPort* p, m_outputPorts) {
        if(p->isConnected())
            plinks.append(p->portLink());
    }
    return plinks;
}

InputPort*  IProcessNode::inputPortAt(qint32 idx)
{
    if(idx>=0 && idx<m_numberOfInputs)
        return m_inputPorts[idx];
    else
        return 0;
}

OutputPort* IProcessNode::outputPortAt(qint32 idx)
{
    if(idx>=0 && idx<m_numberOfOutputs)
        return m_outputPorts[idx];
    else
        return 0;
}

void IProcessNode::repaintNode()
{
    update(outlineRect());
}

void IProcessNode::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget * /* widget */)
{

    QPen pen(m_outlineColor);
    if (option->state & QStyle::State_Selected) {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(m_backgroundColor);

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()),
                           roundness(rect.height()));

    if(m_pixmap) {
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
        QRectF pr = m_pixmap->rect();
        QPointF offset = QPointF(-(pr.width())/2, -(pr.height())/2);
        painter->drawPixmap(offset, *m_pixmap);
    }
    if(m_renderer) {
        m_renderer->render(painter,boundingRect());
    }

    if (option->state & QStyle::State_Selected)
        qt_graphicsItem_highlightSelected(this, painter, option);
}

// TODO: Modify this to adhere to Node rules.
void IProcessNode::qt_graphicsItem_highlightSelected(
        QGraphicsItem *item, QPainter *painter, const QStyleOptionGraphicsItem *option)
{
    const QRectF murect = painter->transform().mapRect(QRectF(0, 0, 1, 1));
    if (qFuzzyIsNull(qMax(murect.width(), murect.height())))
        return;

    const QRectF mbrect = painter->transform().mapRect(item->boundingRect());
    if (qMin(mbrect.width(), mbrect.height()) < qreal(1.0))
        return;

    qreal itemPenWidth;
    switch (item->type()) {
    case QGraphicsEllipseItem::Type:
        itemPenWidth = static_cast<QGraphicsEllipseItem *>(item)->pen().widthF();
        break;
    case QGraphicsPathItem::Type:
        itemPenWidth = static_cast<QGraphicsPathItem *>(item)->pen().widthF();
        break;
    case QGraphicsPolygonItem::Type:
        itemPenWidth = static_cast<QGraphicsPolygonItem *>(item)->pen().widthF();
        break;
    case QGraphicsRectItem::Type:
        itemPenWidth = static_cast<QGraphicsRectItem *>(item)->pen().widthF();
        break;
    case QGraphicsSimpleTextItem::Type:
        itemPenWidth = static_cast<QGraphicsSimpleTextItem *>(item)->pen().widthF();
        break;
    case QGraphicsLineItem::Type:
        itemPenWidth = static_cast<QGraphicsLineItem *>(item)->pen().widthF();
        break;
    default:
        itemPenWidth = 1.0;
    }
    const qreal pad = itemPenWidth / 2;

    const qreal penWidth = 0; // cosmetic pen

    const QColor fgcolor = option->palette.windowText().color();
    const QColor bgcolor( // ensure good contrast against fgcolor
                          fgcolor.red()   > 127 ? 0 : 255,
                          fgcolor.green() > 127 ? 0 : 255,
                          fgcolor.blue()  > 127 ? 0 : 255);

    painter->setPen(QPen(bgcolor, penWidth, Qt::SolidLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));

    painter->setPen(QPen(option->palette.windowText(), 0, Qt::DashLine));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(item->boundingRect().adjusted(pad, pad, -pad, -pad));
}

void IProcessNode::setSvg(QString svgFile)
{
    prepareGeometryChange();
    m_renderer = new QSvgRenderer(svgFile);
    if(m_pixmap) {
        delete m_pixmap;
        m_pixmap = 0;
    }
    repaintNode();
}

void IProcessNode::setPixmap(QString pixmapFile)
{
    prepareGeometryChange();
    m_pixmap = new QPixmap(pixmapFile);

    qreal width = m_pixmap->width();
    qreal height= m_pixmap->height();
    qreal base  = (width > height ? width : height);
    qreal fw    = (width/base)*(Constants::NODE_DEFAULT_SIZE);
    qreal fh    = (height/base)*(Constants::NODE_DEFAULT_SIZE);
    *m_pixmap   = m_pixmap->scaled(fw,fh);

    if(m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }

    repaintNode();
}

QVariant IProcessNode::itemChange(GraphicsItemChange change,
                                  const QVariant &value)
{
    if(change == ItemPositionHasChanged) {
        foreach(InputPort* p, m_inputPorts) {
            IPortLink* link = p->portLink();
            if(link){
                link->trackPorts();
                //link->trackPortsAvoid(portLinks());
            }
        }

        foreach(OutputPort* p, m_outputPorts) {
            IPortLink* link = p->portLink();
            if(link){
                link->trackPorts();
                //link->trackPortsAvoid(portLinks());
            }
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

ProcessState IProcessNode::state() const
{
    return m_state;
}

void IProcessNode::setState(ProcessState state)
{
    m_state = state;
}

void IProcessNode::setNumberOfInputs(const qint32 &numberOfInputs)
{
    m_numberOfInputs = numberOfInputs;
}

void IProcessNode::setNumberOfOutputs(const qint32 &numberOfOutputs)
{
    m_numberOfOutputs = numberOfOutputs;
}

QPainterPath IProcessNode::shape() const
{
    QRectF rect = outlineRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

QRectF IProcessNode::boundingRect() const
{
    qreal Margin = 1.0;
    return outlineRect().adjusted(-Margin/2, -Margin/2, +Margin/2, +Margin/2);
}

QRectF IProcessNode::outlineRect() const
{
    const int Padding = 0;
    QRectF rect;
    if(m_pixmap) {
        rect = m_pixmap->rect();
    }

    if(m_renderer) {
        rect = QRectF(0,0,Constants::NODE_DEFAULT_SIZE,
                      Constants::NODE_DEFAULT_SIZE);
    }

    rect.adjust(-(Padding), -Padding, +(Padding), +Padding);
    rect.translate(-rect.center());
    return rect;
}

QPolygonF IProcessNode::outlinePolygon()
{
    return QPolygonF(this->outlineRect());
}

int IProcessNode::roundness(double size) const
{
    const int Diameter = 6;
    return 100 * Diameter / int(size);
}


bool IProcessNode::isConnected()
{
    bool inputsConnected = true;
    bool outputsConnected= true;

    foreach(InputPort* p, m_inputPorts) {
        inputsConnected &= p->isConnected();
    }
    foreach(OutputPort* p, m_outputPorts) {
        outputsConnected &= p->isConnected();
    }

    if(inputsConnected & outputsConnected)
        return true;
    else
        return false;
}

void IProcessNode::setNodeID(QString id)
{
    m_nodeID->setID(id);
}

QString IProcessNode::nodeIDStr() const
{
    return m_nodeID->nodeID();
}

void *IProcessNode::data() const
{
    return m_data;
}

QString IProcessNode::processTypeString() const
{
    QString typeStr;

    switch(m_procType) {
    case ProcessType::MASTER_START_NODE:
        typeStr = QString("MASTER_START_NODE");
        break;
    case ProcessType::MASTER_END_NODE:
        typeStr = QString("MASTER_END_NODE");
        break;
    case ProcessType::ALGORITHM_NODE:
        typeStr = QString("ALGORITHM_NODE");
        break;
    case ProcessType::OPERATOR_NODE:
        typeStr = QString("OPERATOR_NODE");
        break;
    case ProcessType::PROBLEM_NODE:
        typeStr = QString("PROBLEM_NODE");
        break;
    default:
        break;
    }
    return typeStr;
}

void IProcessNode::dataReceivedFromEngine(void *data)
{
    if(m_data && m_ownData) {
        // Only delete m_data if it is created by the process node
        delete m_data;
        m_data = 0;
    }
    if(m_data != data && data) {
        m_data = data;
        m_ownData = false;
    }
}

void IProcessNode::setData(void *data)
{
    if(m_data != data && data) {
        m_data = data;
    }
}

} // namespace Designer
