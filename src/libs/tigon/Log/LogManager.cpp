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
#include <tigon/Log/LogManager.h>
#include <tigon/Log/LogEntry.h>
#include <tigon/Log/EvaluationEntry.h>
#include <tigon/Log/PopulationEntry.h>

using namespace Json;

namespace Tigon {
namespace Log {

LogManager::LogManager()
    : m_isLogEvaluatoin(false),
      m_isLogPopulation(false)
{

}

LogManager::LogManager(const LogManager &other)
    : m_evalEntries(other.m_evalEntries.size()),
      m_popEntries(other.m_popEntries.size())
{
    for(int i = 0; i < m_evalEntries.size(); i++) {
        m_evalEntries[i] = other.m_evalEntries[i]->clone();
    }
    for(int i = 0; i < m_popEntries.size(); i++) {
        m_popEntries[i] = other.m_popEntries[i]->clone();
    }
    m_isLogEvaluatoin = other.m_isLogEvaluatoin;
    m_isLogPopulation = other.m_isLogPopulation;
}

LogManager::~LogManager()
{
    clearLog();
}

LogManagerSPtr LogManager::clone() const
{
    return LogManagerSPtr(new LogManager(*this));
}

void LogManager::clearLog()
{
    clearEvaluationsLog();
    clearPopulationsLog();
}

void LogManager::clearEvaluationsLog()
{
    for(int i = 0; i < m_evalEntries.size(); i++) {
        delete m_evalEntries[i];
    }
    m_evalEntries.clear();
}

void LogManager::clearPopulationsLog()
{
    for(int i = 0; i < m_popEntries.size(); i++) {
        delete m_popEntries[i];
    }
    m_popEntries.clear();
}

void LogManager::logEvaluation(IFunctionSPtr func,
                               const TVector<IElementSPtr> &inputs,
                               const TVector<IElementSPtr> &outputs,
                               const JsonObject &additinalFields)
{
    m_evalEntries.push_back(new EvaluationEntry(func, inputs, outputs, additinalFields));
}

void LogManager::logPopulation(ISet *solSet)
{
    m_popEntries.push_back(new PopulationEntry(solSet));
}

void LogManager::logPopulation(ISet *solSet, const TString &name,
                               const JsonObject &additinalFields)
{
    m_popEntries.push_back(new PopulationEntry(solSet, name, additinalFields));
}

void LogManager::defineLogAll(bool logAll)
{
    defineLogEvaluation(logAll);
    defineLogPopulation(logAll);
    ///\todo enable/disable any implemented logging entry
}

void LogManager::defineLogEvaluation(bool keepLog)
{
    m_isLogEvaluatoin = keepLog;
}

void LogManager::defineLogPopulation(bool keepLog)
{
    m_isLogPopulation = keepLog;
}

bool LogManager::isLogEvaluation() const
{
    return m_isLogEvaluatoin;
}

bool LogManager::isLogPopulation() const
{
    return m_isLogPopulation;
}

JsonObject LogManager::all()
{
    JsonObject log;
    log[LEvals] = evaluationsLog()[LEvals];
    log[LPops]  = populationsLog()[LPops];
    return log;
}

JsonObject LogManager::evaluationsLog()
{
    JsonArray evalArray;
    for(int i = 0; i < m_evalEntries.size(); i++) {
        evalArray.push_back(m_evalEntries[i]->data());
    }
    JsonObject evalLog;
    evalLog[LEvals] = evalArray;
    return evalLog;
}

JsonObject LogManager::populationsLog()
{
    JsonArray popArray;
    for(int i = 0; i < m_popEntries.size(); i++) {
        popArray.push_back(m_popEntries[i]->data());
    }
    JsonObject popLog;
    popLog[LPops] = popArray;
    return popLog;
}

} // namespace Log
} // namespace Tigon
