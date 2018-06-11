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
#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <json/json.h>

namespace Tigon {
namespace Log {

class LIGER_TIGON_EXPORT LogEntry
{
public:
    LogEntry();
    LogEntry(const LogEntry &other);
    virtual ~LogEntry();

    LogEntry* clone() const;
    Json::JsonObject data() const;

protected:
    void record(const TString &key, const Json::JsonValue &value);

private:
    Json::JsonObject  m_data;
};

} // namespace Log
} // namespace Tigon

#endif // LOGENTRY_H
