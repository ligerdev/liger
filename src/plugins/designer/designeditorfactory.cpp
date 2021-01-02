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
#include "designeditorfactory.h"
#include "designeditorw.h"
#include "designwindoweditor.h"
#include "editordata.h"
#include "designerxmleditorwidget.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/infobar.h>
#include <coreplugin/fileiconprovider.h>
#include <coreplugin/modemanager.h>

#include <QCoreApplication>
#include <QDebug>

using namespace Designer::Constants;

namespace Designer {
namespace Internal {

DesignEditorFactory::DesignEditorFactory()
  : Core::IEditorFactory(Core::ICore::instance())
{
    setId(K_DESIGNER_XML_EDITOR_ID);
    setDisplayName(qApp->translate("Designer", C_DESIGNER_XML_DISPLAY_NAME));
    addMimeType(DESIGNER_MIMETYPE);

    // Liger-Bug: The icon does not load properly.
    Core::FileIconProvider::registerIconOverlayForSuffix(":/designer/images/qt_ui.png", "lgr");
}

Core::IEditor *DesignEditorFactory::createEditor()
{
    const EditorData data = DesignEditorW::instance()->createEditor();
    if (data.m_xmlEditor) {
        Core::InfoBarEntry info(Core::Id(Constants::INFO_READ_ONLY),
                                tr("This file can only be edited in <b>Design</b> mode."));
        info.setCustomButtonInfo(tr("Switch Mode"), this, SLOT(designerModeClicked()));
        data.m_xmlEditor->document()->infoBar()->addInfo(info);
    }
    return data.m_xmlEditor;
}

void DesignEditorFactory::designerModeClicked()
{
    Core::ModeManager::activateMode(Core::Constants::MODE_DESIGN);
}

} // namespace Internal
} // namespace Designer
