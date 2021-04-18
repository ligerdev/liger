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
#include <qtigon/operators/problemgenerator/qopprobgeneratornode.h>
#include <qtigon/dialogs/qopprobformulationnodeform.h>
#include <qtigon/qtigonutils.h>

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <tigon/Operators/Formulations/ProblemGenerator/ProblemGenerator.h>
#include <tigon/tigonconstants.h>
#include <tigon/Engine/TigonEngine.h>
#include <tigon/Engine/TigonEngineConstants.h>

#include <QFile>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

using namespace QTigon;
using namespace Designer;
using namespace Tigon;
using namespace Tigon::Operators;
using namespace Tigon::Engine::Constants;

QOpProbGeneratorNode::QOpProbGeneratorNode()
{
    m_text         = QString("PGN");
    m_procType     = PROBLEM_NODE;

    m_outlineColor = Qt::darkBlue;
    setNumberOfOutputs(1);
    setNumberOfInputs(1);
    setData(new Tigon::Operators::ProblemGenerator());

    QFile img_file(":/qtigon/images/qopprobgeneratornode.svg");
    if(img_file.exists()) {
        setSvg(":/qtigon/images/qopprobgeneratornode.svg");
    } else {
        setSvg(":/qtigon/images/default_qopnode.svg");
    }
    img_file.close();

    m_qOpForm = new QOpProbFormulationNodeForm();
    connect(m_qOpForm, SIGNAL(opDataChanged()),
            this, SIGNAL(nodePropertiesChangedSignal()));

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();
}

QOpProbGeneratorNode::~QOpProbGeneratorNode()
{
    if(m_qOpForm) {
        delete m_qOpForm;
    }
}

void QOpProbGeneratorNode::updateProcessState(ProcessState state)
{
    switch(state) {
    case Designer::ProcessState::NODE_IN_QUEUE:
        m_qOpForm->setInputDisabled();
        break;
    case Designer::ProcessState::NODE_RUNNING:
        m_qOpForm->setInputDisabled();
        break;
    case Designer::ProcessState::NODE_DONE:
        m_qOpForm->setInputDisabled(false);
        break;
    default:
        m_qOpForm->setInputDisabled(false);
        break;
    }
    IProcessNode::updateProcessState(state);
}

void QOpProbGeneratorNode::readDataProperties(QXmlStreamReader &xmlReader)
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
    ProblemGenerator* pg = static_cast<ProblemGenerator*>(data());
    ProblemDefinitionStatus status = pg->processFormulation();
    if(status != FullyDefined) {
        // Error. Throw exception: probelm not fully defined
        qDebug() << "\n\n\n\n ================= \n "  << "Not fully defined" << "\n ================= \n\n\n\n ";
    }
}

void QOpProbGeneratorNode::writeDataProperties(QXmlStreamWriter &xmlWriter)
{
    xmlWriter.writeAttribute("ProcessType", "PROBLEM_NODE");
    TObject* thisOp = static_cast<TObject*>(data());
    xmlWriter.writeTextElement("TigonOperator",
                               QString::fromStdString(thisOp->className()));
    propertiesToXml(thisOp, &xmlWriter);
}

void QOpProbGeneratorNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    Tigon::Operators::ProblemGenerator* thisOp =
            static_cast<Tigon::Operators::ProblemGenerator*>(data());
    if(thisOp) {
        m_qOpForm->setup(thisOp);
        m_qOpForm->show();
        m_qOpForm->raise();
    }
}
