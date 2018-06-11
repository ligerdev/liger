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
#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QVariant>

namespace Designer {
namespace Internal {

/* Prepends "Designer" to every value stored/retrieved by designer plugins,
   to avoid namespace polution. We cannot use a group because groups cannot be nested,
   and designer uses groups internally. */
class SettingsManager //: public QDesignerSettingsInterface
{
public:
    virtual void beginGroup(const QString &prefix);
    virtual void endGroup();

    virtual bool contains(const QString &key) const;
    virtual void setValue(const QString &key, const QVariant &value);
    virtual QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    virtual void remove(const QString &key);

private:
    QString addPrefix(const QString &name) const;
};

} // namespace Internal
} // namespace Designer

#endif // SETTINGSMANAGER_H
