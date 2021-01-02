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
#include <qtigon/algorithms/sparego/qalgosparegonode.h>
#include <qtigon/qtigonutils.h>
#include <tigon/Algorithms/sParEGO.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>

#include <tigon/Tigon.h>
using Tigon::TObject;
using Tigon::TString;

#include <QFile>
#include <QString>

#include <QDebug>

using namespace QTigon;
using namespace Designer;
using namespace Tigon::Algorithms;

QAlgosParEGONode::QAlgosParEGONode()
    : m_dialog(new QAlgorithmDialog)
{
    m_text         = QString("C");
    m_procType     = ALGORITHM_NODE;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;
    setNumberOfOutputs(1);
    setNumberOfInputs(1);
    setData(new sParEGO);

    QFile img_file(":/qtigon/images/qalgosparegonode.svg");
    if(img_file.exists()) {
        setSvg(":/qtigon/images/qalgosparegonode.svg");
    } else {
        setSvg(":/qtigon/images/default_qalgonode.svg");
    }
    img_file.close();

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();

    connect(m_dialog, SIGNAL(algDataChanged()), this, SIGNAL(nodePropertiesChangedSignal()));
}

QAlgosParEGONode::~QAlgosParEGONode()
{
    delete m_dialog;
}

void QAlgosParEGONode::updateProcessState(ProcessState state)
{
    switch(state) {
    case Designer::ProcessState::NODE_RUNNING:
        m_dialog->setInputDisabled();
        break;
    case Designer::ProcessState::NODE_DONE:
        m_dialog->setInputDisabled(false);
        break;
    default:
        m_dialog->setInputDisabled(false);
        break;
    }
    IProcessNode::updateProcessState(state);
}

void QAlgosParEGONode::readDataProperties(QXmlStreamReader &xmlReader)
{
    xmlReader.readNextStartElement(); //todo remove
    TObject* thisOp = static_cast<TObject*>(data());
    TObject* op = 0;
    while(!xmlReader.isEndElement() || !(xmlReader.name() == "Properties")) {
        xmlReader.readNext();
        if(xmlReader.isStartElement()) {
            if(xmlReader.name() == "TigonOperator"){
                op = 0;
            } else if(xmlReader.name() == "Name") {
                TString opName = xmlReader.readElementText().toStdString();
                sParEGO* alg = static_cast<sParEGO*>(thisOp);
                for(int i=0; i<alg->operators().size(); i++) {
                    if(alg->operators()[i]->className() == opName) {
                        op = alg->operators()[i];
                    }
                }
            } else {
                assignPropertyToObject(op, &xmlReader);
            }
        }
    }
}

void QAlgosParEGONode::writeDataProperties(QXmlStreamWriter &xmlWriter)
{
    xmlWriter.writeAttribute("ProcessType", "ALGORITHM_NODE");
    IAlgorithm* thisAlg = static_cast<IAlgorithm*>(data());
    xmlWriter.writeTextElement("TigonAlgorithm",
                               QString::fromStdString(thisAlg->className()));
    for(int i=0; i<thisAlg->operators().size(); i++) {
        xmlWriter.writeStartElement("TigonOperator");
        xmlWriter.writeTextElement("Name",
                                   QString::fromStdString(
                                       thisAlg->operators()[i]->className()));
        TObject* object = thisAlg->operators()[i];
        propertiesToXml(object, &xmlWriter);
        xmlWriter.writeEndElement();
    }
}

void QAlgosParEGONode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    sParEGO* thisAlg = static_cast<sParEGO*>(data());
    if(thisAlg) {
        m_dialog->setup(thisAlg);
        m_dialog->show();
        m_dialog->raise();
    }
}
