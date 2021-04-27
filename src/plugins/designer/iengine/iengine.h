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
#ifndef IENGINE_H
#define IENGINE_H
#include <designer/designer_global.h>
#include <designer/designerconstants.h>
#include <coreplugin/progressmanager/futureprogress.h>

#include <QtConcurrent>
#include <QMap>
#include <QString>
#include <QList>
#include <QObject>

namespace Designer {

class IPortLink;
class IProcessNode;
class ILogManager;

/*!
 * \brief The IEngine class. Interface for all engines.
 * \todo disable copy constructor of engine - do not allow copy
 */
class DESIGNER_EXPORT IEngine : public QObject
{
    Q_OBJECT

public:
    IEngine();
    virtual ~IEngine();

    virtual bool loadDesign(QList<IProcessNode *> nodes, QString ligerFile);
    /*!
     * \brief unloadDesign Reset the engine, removing all loaded operators and
     * log manager if any.
     * \return
     */
    virtual bool unloadDesign();
    virtual void unloadLogManagers();
    virtual bool addProcessNode(IProcessNode* node);
    virtual bool removeProcessNode(IProcessNode* node);
    virtual bool removeAllNodes();

    bool isRunning();
    bool isPaused();
    void run();
    virtual void engine_loop() = 0;
    /*!
     * \brief Overload this function to handle incoming data from other node.
     * Created for network version.
     */
    virtual void handleIncomingData(const QMap<QString, QVector<QVector<qreal> > > &dataMap,
                                    const QMap<QString, QStringList> &IDMap,
                                    const QMap<QString, QVector<int> > &runMap);

    QList<ILogManager *> logManagers() const;
    /*!
     * \brief Append a log manager to engine. If log is null pointer or has
     * already been added, do nothing.
     * \param logManager
     */
    void appendLogManager(ILogManager *log);
    /*!
     * \brief Remove the log manager if it has been included.
     * \param log
     */
    void removeLogManager(ILogManager* log);

public slots:
    void start();
    void pause();
    void resume();
    void exit();
    void finished();
    void receivedIncomingData(const QMap<QString, QVector<QVector<qreal> > > &dataMap,
                              const QMap<QString, QStringList> &IDMap,
                              const QMap<QString, QVector<int>>& runMap);

signals:
    /// Iteration and budget signals
    void signalIterationCounter(int iter);
    void signalBudgetCounter(int budget);

    /// Logging signals
    void signalPopulationsLog(const QJsonObject& jobj);
    void signalEvaluationsLog(const QJsonObject& jobj);
    void signalNetworkPopulationLog(const QJsonObject& jobj);
    void signalProblemDefinition(const QJsonObject& jobj);
    void signalProcess(const QJsonObject& jobj);

    /// Date update
    void dataUpdated();

    /// Engine status signals
    void engineStarted();
    void enginePaused();
    void engineResumed();
    void engineExit();
    void engineFinished();

    /// Error
    void engineError(const QString& err);

private slots:
    void receivedIterationCount(int iter);
    void receivedBudgetCount(int budget);

///\todo Move all protected data to be private
protected:
    void setMasterStartNodePresent();
    void setMasterEndNodePresent();

    IProcessNode*        m_masterStartNode;
    IProcessNode*        m_masterEndNode;
    QList<IPortLink*>    m_portLinks;
    QList<IProcessNode*> m_processNodes;
    QList<IProcessNode*> m_sourceNodes;
    QList<IProcessNode*> m_sinkNodes;

    QVector<qint64> m_pauseByIter;
    QVector<qint64> m_pauseByBudget;

    QFuture<void> m_result;
    QFutureInterface<void>* m_iterationProgressInterface;
    QFutureInterface<void>* m_budgetProgressInterface;
    QFutureWatcher<void>    m_iterationProgressWatcher;
    QFutureWatcher<void>    m_budgetProgressWatcher;
    Core::FutureProgress*   m_iterationFutureProgress;
    Core::FutureProgress*   m_budgetFutureProgress;
    volatile EngineStatus   m_engineStatus;
    int m_maxIteration;
    int m_budget;
    int m_currentIteration;
    int m_usedBudget;

private:
    void updateProcessNodeStatus(ProcessState state);
    void resetLogFlags();

    bool masterNodesPresent();

    QList<ILogManager*> m_logManagers;
    // Disable copying of an engine.
    IEngine(const IEngine &engine);
    IEngine& operator == (const IEngine& rhs);

    bool m_masterStartNodePresent;
    bool m_masterEndNodePresent;
};

} // namespace Designer

//Q_DECLARE_METATYPE(Designer::IEngine)

#endif // IENGINE_H
