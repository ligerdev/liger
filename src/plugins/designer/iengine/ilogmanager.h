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
#ifndef ILOGMANAGER_H
#define ILOGMANAGER_H
#include <designer/designer_global.h>

#include <QObject>
#include <QFile>
#include <QSharedPointer>

namespace Designer {

class DESIGNER_EXPORT ILogManager : public QObject
{
    Q_OBJECT

public:
    explicit ILogManager(QObject *parent = 0, const QString& path = QString());
    virtual ~ILogManager();
    QString logPath() const;
    void setLogPath(const QString& path);

    QSharedPointer<QFile> file() const;

    bool logPopulations() const;
    void setLogPopulations(bool logPopulations);

    bool logEvaluations() const;
    void setLogEvaluations(bool logEvaluations);

    bool logNetworkPopulation() const;
    void setLogNetworkPopulation(bool logNetPopulation);

    bool logProb() const;
    void setLogProb(bool logProb);

    bool logProgress() const;
    void setLogProgress(bool logProgress);

    bool keepPopulationsHistory() const;
    void setKeepPopulationsHistory(bool keepPopulationsHistory);

protected:
    virtual void writeLog(const QJsonObject& logObj);
    virtual void close();

signals:
    void error(const QString& errMsg);
    void logUpdated();

public slots:
    void writeLogSlot(const QJsonObject& logObj);

private:
    ILogManager(const ILogManager& manager);
    ILogManager& operator=(const ILogManager& rhs);
    QSharedPointer<QFile>   m_file;
    bool m_start;
    bool m_logPopulations;
    bool m_logEvaluations;
    bool m_logNetworkPopulation;
    bool m_logProb;
    bool m_logProgress;
    bool m_keepPopulationsHistory;
};

}

#endif // ILOGMANAGER_H
