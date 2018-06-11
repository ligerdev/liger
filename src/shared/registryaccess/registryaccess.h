/****************************************************************************
**
** Copyright (C) 2012-2016 The University of Sheffield (www.sheffield.ac.uk)
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
** This is a modified version of registryaccess.h from Qt Creator version 2.6.81
** Filename: registryaccess.h 
** Path:     QTC_ROOT/src/shared/registryaccess 
** Date:     11/11/2012
**
****************************************************************************/

// Some functions used by liger.exe and lgrdebugger.exe to check if
// lgrdebugger is currently registered for post-mortem debugging.
// This is only needed on Windows.

#ifndef REGISTRYACCESS_H
#define REGISTRYACCESS_H

#include <QString>
#include <QLatin1String>

#include <windows.h>

namespace RegistryAccess {

static const char *debuggerApplicationFileC = "lgrdebugger";
static const WCHAR *debuggerRegistryKeyC = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\AeDebug";
static const WCHAR *debuggerRegistryValueNameC = L"Debugger";

static inline QString wCharToQString(const WCHAR *w)
{
    return QString::fromUtf16(reinterpret_cast<const ushort *>(w));
}

QString msgFunctionFailed(const char *f, unsigned long error);

static inline QString msgRegistryOperationFailed(const char *op, const WCHAR *valueName, const QString &why)
{
    QString rc = QLatin1String("Registry ");
    rc += QLatin1String(op);
    rc += QLatin1String(" of ");
    rc += wCharToQString(valueName);
    rc += QLatin1String(" failed: ");
    rc += why;
    return rc;
}

bool registryReadStringKey(HKEY handle, // HKEY_LOCAL_MACHINE, etc.
                           const WCHAR *valueName,
                           QString *s,
                           QString *errorMessage);

bool openRegistryKey(HKEY category, // HKEY_LOCAL_MACHINE, etc.
                     const WCHAR *key,
                     bool readWrite,
                     HKEY *keyHandle,
                     QString *errorMessage);

QString debuggerCall(const QString &additionalOption = QString());

bool isRegistered(HKEY handle, const QString &call, QString *errorMessage, QString *oldDebugger = 0);

} // namespace RegistryAccess

#endif // REGISTRYACCESS_H
