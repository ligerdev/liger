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
#include <qtigon/operators/initialisations/qopuserdefinedinitnode.h>
#include <qtigon/qtigonutils.h>
#include <qtigon/dialogs/populationviewer.h>
#include <qtigon/dialogs/qoperatordiag.h>

#include <designer/iengine/iport.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>

#include <tigon/Tigon.h>
using Tigon::TObject;

#include <QFile>
#include <QColor>
#include <QMessageBox>
#include <QGroupBox>
#include <QString>

#include <QDebug>

using namespace QTigon;
using namespace Designer;
using namespace Tigon::Operators;

QOpUserDefinedInitNode::QOpUserDefinedInitNode()
    : m_dialog(new QOperatorDiag)
{
    m_text         = QString("C");
    m_procType     = OPERATOR_NODE;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;
    setNumberOfOutputs(1);
    setNumberOfInputs(1);
    setData(new UserDefinedInit);

    QFile img_file(":/qtigon/images/qopuserdefinedinitnode.svg");
    if(img_file.exists()) {
        setSvg(":/qtigon/images/qopuserdefinedinitnode.svg");
    } else {
        setSvg(":/qtigon/images/default_qopnode.svg");
    }
    img_file.close();

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();

    connect(m_dialog, SIGNAL(opDataChanged()),
            this, SIGNAL(nodePropertiesChangedSignal()));
    m_viwer = new PopulationViewer;
    connect(m_viwer, &PopulationViewer::updateFromPoupluationViewer,
            this, &QOpUserDefinedInitNode::updateDVec);
    connect(m_viwer, &PopulationViewer::updateJsonFromPopViewr,
            this, &QOpUserDefinedInitNode::updateFromJson);
    connect(m_viwer, &PopulationViewer::updateFilePath,
            this, &QOpUserDefinedInitNode::updateFilePath);

    m_dialog->addToolWidget(m_viwer, "Load Initial Population");
}

QOpUserDefinedInitNode::~QOpUserDefinedInitNode()
{
    delete m_dialog;
}

QString QOpUserDefinedInitNode::filePath() const
{
    return m_filePath;
}

void QOpUserDefinedInitNode::updateProcessState(ProcessState state)
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
}

void QOpUserDefinedInitNode::setFilePath(QString path)
{
    m_filePath = path;
}

void QOpUserDefinedInitNode::readDataProperties(QXmlStreamReader &xmlReader)
{
    xmlReader.readNextStartElement(); //todo remove
    TObject* thisOp = static_cast<TObject*>(data());
    while(!xmlReader.isEndElement() || !(xmlReader.name() == "Properties")) {
        xmlReader.readNext();
        if(xmlReader.isStartElement()) {
            if(xmlReader.name() != "TigonOperator") {
                assignPropertyToObject(thisOp, &xmlReader);
            }
        }
    }
}

void QOpUserDefinedInitNode::writeDataProperties(QXmlStreamWriter &xmlWriter)
{
    xmlWriter.writeAttribute("ProcessType", "OPERATOR_NODE");
    TObject* thisOp = static_cast<TObject*>(data());
    xmlWriter.writeTextElement("TigonOperator",
                               QString::fromStdString(thisOp->className()));
    propertiesToXml(thisOp, &xmlWriter);
}

void QOpUserDefinedInitNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)
    UserDefinedInit* thisOp = static_cast<UserDefinedInit*>(data());
    if(thisOp) {
        m_viwer->setFilePath(m_filePath);
        m_dialog->setup(thisOp);
        m_dialog->show();
        m_dialog->raise();
    }
}

void QOpUserDefinedInitNode::updateDVec(const QVector<QVector<qreal> > &dVecData)
{
    using Tigon::TVector;
    UserDefinedInit* thisOp = static_cast<UserDefinedInit*>(data());
    TVector<TVector<qreal> > vecData;
    vecData.resize(dVecData.size());
    foreach (const QVector<qreal>& v, dVecData) {
        vecData.push_back(v.toStdVector());
    }
    thisOp->defineInitialDVecs(vecData);
    m_dialog->upadtePropertyDisplayData();
}

void QOpUserDefinedInitNode::updateFromJson(const QJsonObject &json)
{
    Q_UNUSED(json);
}

void QOpUserDefinedInitNode::updateFilePath(QString filePath)
{
    m_filePath = filePath;
    UserDefinedInit* thisOp = static_cast<UserDefinedInit*>(data());
    thisOp->defineInitialSetFromFile(m_filePath.toStdString());
    m_dialog->upadtePropertyDisplayData();
}
