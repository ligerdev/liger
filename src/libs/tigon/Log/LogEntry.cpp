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
#include <tigon/Log/LogEntry.h>
#include <ctime>

using namespace Json;

namespace Tigon {
namespace Log {

LogEntry::LogEntry()
{
    std::time_t result = std::time(NULL);
    record(LTime, JsonValue(std::asctime(std::localtime(&result))));
}

LogEntry::LogEntry(const LogEntry &other)
{
    m_data = other.m_data;
}

LogEntry::~LogEntry()
{

}

LogEntry* LogEntry::clone() const
{
    return new LogEntry(*this);
}

JsonObject LogEntry::data() const
{
    return m_data;
}

void LogEntry::record(const TString &key, const JsonValue &value)
{
    m_data[key] = value;
}

} // namespace Log
} // namespace Tigon
