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
#include <designer/iengine/ilogmanager.h>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace Designer {

ILogManager::ILogManager(QObject *parent, const QString &path)
    : QObject(parent)
    , m_start(true)
    , m_logPopulations(false)
    , m_logEvaluations(false)
    , m_logNetworkPopulation(false)
    , m_logProb(false)
    , m_logProgress(false)
    , m_keepPopulationsHistory(false)
{
    setLogPath(path);
}

ILogManager::~ILogManager()
{
    close();
}

QString ILogManager::logPath() const
{
    if(m_file.isNull()) {
        return QString("");
    }
    return m_file->fileName();
}

void ILogManager::setLogPath(const QString &path)
{
    if(path.isEmpty()) {
        m_file = QSharedPointer<QFile>(new QFile(QDir::home().filePath("ligerlog.json")));
    } else {
        m_file = QSharedPointer<QFile>(new QFile(path));
    }
}

void ILogManager::writeLog(const QJsonObject &logObj)
{
    // Check if all fields are empty
    bool isEmpty = true;
    QStringList keys = logObj.keys();
    for(int i=0; i<logObj.size(); i++) {
        if(!(logObj[keys[i]].toArray().isEmpty())) {
            isEmpty = false;
            break;
        }
    }

    if(isEmpty) {
        return;
    }

    if(m_start && m_file->exists()) {
        if(!m_file->remove()) {
            emit error(QString("ILogManager: " + m_file->errorString()));
            return;
        }
        qDebug() << m_file->fileName() << "removed";
        m_start = false;
    }

    QJsonDocument jdoc(logObj);
    if(!logObj.isEmpty()) {
        if(m_file->open(QIODevice::WriteOnly)) {
            qDebug() << "Write to file " << m_file->fileName();
            //m_file->write(jdoc.toJson(QJsonDocument::Compact));
            m_file->write(jdoc.toJson());
            m_file->close();
        } else {
            qDebug() << m_file->fileName() << ": cannot not open.";
        }
    }
}

void ILogManager::close()
{
    if(!m_file.isNull() && m_file->isOpen()) {
        m_file->close();
    }
    m_start = true;
}

void ILogManager::writeLogSlot(const QJsonObject &logObj)
{
    writeLog(logObj);
}

bool ILogManager::logPopulations() const
{
    return m_logPopulations;
}

void ILogManager::setLogPopulations(bool logPopulations)
{
    m_logPopulations = logPopulations;
}

bool ILogManager::logEvaluations() const
{
    return m_logEvaluations;
}

void ILogManager::setLogEvaluations(bool logEvaluations)
{
    m_logEvaluations = logEvaluations;
}

bool ILogManager::logProgress() const
{
    return m_logProgress;
}

void ILogManager::setLogProgress(bool logProgress)
{
    m_logProgress = logProgress;
}

bool ILogManager::keepPopulationsHistory() const
{
    return m_keepPopulationsHistory;
}

void ILogManager::setKeepPopulationsHistory(bool keepPopulationsHistory)
{
    m_keepPopulationsHistory = keepPopulationsHistory;
}

QSharedPointer<QFile> ILogManager::file() const
{
    return m_file;
}

bool ILogManager::logProb() const
{
    return m_logProb;
}

void ILogManager::setLogProb(bool logProb)
{
    m_logProb = logProb;
}

bool ILogManager::logNetworkPopulation() const
{
    return m_logNetworkPopulation;
}

void ILogManager::setLogNetworkPopulation(bool logNetPopulation)
{
    m_logNetworkPopulation = logNetPopulation;
}

}
