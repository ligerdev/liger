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
#include <designer/iengine/iengine.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/icountinglabel.h>
#include <designer/iengine/ilogmanager.h>
#include <coreplugin/icore.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/progressmanager/progressmanager.h>
#include <extensionsystem/iplugin.h>
#include <designer/iengine/icountinglabel.h>

#include <QTextCursor>
#include <QMutexLocker>
#include <QMessageBox>
#include <QDebug>

using namespace Core;

namespace Designer {

IEngine::IEngine()
    : m_masterStartNode(0)
    , m_masterEndNode(0)
    , m_masterStartNodePresent(false)
    , m_masterEndNodePresent(false)
    , m_iterationProgressInterface(0)
    , m_budgetProgressInterface(0)
    , m_iterationFutureProgress(0)
    , m_engineStatus(EngineStatus::IDLE)
    , m_maxIteration(0)
    , m_budget(0)
    , m_currentIteration(0)
    , m_usedBudget(0)
{
    connect(this, SIGNAL(signalIterationCounter(int)),
            this, SLOT(receivedIterationCount(int)));
    connect(this, SIGNAL(signalBudgetCounter(int)),
            this, SLOT(receivedBudgetCount(int)));
    connect(&m_iterationProgressWatcher, SIGNAL(canceled()), this, SLOT(exit()));
    connect(&m_iterationProgressWatcher, SIGNAL(finished()), this, SLOT(finished()));
    connect(&m_iterationProgressWatcher, SIGNAL(paused()), this, SLOT(pause()));

    connect(&m_budgetProgressWatcher, SIGNAL(canceled()), this, SLOT(exit()));
    connect(&m_budgetProgressWatcher, SIGNAL(finished()), this, SLOT(finished()));
    connect(&m_budgetProgressWatcher, SIGNAL(paused()), this, SLOT(pause()));
}

IEngine::~IEngine()
{
    if(isRunning()) {
        exit();
    }
    unloadDesign();
    unloadLogManagers();

    delete m_iterationProgressInterface;
    delete m_budgetProgressInterface;
}

/*!
 * \brief Load xml file and create a workflow.
 * \param ligerFile
 * \return true on success; otherwise false.
 */
bool IEngine::loadDesign(QList<IProcessNode*> nodes, QString ligerFile)
{
    bool status = true;
    foreach(IProcessNode* p, nodes) {
        status = addProcessNode(p);
        if(status == false)
            return status;
    }
    Q_UNUSED(ligerFile)
    return true;
}

bool IEngine::unloadDesign()
{
    m_processNodes.clear();
    m_portLinks.clear();

    m_masterStartNode = 0;
    m_masterEndNode   = 0;

    m_sourceNodes.clear();
    m_sinkNodes.clear();
    m_masterStartNodePresent = false;
    m_masterEndNodePresent   = false;

    return true;
}

void IEngine::unloadLogManagers()
{
    qDeleteAll(m_logManagers);
    m_logManagers.clear();
}

bool IEngine::addProcessNode(IProcessNode* node)
{
    // Can have only one master start and master end nodes.
    switch(node->processType()) {
    case MASTER_START_NODE:
        if(m_masterStartNode == 0) {
            m_masterStartNode = node;
        } else {
            return false;
        }
        break;

    case MASTER_END_NODE:
        if(m_masterEndNode == 0) {
            m_masterEndNode = node;
            m_maxIteration = node->maxIterations();
            m_budget = node->budget();
        } else {
            return false;
        }
        break;

    case SOURCE_NODE:
        m_sourceNodes.append(node);
        break;

    case SINK_NODE:
        m_sinkNodes.append(node);
        break;
    default:
        break;
    }

    m_processNodes.append(node);
    return true;
}

bool IEngine::removeProcessNode(IProcessNode* node)
{
    int idx = m_processNodes.indexOf(node);
    if(idx != -1) {
        if(m_masterStartNode == m_processNodes[idx]) {
            m_masterStartNode = 0;
            m_masterStartNodePresent = false;
            m_processNodes.removeAt(idx);
        }
        else if(m_masterEndNode == m_processNodes[idx]) {
            m_masterEndNode = 0;
            m_masterEndNodePresent = false;
            m_processNodes.removeAt(idx);
        }
        else {
            int idxs = -1;
            switch(node->processType()) {
            case SOURCE_NODE:
                idxs = m_sourceNodes.indexOf(node);
                if(idxs != -1) {
                    m_sourceNodes.removeAt(idxs);
                    break;
                }

            case SINK_NODE:
                idxs = m_sinkNodes.indexOf(node);
                if(idxs != -1) {
                    m_sinkNodes.removeAt(idxs);
                    break;
                }

            default:
                break;
            }

            m_processNodes.removeAt(idx);
        }
    }

    return true;
}

bool IEngine::removeAllNodes()
{
    m_masterStartNode = 0;
    m_masterEndNode = 0;

    for(int i=0; i<m_processNodes.size(); i++) {
        m_processNodes.removeAt(i);
    }

    for(int i=0; i<m_sourceNodes.size(); i++) {
        m_sourceNodes.removeAt(i);
    }

    for(int i=0; i<m_sinkNodes.size(); i++) {
        m_sinkNodes.removeAt(i);
    }

    m_masterStartNodePresent = false;
    m_masterEndNodePresent   = false;

    return true;
}

void IEngine::setMasterStartNodePresent()
{
    m_masterStartNodePresent = true;
    return;
}

void IEngine::setMasterEndNodePresent()
{
    m_masterEndNodePresent = true;
    return;
}


bool IEngine::masterNodesPresent()
{
    /// Check for Presence of Master Start and End Nodes
    if (!m_masterEndNodePresent){
        QMessageBox::critical(0, QString("Liger Engine"),
                              tr("There is no End node present and connected."),
                              QMessageBox::Ok);
        return false;
    }
    if (!m_masterStartNodePresent){
        QMessageBox::critical(0, QString("Liger Engine"),
                              tr("There is no Start node present and connected."),
                              QMessageBox::Ok);
        return false;
    }
    return true;
}
/*!
 * \brief Start the execution process.
 */
void IEngine::start()
{
    if(isRunning()) {
        return;
    } else if(isPaused()) {
        return;
    } else if(!masterNodesPresent()) {
        return;
    } else {
        /// Termination
        if(!m_masterEndNode->isUseBudget() && !m_masterEndNode->isUseIteration()){
            QMessageBox::critical(0, QString("Liger Engine"),
                                  tr("Termination criterion not defined."),
                                  QMessageBox::Ok);
            return;
        }

        if(m_masterEndNode->isUseIteration()) {
            m_iterationProgressInterface = new QFutureInterface<void>;
            qDebug() << "Use iteration" << m_iterationProgressInterface;
            m_iterationProgressWatcher.setFuture(m_iterationProgressInterface->future());
            m_iterationProgressInterface->setProgressRange(0, m_maxIteration);
            m_iterationFutureProgress = ProgressManager::addTask(
                        m_iterationProgressInterface->future(),
                        tr("Iterations"),
                        Designer::Constants::TASK_ENGINE_RUN);
            ICountingLabel* label = new ICountingLabel();
            label->setMaxCount(m_maxIteration);
            m_iterationFutureProgress->setWidget(label);
            m_iterationProgressInterface->reportStarted();
        }

        if(m_masterEndNode->isUseBudget()) {
            m_budgetProgressInterface = new QFutureInterface<void>;
            qDebug() << "Use budget" << m_budgetProgressInterface;
            m_budgetProgressWatcher.setFuture(m_budgetProgressInterface->future());
            m_budgetProgressInterface->setProgressRange(0, m_budget);
            m_budgetFutureProgress = ProgressManager::addTask(
                        m_budgetProgressInterface->future(),
                        tr("Budget"),
                        Designer::Constants::TASK_ENGINE_RUN);
            ICountingLabel* label = new ICountingLabel();
            label->setMaxCount(m_budget);
            m_budgetFutureProgress->setWidget(label);
            m_budgetProgressInterface->reportStarted();
        }

        /// Pause
        if(m_masterEndNode->isPauseByIteration()) {
            m_pauseByIter = m_masterEndNode->iterPausePointVec();
            qDebug() << "Pause by iteration: " << m_pauseByIter;
        }

        if(m_masterEndNode->isPauseByBudget()) {
            m_pauseByBudget = m_masterEndNode->budgetPausePointVec();
            qDebug() << "Pause by budget: " << m_pauseByBudget;
        }

        m_result = QtConcurrent::run(this, &IEngine::run);
        updateProcessNodeStatus(ProcessState::NODE_RUNNING);
        emit engineStarted();
    }
}

/*!
 * \brief IEngine::pause
 */
void IEngine::pause()
{
    qDebug() << "Pause button pressed";
    if(isRunning()) {
        m_engineStatus = EngineStatus::PAUSED;
        if(m_iterationProgressInterface) {
            m_iterationProgressInterface->setPaused(true);
            m_iterationProgressInterface->waitForFinished();
            m_iterationFutureProgress->setTitle("Engine Paused");
        }
        if(m_budgetProgressInterface) {
            m_budgetProgressInterface->setPaused(true);
            m_budgetProgressInterface->waitForFinished();
            m_budgetFutureProgress->setTitle("Engine Paused");
        }
        updateProcessNodeStatus(ProcessState::NODE_IN_QUEUE);
        emit enginePaused();
    }
}

/*!
 * \brief IEngine::resume
 */
void IEngine::resume()
{
    qDebug() << "resume button pressed";
    if(isPaused()) {
        if(m_iterationProgressInterface) {
            delete m_iterationProgressInterface;
            m_iterationProgressInterface = new QFutureInterface<void>;
            m_iterationProgressWatcher.setFuture(m_iterationProgressInterface->future());
            m_iterationProgressInterface->setProgressRange(0, m_maxIteration);
            m_iterationFutureProgress = ProgressManager::addTask(
                        m_iterationProgressInterface->future(),
                        tr("Iterations"),
                        Designer::Constants::TASK_ENGINE_RUN);

            ICountingLabel* label = new ICountingLabel(m_currentIteration);
            label->setMaxCount(m_maxIteration);
            m_iterationFutureProgress->setWidget(label);

            m_iterationProgressInterface->reportStarted();
            m_iterationProgressInterface->setProgressValue(m_currentIteration);
        }

        if(m_budgetProgressInterface) {
            delete m_budgetProgressInterface;
            m_budgetProgressInterface = new QFutureInterface<void>;
            m_budgetProgressWatcher.setFuture(m_budgetProgressInterface->future());
            m_budgetProgressInterface->setProgressRange(0, m_budget);
            m_budgetFutureProgress = ProgressManager::addTask(
                        m_budgetProgressInterface->future(),
                        tr("Budget"),
                        Designer::Constants::TASK_ENGINE_RUN);

            ICountingLabel* label = new ICountingLabel(m_usedBudget);
            label->setMaxCount(m_budget);
            m_budgetFutureProgress->setWidget(label);

            m_budgetProgressInterface->reportStarted();
            m_budgetProgressInterface->setProgressValue(m_usedBudget);
        }

        m_result = QtConcurrent::run(this, &IEngine::run);
        updateProcessNodeStatus(ProcessState::NODE_RUNNING);
        emit engineResumed();
    }
}

/*!
 * \brief Terminate the execution process. A signal will be sent to terminate
 *        the workflow running on the other thread.
 */
void IEngine::exit()
{
    qDebug() << "exit button pressed";
    if(isRunning()) {
        m_engineStatus = EngineStatus::IDLE;
        if(m_iterationProgressInterface) {
            m_iterationProgressInterface->reportCanceled();
            m_iterationProgressInterface->waitForFinished();
            delete m_iterationProgressInterface;
            m_iterationProgressInterface = 0;
        }
        if(m_budgetProgressInterface) {
            m_budgetProgressInterface->reportCanceled();
            m_budgetProgressInterface->waitForFinished();
            delete m_budgetProgressInterface;
            m_budgetProgressInterface = 0;
        }
        m_result.waitForFinished();
    } else if(isPaused()) {
        m_engineStatus = EngineStatus::IDLE;
        if(m_iterationProgressInterface) {
            m_iterationFutureProgress->close();
            delete m_iterationProgressInterface;
            m_iterationProgressInterface = 0;
        }
        if(m_budgetProgressInterface) {
            m_budgetFutureProgress->close();
            delete m_budgetProgressInterface;
            m_budgetProgressInterface = 0;
        }
        m_result.waitForFinished();
    }
    updateProcessNodeStatus(ProcessState::NODE_DONE);
    emit engineExit();
    finished();
}

void IEngine::finished()
{
    m_engineStatus = EngineStatus::IDLE;
    updateProcessNodeStatus(ProcessState::NODE_DONE);
    unloadDesign();
    emit engineFinished();
}

void IEngine::receivedIncomingData(const QMap<QString, QVector<QVector<qreal> > >& dataMap,
                                   const QMap<QString, QStringList>& IDMap,
                                   const QMap<QString, QVector<int>>& runMap)
{
    handleIncomingData(dataMap, IDMap, runMap);
}

void IEngine::receivedIterationCount(int iter)
{
    // Currently finished iterations
    if(m_iterationProgressInterface) {
    m_currentIteration = iter;
    m_iterationProgressInterface->setProgressValue(iter);
    ICountingLabel* label =
            static_cast<ICountingLabel*>(m_iterationFutureProgress->widget());
    label->updateCount(iter);
    if(m_pauseByIter.size() > 0 && iter >= m_pauseByIter.first()) {
        pause();
        m_pauseByIter.removeFirst();
        QMessageBox::information(0, QString("Engine Info")
                                 , QString("Engine paused at iteration %1.")
                                 .arg(QString::number(iter))
                                 , QMessageBox::Ok, QMessageBox::Ok);
    }
    }
}

void IEngine::receivedBudgetCount(int budget)
{
    // Currently finished budget
    if(m_budgetProgressInterface) {
    m_budgetProgressInterface->setProgressValue(budget);
    m_usedBudget = budget;
    ICountingLabel* label =
            static_cast<ICountingLabel*>(m_budgetFutureProgress->widget());
    label->updateCount(budget);
    if(m_pauseByBudget.size() > 0 && budget >= m_pauseByBudget.first()) {
        pause();
        m_pauseByBudget.removeFirst();
        QMessageBox::information(0, QString("Engine Info")
                                 , QString("Engine paused after %1 budget.")
                                 .arg(QString::number(budget))
                                 , QMessageBox::Ok, QMessageBox::Ok);
    }
    }
}

void IEngine::updateProcessNodeStatus(ProcessState state)
{
    foreach (IProcessNode* node, m_processNodes) {
        node->updateProcessState(state);
    }
}

QList<ILogManager *> IEngine::logManagers() const
{
    return m_logManagers;
}

void IEngine::appendLogManager(ILogManager *log)
{
    if(!log) {
        return;
    }
    if(!m_logManagers.contains(log)) {
        m_logManagers.append(log);
        qDebug() << Q_FUNC_INFO << log->metaObject()->className();
    }

    // Connect the proper signals to logmanager
    if(log->logPopulations()) {
        connect(this, &IEngine::signalPopulationsLog,
                log, &ILogManager::writeLogSlot);
    }
    if(log->logEvaluations()) {
        connect(this, &IEngine::signalEvaluationsLog,
                log, &ILogManager::writeLogSlot);
    }

    // Network log signals
    if(log->logNetworkPopulation()) {
        connect(this, &IEngine::signalNetworkPopulationLog,
                log, &ILogManager::writeLogSlot);
    }
    if(log->logProb()) {
        connect(this, &IEngine::signalProblemDefinition,
                log, &ILogManager::writeLogSlot);
    }
    if(log->logProgress()) {
        connect(this, &IEngine::signalProcess,
                log, &ILogManager::writeLogSlot);
    }
}

void IEngine::removeLogManager(ILogManager *log)
{
    if(log) {
        int idx = m_logManagers.indexOf(log);
        if(idx != -1) {
            delete log;
            m_logManagers.removeAt(idx);
        }
    }
}

void IEngine::run()
{
    qDebug() << "run button pressed";
    if(m_engineStatus == EngineStatus::RUNNING) {
        qDebug() << "Engine is busy";
        if(m_iterationProgressInterface) {
            m_iterationProgressInterface->reportFinished();
        }
        if(m_budgetProgressInterface) {
            m_budgetProgressInterface->reportFinished();
        }
        return;
    }

    m_engineStatus = EngineStatus::RUNNING;

    engine_loop();

    if(m_iterationProgressInterface) {
        m_iterationProgressInterface->reportFinished();
    }
    if(m_budgetProgressInterface) {
        m_budgetProgressInterface->reportFinished();
    }
}

void IEngine::handleIncomingData(const QMap<QString, QVector<QVector<qreal> > >& dataMap,
                                 const QMap<QString, QStringList>& IDMap,
                                 const QMap<QString, QVector<int>>& runMap)
{
    Q_UNUSED(dataMap);
    Q_UNUSED(IDMap);
    Q_UNUSED(runMap);
}

bool IEngine::isRunning()
{
    return (m_engineStatus == EngineStatus::RUNNING);
}

bool IEngine::isPaused()
{
    return (m_engineStatus == EngineStatus::PAUSED);
}

} // namespace Designer
