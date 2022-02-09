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
#include <qtigon/engine/qtigonengine.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/ilogmanager.h>
#include <designer/designerconstants.h>

#include <tigon/Tigon.h>

#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>

using namespace QTigon;
using namespace Designer;
using namespace Tigon;
using namespace Json;

QTigonEngine::QTigonEngine()
    : m_engine(Tigon::TigonEngine::instance())
    , m_populationsLog(0)
    , m_evaluationsLog(0)
    , m_isLogPopulations(false)
    , m_isLogEvaluations(false)
    , m_isLogNetworkPopulation(false)
    , m_isLogProb(false)
    , m_isLogProgress(false)
    , m_isKeepPopulationsHistory(false)
{
    connect(this, &QTigonEngine::engineError,
            this, &QTigonEngine::errorHandling);
}

QTigonEngine::~QTigonEngine()
{
    // No need to delete engine
    m_engine->clearFlow();
}

bool QTigonEngine::removeAllNodes()
{
    IEngine::removeAllNodes();
    m_engine->clearFlow();
    return true;
}

/*!
 * \brief QTigonEngine::loadDesign
 * \todo  Send newly created tigon operators back to processnodes
 */
bool QTigonEngine::loadDesign(QList<IProcessNode *> nodes, QString ligerFile)
{
    bool status = true;

    ///\note This will cause problem for the network protocol. The network log managers
    /// are attached to the engine when network protocol starts and I expect them
    /// to remain in the engine until the network protocol stops.
    /// So it is better that qtigonengine deletes the log managers created by its
    /// own however leaves log managers created by others alone.

    //unloadLogManagers();


    // Remove previous log managers and reset flags
    resetLogs(); // Shual You may want to change the function names

    // Append process nodes
    foreach(IProcessNode* p, nodes) {
        status = addProcessNode(p);
        if(status == false) {
            return status;
        }

        // Handle the Master End Node
        if(p->processType() ==  Designer::ProcessType::MASTER_END_NODE) {
            if(p->logPopulations()) {
                m_populationsLog = new ILogManager(this, p->populationsLogPath());
                m_populationsLog->setLogPopulations(true);
                m_populationsLog->setKeepPopulationsHistory(p->keepPopulationsHistory());

                appendLogManager(m_populationsLog);
            }
            if(p->logEvaluations()) {
                m_evaluationsLog = new ILogManager(this, p->evaluationsLogPath());
                m_evaluationsLog->setLogEvaluations(true);
                appendLogManager(m_evaluationsLog);
            }
            setMasterEndNodePresent();
        }

        // Check the Master Start Node
        if(p->processType() == Designer::ProcessType::MASTER_START_NODE) {
            setMasterStartNodePresent();
        }
    }

    status = m_engine->loadFlowFromFile(ligerFile.toStdString());
    if(status == false) {
        return status;
    }

    foreach(IProcessNode* p, m_sinkNodes) {
        if(m_engine->currentFlow()->size() > 0) {
            // Only give base
            IPSet* data = m_engine->currentFlow()->node(0);
            p->dataReceivedFromEngine(data);
            connect(this, &QTigonEngine::dataUpdated, p, &IProcessNode::dataUpdatedByEngine);
        } else {
            status = false;
            break;
        }
    }

    /// Enable Logging
    qDebug() << "number of attached log managers: " << logManagers().size()
             << "They are: ";
    if(logManagers().size() > 0) {
        for(int i=0; i<logManagers().size(); i++) {
            qDebug() << logManagers()[i]->metaObject()->className();
            if(logManagers()[i]->logPopulations() && !m_isLogPopulations) {
                m_isLogPopulations = true;
                if(logManagers()[i]->keepPopulationsHistory() && !m_isKeepPopulationsHistory) {
                    m_isKeepPopulationsHistory = true;
                }
            }

            if(logManagers()[i]->logEvaluations() && !m_isLogEvaluations) {
                m_isLogEvaluations = true;
            }

            if(logManagers()[i]->logNetworkPopulation() && !m_isLogNetworkPopulation) {
                m_isLogNetworkPopulation = true;
            }

            if(logManagers()[i]->logProb() && !m_isLogProb) {
                m_isLogProb = true;
            }

            if(logManagers()[i]->logProgress() && !m_isLogProgress) {
                m_isLogProgress = true;
            }
        }

        m_engine->currentFlow()->finalNode()->log()
                ->defineLogEvaluation(m_isLogEvaluations);
        m_engine->currentFlow()->finalNode()->log()
                ->defineLogPopulation(m_isLogPopulations);
    }

    return status;
}

/*!
 * \brief QTigonEngine::engine_loop
 * \todo  Run multiflows at the same time
 */
