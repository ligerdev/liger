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
#include <qtigon/masterendnode.h>
#include <qtigon/dialogs/endnodedialog.h>
#include <qtigon/qtigonutils.h>

#include <tigon/Tigon.h>
using Tigon::TObject;
using namespace Tigon::Operators;

#include <algorithm>

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMessageBox>
#include <QGraphicsSceneMouseEvent>

namespace QTigon {

MasterEndNode::MasterEndNode()
    : m_dialog(new EndNodeDialog())
    , m_useIter(false)
    , m_useBudget(false)
    , m_pauseByIter(false)
    , m_pauseByBudget(false)
{
    m_text         = QString("MEN");
    m_procType     = Designer::MASTER_END_NODE;

    m_outlineColor = Qt::darkBlue;
    setNumberOfOutputs(0);
    setNumberOfInputs(1);
    setData(new ITermination);

    setSvg(":/qtigon/images/master_end_node.svg");

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();

    connect(m_dialog, SIGNAL(accepted()), this, SLOT(acceptChangesSlot()));
}

MasterEndNode::~MasterEndNode()
{
    delete m_dialog;
}

void MasterEndNode::updateProcessState(Designer::ProcessState state)
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

void MasterEndNode::readDataProperties(QXmlStreamReader &xmlReader)
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

    // Master end node specific
    setMaxIterations(Tigon::convertStringTo<int>(thisOp->propertyValue("maxIter")));
    setBudget(Tigon::convertStringTo<int>(thisOp->propertyValue("budget")));
}

void MasterEndNode::writeDataProperties(QXmlStreamWriter &xmlWriter)
{
    xmlWriter.writeAttribute("ProcessType", "OPERATOR_NODE");
    TObject* thisOp = static_cast<TObject*>(data());
    xmlWriter.writeTextElement("TigonOperator",
                               QString::fromStdString(thisOp->className()));
    propertiesToXml(thisOp, &xmlWriter);
}

void MasterEndNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        ITermination* op = static_cast<ITermination*>(data());
        m_dialog->setNiter(op->maxIteration());
        m_dialog->setBudget(op->budget());
        m_dialog->setUseBugdget(isUseBudget());
        m_dialog->setUseIter(isUseIteration());
        m_dialog->setPauseByIteration(isPauseByIteration());
        m_dialog->setPauseByBudget(isPauseByBudget());
        m_dialog->show();
        m_dialog->raise();
    }
}

QString MasterEndNode::populationsLogPath() const
{
    return m_dialog->populationsLogPath();
}

void MasterEndNode::setPopulationsLogPath(const QString &logPath)
{
    m_dialog->setPopulationsLogPath(logPath);
}

QString MasterEndNode::evaluationsLogPath() const
{
    return m_dialog->evaluationsLogPath();
}

void MasterEndNode::setEvaluationsLogPath(const QString &logPath)
{
    m_dialog->setEvaluationsLogPath(logPath);
}

bool MasterEndNode::logPopulations() const
{
    return m_dialog->isLoggingPopulation();
}

void MasterEndNode::setLogPopulations(bool logPopulation)
{
    m_dialog->setLoggingPopulation(logPopulation);
}

bool MasterEndNode::keepPopulationsHistory() const
{
    return m_dialog->isKeepPopulationsHistory();
}

void MasterEndNode::setkeepPopulationsHistory(bool keepHistory)
{
    m_dialog->setKeepPopulationsHistory(keepHistory);
}

bool MasterEndNode::logEvaluations() const
{
    return m_dialog->isLoggingEvaluation();
}

void MasterEndNode::setLogEvaluations(bool logEvaluation)
{
    m_dialog->setLoggingEvaluation(logEvaluation);
}

void MasterEndNode::setPauseByBudget(bool pauseByBudget)
{
    m_pauseByBudget = pauseByBudget;
}

void MasterEndNode::setPauseByIteration(bool pauseByIter)
{
    m_pauseByIter = pauseByIter;
}

bool MasterEndNode::isUseBudget() const
{
    return m_useBudget;
}

void MasterEndNode::setUseBudget(bool useBudget)
{
    m_useBudget = useBudget;
}

bool MasterEndNode::isPauseByIteration() const
{
    return m_pauseByIter;
}

QString MasterEndNode::iterPausePoints() const
{
    return m_dialog->iterPausePoints();
}

QVector<qint64> MasterEndNode::iterPausePointVec() const
{
    QStringList content = iterPausePoints().split(";");
    QVector<qint64> res;
    foreach(const QString& str, content) {
        res.append(str.toInt());
    }
    std::sort(res.begin(), res.end());
    if(iterPausePointType() == "Percentage") {
        for(int i=0; i<res.size(); i++) {
            res[i] = res[i] * maxIterations() / 100;
        }
    }
    return res;
}

void MasterEndNode::setIterPausePoints(const QString &points)
{
    m_dialog->setIterPausePoints(points);
}

bool MasterEndNode::isPauseByBudget() const
{
    return m_pauseByBudget;
}

QString MasterEndNode::budgetPausePoints() const
{
    return m_dialog->budgetPausePoints();
}

QVector<qint64> MasterEndNode::budgetPausePointVec() const
{
    QStringList content = budgetPausePoints().split(";");
    QVector<qint64> res;
    foreach(const QString& str, content) {
        res.append(str.toInt());
    }
    std::sort(res.begin(), res.end());
    if(budgetPausePointType() == "Percentage") {
        for(int i=0; i<res.size(); i++) {
            res[i] = res[i] * budget() / 100;
        }
    }
    return res;
}

void MasterEndNode::setBudgetPausePoints(const QString &points)
{
    m_dialog->setBudgetPausePoints(points);
}

QString MasterEndNode::iterPausePointType() const
{
    return m_dialog->iterPauseType();
}

void MasterEndNode::setIterPausePointType(const QString &points)
{
    m_dialog->setIterPauseType(points);
}

QString MasterEndNode::budgetPausePointType() const
{
    return m_dialog->budgetPauseType();
}

void MasterEndNode::setBudgetPausePointType(const QString &points)
{
    m_dialog->setBudgetPauseType(points);
}

bool MasterEndNode::isUseIteration() const
{
    return m_useIter;
}

void MasterEndNode::setUseIteration(bool useIter)
{
    m_useIter = useIter;
}

void MasterEndNode::acceptChangesSlot()
{
    ITermination* op = static_cast<ITermination*>(data());
    bool useIter = m_dialog->isUseIteration();
    setUseIteration(useIter);
    if(useIter) {
        setMaxIterations(m_dialog->nIter());
        op->TP_defineMaxIteration(m_dialog->nIter());
    } else {
        op->resetMaxIter();
    }

    bool useBudget = m_dialog->isUseBuget();
    setUseBudget(useBudget);
    if(useBudget) {
        setBudget(m_dialog->budget());
        op->TP_defineBudget(m_dialog->budget());
    } else {
        op->resetBudget();
    }

    setPauseByIteration(m_dialog->isPauseByIteration());
    setPauseByBudget(m_dialog->isPauseByBudget());
    emit nodePropertiesChangedSignal();
}

} // namespace QTigon
