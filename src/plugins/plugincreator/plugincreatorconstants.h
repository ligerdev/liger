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
#ifndef PLUGINCREATORCONSTRANTS_H
#define PLUGINCREATORCONSTRANTS_H

namespace PluginCreator {
namespace Constants {

const char * const ACTION_ID = "PluginCreator.Action";
const char * const MENU_ID   = "PluginCreator.Menu";

// Wizard type
const char DESIGNER_FILE_TYPE[]       = "LigerFiles";
const char DESIGNER_MIMETYPE[]        = "application/liger";
const char WIZARD_CATEGORY[]          = "T.Template";
const char WIZARD_TR_CATEGORY[]       = QT_TRANSLATE_NOOP("Designer", "Liger Templates");
const char C_DESIGNER_MIMETYPE_TEXT[] = "application/lgr";

const char WIZARD_ID[]                = "D.CustomLigerPlugin";
const char WIZARD_DISPLAY_NAME[]      = "Custom Liger Plugin Template";
const char WIZARD_DIAG_TITLE[]        = "Custom Plugin Information";
const char WIZARD_DESCRIPTIONS[]      = "Creates a Liger plugin template.";

// Internal constants
const char TEMPLATES_PATH[]           = "src/plugins/plugincreator/template";
const char PLUGIN_PRO_PATH[]          = "src/plugins/plugins.pro";
} // namespace Constants
} // namespace PluginCreator


#endif // PLUGINCREATORCONSTRANTS_H
