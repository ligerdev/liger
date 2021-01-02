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
#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <json/json.h>

using namespace Tigon::Representation;

namespace Tigon {
namespace Representation {
class ISet;
}
namespace Log {
class LogEntry;

/// \todo completely rewrite the log related classes.
/// @attention remove the use of Json::Json. Consider:
/// 1. boost property tree
/// 2. hdf5 data format
/// @badcode
class LIGER_TIGON_EXPORT LogManager
{
public:
    LogManager();
    LogManager(const LogManager &other);
    ~LogManager();

    LogManagerSPtr clone() const;

    void clearLog();
    void clearEvaluationsLog();
    void clearPopulationsLog();

    /// Logging methods
    void logEvaluation(IFunctionSPtr func,
                       const TVector<IElementSPtr> &inputs,
                       const TVector<IElementSPtr> &outputs,
                       const Json::JsonObject &additinalFields = Json::JsonObject());
    void logPopulation(ISet* solSet);
    void logPopulation(ISet* solSet, const TString &name,
                       const Json::JsonObject &additinalFields = Json::JsonObject());

    /// Enable log flags
    ///\brief enable or disable all log entries
    void defineLogAll(bool         logAll);
    void defineLogEvaluation(bool keepLog);
    void defineLogPopulation(bool keepLog);
    bool isLogEvaluation()           const;
    bool isLogPopulation()           const;

    /// Output
    Json::JsonObject all();
    Json::JsonObject evaluationsLog();
    Json::JsonObject populationsLog();

private:
    TVector<LogEntry*> m_evalEntries;
    TVector<LogEntry*> m_popEntries;
    bool m_isLogEvaluatoin;
    bool m_isLogPopulation;
};

} // namespace Log
} // namespace Tigon

#endif // LOGMANAGER_H
