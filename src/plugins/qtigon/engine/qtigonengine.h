/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef QTIGONENGINE_H
#define QTIGONENGINE_H
#include <qtigon/qtigon_global.h>
#include <qtigon/qtigonconstants.h>
#include <designer/iengine/iengine.h>

#include <json/json.h>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>

namespace Tigon {
class TigonEngine;
class OptimizationLinearFlow;
}

namespace Designer {
class ILogManager;
}

namespace QTigon {

class QTIGON_EXPORT QTigonEngine : public Designer::IEngine
{
    Q_OBJECT

public:
    QTigonEngine();
    ~QTigonEngine();

    bool removeAllNodes();
    bool loadDesign(QList<Designer::IProcessNode *> nodes, QString ligerFile);
    void engine_loop();

    //void unloadLogManagers();

private slots:
    void errorHandling(const QString& err);

private:
    void logPopulations(Tigon::OptimizationLinearFlow* flow);
    void logNetworkPopulation(Tigon::OptimizationLinearFlow* flow);
    QJsonObject toQJsonObject(const Json::JsonObject &obj) const;
    QJsonArray toQJsonArray(const Json::JsonArray &array) const;
    // Disable copying of an engine.
    QTigonEngine(const QTigonEngine &engine);
    QTigonEngine& operator == (const QTigonEngine& rhs);

    void resetLogs();

    Tigon::TigonEngine* m_engine;
    Designer::ILogManager* m_populationsLog;
    Designer::ILogManager* m_evaluationsLog;

    bool m_isLogPopulations;
    bool m_isLogEvaluations;
    bool m_isLogNetworkPopulation;
    bool m_isLogProb;
    bool m_isLogProgress;
    bool m_isKeepPopulationsHistory;
};

} // namespace QTigon

#endif // QTIGONENGINE_H
