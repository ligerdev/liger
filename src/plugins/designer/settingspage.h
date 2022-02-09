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
#ifndef DESIGNER_SETTINGSPAGE_H
#define DESIGNER_SETTINGSPAGE_H

#include <coreplugin/dialogs/ioptionspage.h>

#include <QPointer>

//QT_BEGIN_NAMESPACE
//class QDesignerOptionsPageInterface;
//QT_END_NAMESPACE

namespace Designer {
namespace Internal {

class SettingsPageWidget;

class SettingsPage : public Core::IOptionsPage
{
    Q_OBJECT

public:
    explicit SettingsPage();//QDesignerOptionsPageInterface *designerPage);

    QWidget *widget();
    void apply();
    void finish();

private:
//    QDesignerOptionsPageInterface *m_designerPage;
    bool m_initialized;
    QPointer<QWidget> m_widget;
};

class SettingsPageProvider : public Core::IOptionsPageProvider
{
    Q_OBJECT

public:
    SettingsPageProvider(QObject *parent = 0);

    QList<Core::IOptionsPage *> pages() const;
    bool matches(const QString &searchKeyWord) const;

private:
    mutable bool m_initialized;
    mutable QStringList m_keywords;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNER_SETTINGSPAGE_H
