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
#include "settingsmanager.h"

#include <coreplugin/icore.h>
#include <utils/qtcassert.h>

#include <QSettings>
#include <QDebug>

using namespace Designer::Internal;

void SettingsManager::beginGroup(const QString &prefix)
{
    Core::ICore::settings()->beginGroup(addPrefix(prefix));
}

void SettingsManager::endGroup()
{
    Core::ICore::settings()->endGroup();
}

bool SettingsManager::contains(const QString &key) const
{
    return Core::ICore::settings()->contains(addPrefix(key));
}

void SettingsManager::setValue(const QString &key, const QVariant &value)
{
    Core::ICore::settings()->setValue(addPrefix(key), value);
}

QVariant SettingsManager::value(const QString &key, const QVariant &defaultValue) const
{
    return Core::ICore::settings()->value(addPrefix(key), defaultValue);
}

void SettingsManager::remove(const QString &key)
{
    Core::ICore::settings()->remove(addPrefix(key));
}

QString SettingsManager::addPrefix(const QString &name) const
{
    QString result = name;
    if (Core::ICore::settings()->group().isEmpty())
        result.prepend(QLatin1String("Designer"));
    return result;
}
