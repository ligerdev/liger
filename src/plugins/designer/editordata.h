/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef EDITORDATA_H
#define EDITORDATA_H

namespace SharedTools { class WidgetHost; }

namespace Designer {
    class DesignWindowEditor;

namespace Internal {

// Associates the XML editor implementing the IEditor and its form widget host
struct EditorData {
    EditorData() : m_xmlEditor(0), widgetHost(0) {}
    operator bool() const { return m_xmlEditor != 0; }

    // m_xmlEditor is the IEditor showing the XML contents - not Editable at the moment.
    DesignWindowEditor *m_xmlEditor;
    // widgetHost is a QScrollArea that hosts the Designer window.
    SharedTools::WidgetHost *widgetHost;
};

} // namespace Internal
} // namespace Designer

#endif // EDITORDATA_H
