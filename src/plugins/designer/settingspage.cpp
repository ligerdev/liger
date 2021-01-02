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
#include "settingspage.h"
#include "designeditorw.h"

#include <QCoreApplication>
#include <coreplugin/icore.h>

using namespace Designer::Internal;

SettingsPage::SettingsPage() :
   /* m_designerPage(designerPage),*/ m_initialized(false)
{
//    setId(Core::Id::fromString(m_designerPage->name()));
//    setDisplayName(m_designerPage->name());
//    setCategory(Designer::Constants::SETTINGS_CATEGORY);
//    setDisplayCategory(QCoreApplication::translate("Designer",
//        Designer::Constants::SETTINGS_TR_CATEGORY));
//    setCategoryIcon(QLatin1String(Designer::Constants::SETTINGS_CATEGORY_ICON));
}

QWidget *SettingsPage::widget()
{
    m_initialized = true;
//    if (!m_widget)
//        m_widget = m_designerPage->createPage(0);
    return m_widget;

}

void SettingsPage::apply()
{
//    if (m_initialized)
//        m_designerPage->apply();
}

void SettingsPage::finish()
{
//    if (m_initialized)
//        m_designerPage->finish();
    delete m_widget;
}

SettingsPageProvider::SettingsPageProvider(QObject *parent)
    : IOptionsPageProvider(parent), m_initialized(false)
{
    setCategory(Designer::Constants::SETTINGS_CATEGORY);
    setDisplayCategory(QCoreApplication::translate("Designer",
        Designer::Constants::SETTINGS_TR_CATEGORY));
    setCategoryIcon(QLatin1String(Designer::Constants::SETTINGS_CATEGORY_ICON));
}

QList<Core::IOptionsPage *> SettingsPageProvider::pages() const
{
    if (!m_initialized) {
        // get options pages from designer
        m_initialized = true;
        DesignEditorW::ensureInitStage(DesignEditorW::RegisterPlugins);
    }
    return DesignEditorW::instance()->optionsPages();
}

bool SettingsPageProvider::matches(const QString &searchKeyWord) const
{
    // to avoid fully initializing designer when typing something in the options' filter edit
    // we hardcode matching of UI text from the designer pages, which are taken if the designer pages
    // were not yet loaded
    // luckily linguist cannot resolve the translated texts, so we do not end up with duplicate
    // translatable strings for Qt Creator
    static const struct { const char *context; const char *value; } uitext[] = {
        {"EmbeddedOptionsPage", "Embedded Design"},
        {"EmbeddedOptionsPage", "Device Profiles"},
        {"FormEditorOptionsPage", "Forms"},
        {"FormEditorOptionsPage", "Preview Zoom"},
        {"FormEditorOptionsPage", "Default Zoom"},
        {"FormEditorOptionsPage", "Default Grid"},
        {"qdesigner_internal::GridPanel", "Visible"},
        {"qdesigner_internal::GridPanel", "Snap"},
        {"qdesigner_internal::GridPanel", "Reset"},
        {"qdesigner_internal::GridPanel", "Grid"},
        {"qdesigner_internal::GridPanel", "Grid &X"},
        {"qdesigner_internal::GridPanel", "Grid &Y"},
        {"PreviewConfigurationWidget", "Print/Preview Configuration"},
        {"PreviewConfigurationWidget", "Style"},
        {"PreviewConfigurationWidget", "Style sheet"},
        {"PreviewConfigurationWidget", "Device skin"},
        {"TemplateOptionsPage", "Template Paths"},
        {"qdesigner_internal::TemplateOptionsWidget", "Additional Template Paths"}
    };
    static const size_t itemCount = sizeof(uitext)/sizeof(uitext[0]);
    if (m_keywords.isEmpty()) {
        m_keywords.reserve(itemCount);
        for (size_t i = 0; i < itemCount; ++i)
            m_keywords << QCoreApplication::translate(uitext[i].context, uitext[i].value).remove(QLatin1Char('&'));
    }
    foreach (const QString &key, m_keywords) {
        if (key.contains(searchKeyWord, Qt::CaseInsensitive))
            return true;
    }
    return false;
}