void QTigonEngine::engine_loop()
{
    if(m_engine) {
        m_engine->setCurrentIndex(0);
        OptimizationLinearFlow* flow = m_engine->currentFlow();
        Log::LogManagerSPtr log = flow->finalNode()->log();

        // Problem log
        if(m_isLogProb) {
            QJsonObject jprob = toQJsonObject(problemToJsonObject(flow->finalNode()->problem()));
            emit signalProblemDefinition(jprob);
        }

        // Process log
        QJsonObject jprocess;
        if(m_isLogProgress) {
            jprocess["Budget"]        = flow->budget();
            jprocess["MaxIteration"]  = flow->maxIteration();
        }

        while(!flow->isTerminate()) {
            if(isRunning()) {
                flow->incrementIteration();
                qDebug() << "start: " << flow->currentIteration();

                /// [] Report Iteration
                if(flow->maxIteration() > 0) {
                    emit signalIterationCounter(flow->currentIteration());
                }

                /// [] Evaluate
                try {
                    flow->evaluate();
                } catch(const TException &e) {
                    QString errMessage(e.what());
                    emit engineError(errMessage);
                    return;
                } catch(const std::exception& e) {
                    QString errMessage("Tigon has encounter a standard error: ");
                    errMessage += QString(e.what());
                    emit engineError(errMessage);
                    return;
                }

                /// [] Generate Logs
                if(m_isLogPopulations) {
                    logPopulations(flow);
                }
                if(m_isLogEvaluations) {
                    emit signalEvaluationsLog(toQJsonObject(log->evaluationsLog()));
                }

                /// \todo YIMING please check that this maintains the
                ///       functionality you need
                if(m_isLogNetworkPopulation) {
                    logNetworkPopulation(flow);
                }

                // Progress log
                if(m_isLogProgress) {
                    jprocess["UsedBudget"] = flow->usedBudget();
                    jprocess["Iteration"]  = flow->currentIteration();
                    jprocess["Terminated"] = flow->isTerminate();
                    emit signalProcess(jprocess);
                }

                /// [] Report Budget
                if(flow->budget() > 0) {
                    emit signalBudgetCounter(flow->usedBudget());
                }
                qDebug() << "finished: " << flow->currentIteration();
            } else if(isPaused()) {
                if(m_isLogProgress) {
                    jprocess["Paused"] = isPaused();
                    emit signalProcess(jprocess);
                }
                qDebug() << "Engine has been paused by user";
                return;
            } else {
                if(m_isLogProgress) {
                    QJsonObject jprocess;
                    jprocess["Interrupted"] = true;
                    emit signalProcess(jprocess);
                }
                qDebug() << "Engine has been interrupted by user";
                return;
            }
            emit dataUpdated();
        }
    }
}

//void QTigonEngine::unloadLogManagers()
//{
//    resetLogFlags();
//    IEngine::unloadLogManagers();
//}

void QTigonEngine::errorHandling(const QString &err)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setWindowTitle("Tigon Engine");
    msgBox.setText("Tigon has encounter an error and needs to be terminated.");
    msgBox.setInformativeText(err);
    msgBox.exec();

    logPopulations(m_engine->currentFlow());
}

void QTigonEngine::logPopulations(OptimizationLinearFlow* flow)
{
    Log::LogManagerSPtr log = flow->finalNode()->log();

    Json::JsonObject customFields;
    customFields["UsedBudget"] = flow->usedBudget();
    customFields["Iteration"]  = flow->currentIteration();

    if(!m_isKeepPopulationsHistory) {
        log->clearPopulationsLog();
    }
    log->logPopulation(flow->finalNode()->set(0), "",
                       customFields); // main set
    emit signalPopulationsLog(toQJsonObject(log->populationsLog()));
}

void QTigonEngine::logNetworkPopulation(OptimizationLinearFlow* flow)
{
    Log::LogManager log;
    log.clearLog();

    Json::JsonObject customFields;
    customFields["UsedBudget"] = flow->usedBudget();
    customFields["Iteration"]  = flow->currentIteration();

    log.logPopulation(flow->finalNode()->set(0), "", customFields); // main set
    emit signalNetworkPopulationLog(toQJsonObject(log.populationsLog()));
}

QJsonObject QTigonEngine::toQJsonObject(const JsonObject &obj) const
{
    QJsonObject qobj;
    TStringList keys = obj.keys();
    for(int i=0; i<keys.size(); i++) {
        TString key = keys[i];
        if(obj[key].isNull() || obj[key].isUndefined()) {
            continue;
        }

        if(obj[key].isBool()) {
            qobj[QString::fromStdString(key)] = obj[key].toBool();
            continue;
        }

        if(obj[key].isDouble()) {
            qobj[QString::fromStdString(key)] = obj[key].toDouble();
            continue;
        }

        if(obj[key].isString()) {
            qobj[QString::fromStdString(key)] =
                    QString::fromStdString(obj[key].toString());
            continue;
        }

        if(obj[key].isArray()) {
            QJsonArray array = toQJsonArray(obj[key].toArray());
            qobj[QString::fromStdString(key)] = array;
            continue;
        }

        if(obj[key].isObject()) {
            qobj[QString::fromStdString(key)] =
                    toQJsonObject(obj[key].toObject());
            continue;
        }
    }
    return qobj;
}

QJsonArray QTigonEngine::toQJsonArray(const JsonArray &array) const
{
    QJsonArray qarray;
    std::transform(array.begin(), array.end(), std::back_inserter(qarray),
                   [this](const JsonValue& val) {
        if(val.isNull() || val.isUndefined()) {
            return QJsonValue();
        }
        if(val.isBool()) {
            return QJsonValue(val.toBool());
        }
        if(val.isDouble()) {
            return QJsonValue(val.toDouble());
        }
        if(val.isString()) {
            return QJsonValue(QString::fromStdString(val.toString()));
        }

        if(val.isArray()) {
            return QJsonValue(this->toQJsonArray(val.toArray()));
        }

        if(val.isObject()) {
            return QJsonValue(this->toQJsonObject(val.toObject()));
        }
        return QJsonValue();
    });
    return qarray;
}

void QTigonEngine::resetLogs()
{
    if(m_populationsLog) {
        removeLogManager(m_populationsLog);
        m_populationsLog = 0;
    }
    if(m_evaluationsLog) {
        removeLogManager(m_evaluationsLog);
        m_evaluationsLog = 0;
    }

    m_isLogPopulations = false;
    m_isLogEvaluations = false;
    m_isLogNetworkPopulation = false;
    m_isLogProb = false;
    m_isLogProgress = false;
    m_isKeepPopulationsHistory = false;
}
