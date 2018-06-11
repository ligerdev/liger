/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef CORECONSTANTS_H
#define CORECONSTANTS_H

#include <QtGlobal>

namespace Core {
namespace Constants {

// Modes
const char MODE_WELCOME[]          = "Welcome";
const char MODE_WELCOME_TYPE[]     = "Type.Welcome";
const char MODE_EDIT[]             = "Edit";
const char MODE_EDIT_TYPE[]        = "Type.Edit";
const char MODE_DESIGN[]           = "Design";
const char MODE_DESIGN_TYPE[]      = "Type.Design";
// Order that side bar buttons are displayed. Largest number first etc.
const int  P_MODE_WELCOME          = 100;
const int  P_MODE_DESIGN           = 90;
const int  P_MODE_EDIT             = 89;

// Menubar
const char MENU_BAR[]              = "Liger.MenuBar";

// Side Tab

// Menus
const char M_FILE[]                = "Liger.Menu.File";
const char M_FILE_RECENTFILES[]    = "Liger.Menu.File.RecentFiles";
const char M_EDIT[]                = "Liger.Menu.Edit";
const char M_EDIT_ADVANCED[]       = "Liger.Menu.Edit.Advanced";
const char M_TOOLS[]               = "Liger.Menu.Tools";
const char M_TOOLS_EXTERNAL[]      = "Liger.Menu.Tools.External";
const char M_WINDOW[]              = "Liger.Menu.Window";
const char M_WINDOW_PANES[]        = "Liger.Menu.Window.Panes";
const char M_WINDOW_VIEWS[]        = "Liger.Menu.Window.Views";
const char M_HELP[]                = "Liger.Menu.Help";

// Contexts
const char C_GLOBAL[]              = "Global Context";
const char C_WELCOME_MODE[]        = "Core.WelcomeMode";
const char C_EDIT_MODE[]           = "Core.EditMode";
const char C_DESIGN_MODE[]         = "Core.DesignMode";
const char C_EDITORMANAGER[]       = "Core.EditorManager";
const char C_NAVIGATION_PANE[]     = "Core.NavigationPane";
const char C_PROBLEM_PANE[]        = "Core.ProblemPane";
const char C_GENERAL_OUTPUT_PANE[] = "Core.GeneralOutputPane";

// Default editor kind
const char K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME[] = QT_TRANSLATE_NOOP("OpenWith::Editors", "Plain Text Editor");
const char K_DEFAULT_TEXT_EDITOR_ID[] = "Core.PlainTextEditor";
const char K_DEFAULT_BINARY_EDITOR_ID[] = "Core.BinaryEditor";
const char K_DEFAULT_DESIGNER_ID[] = "Core.Designer";

// Default designer kind

//actions
const char UNDO[]                  = "Liger.Undo";
const char REDO[]                  = "Liger.Redo";
const char COPY[]                  = "Liger.Copy";
const char PASTE[]                 = "Liger.Paste";
const char CUT[]                   = "Liger.Cut";
const char SELECTALL[]             = "Liger.SelectAll";

const char GOTO[]                  = "Liger.Goto";

const char NEW[]                   = "Liger.New";
const char OPEN[]                  = "Liger.Open";
const char OPEN_WITH[]             = "Liger.OpenWith";
const char REVERTTOSAVED[]         = "Liger.RevertToSaved";
const char SAVE[]                  = "Liger.Save";
const char SAVEAS[]                = "Liger.SaveAs";
const char SAVEALL[]               = "Liger.SaveAll";
const char PRINT[]                 = "Liger.Print";
const char EXIT[]                  = "Liger.Exit";

const char OPTIONS[]               = "Liger.Options";
const char TOGGLE_SIDEBAR[]        = "Liger.ToggleSidebar";
const char TOGGLE_MODE_SELECTOR[]  = "Liger.ToggleModeSelector";
const char TOGGLE_FULLSCREEN[]     = "Liger.ToggleFullScreen";

const char MINIMIZE_WINDOW[]       = "Liger.MinimizeWindow";
const char ZOOM_WINDOW[]           = "Liger.ZoomWindow";

const char SPLIT[]                 = "Liger.Split";
const char SPLIT_SIDE_BY_SIDE[]    = "Liger.SplitSideBySide";
const char SPLIT_NEW_WINDOW[]      = "Liger.SplitNewWindow";
const char REMOVE_CURRENT_SPLIT[]  = "Liger.RemoveCurrentSplit";
const char REMOVE_ALL_SPLITS[]     = "Liger.RemoveAllSplits";
const char GOTO_NEXT_SPLIT[]      = "Liger.GotoOtherSplit";
const char CLOSE[]                 = "Liger.Close";

const char CLOSE_ALTERNATIVE[]     = "Liger.Close_Alternative"; // temporary, see QTCREATORBUG-72

const char CLOSEALL[]              = "Liger.CloseAll";
const char CLOSEOTHERS[]           = "Liger.CloseOthers";
const char CLOSEALLEXCEPTVISIBLE[] = "Liger.CloseAllExceptVisible";
const char GOTONEXT[]              = "Liger.GotoNext";
const char GOTOPREV[]              = "Liger.GotoPrevious";
const char GOTONEXTINHISTORY[]     = "Liger.GotoNextInHistory";
const char GOTOPREVINHISTORY[]     = "Liger.GotoPreviousInHistory";
const char GO_BACK[]               = "Liger.GoBack";
const char GO_FORWARD[]            = "Liger.GoForward";
const char ABOUT_LIGER[]           = "Liger.AboutLiger";
const char ABOUT_PLUGINS[]         = "Liger.AboutPlugins";
const char S_RETURNTOEDITOR[]      = "Liger.ReturnToEditor";

// Default groups
const char G_DEFAULT_ONE[]         = "Liger.Group.Default.One";
const char G_DEFAULT_TWO[]         = "Liger.Group.Default.Two";
const char G_DEFAULT_THREE[]       = "Liger.Group.Default.Three";

// Main menu bar groups
const char G_FILE[]                = "Liger.Group.File";
const char G_EDIT[]                = "Liger.Group.Edit";
const char G_VIEW[]                = "Liger.Group.View";
const char G_TOOLS[]               = "Liger.Group.Tools";
const char G_WINDOW[]              = "Liger.Group.Window";
const char G_HELP[]                = "Liger.Group.Help";

// File menu groups
const char G_FILE_NEW[]            = "Liger.Group.File.New";
const char G_FILE_OPEN[]           = "Liger.Group.File.Open";
const char G_FILE_PROJECT[]        = "Liger.Group.File.Project";
const char G_FILE_SAVE[]           = "Liger.Group.File.Save";
const char G_FILE_CLOSE[]          = "Liger.Group.File.Close";
const char G_FILE_PRINT[]          = "Liger.Group.File.Print";
const char G_FILE_OTHER[]          = "Liger.Group.File.Other";

// Edit menu groups
const char G_EDIT_UNDOREDO[]       = "Liger.Group.Edit.UndoRedo";
const char G_EDIT_COPYPASTE[]      = "Liger.Group.Edit.CopyPaste";
const char G_EDIT_SELECTALL[]      = "Liger.Group.Edit.SelectAll";
const char G_EDIT_ADVANCED[]       = "Liger.Group.Edit.Advanced";

const char G_EDIT_FIND[]           = "Liger.Group.Edit.Find";
const char G_EDIT_OTHER[]          = "Liger.Group.Edit.Other";

// Advanced edit menu groups
const char G_EDIT_FORMAT[]         = "Liger.Group.Edit.Format";
const char G_EDIT_COLLAPSING[]     = "Liger.Group.Edit.Collapsing";
const char G_EDIT_TEXT[]           = "Liger.Group.Edit.Text";
const char G_EDIT_BLOCKS[]         = "Liger.Group.Edit.Blocks";
const char G_EDIT_FONT[]           = "Liger.Group.Edit.Font";
const char G_EDIT_EDITOR[]         = "Liger.Group.Edit.Editor";

const char G_TOOLS_OPTIONS[]       = "Liger.Group.Tools.Options";

// Window menu groups
const char G_WINDOW_SIZE[]         = "Liger.Group.Window.Size";
const char G_WINDOW_PANES[]        = "Liger.Group.Window.Panes";
const char G_WINDOW_VIEWS[]        = "Liger.Group.Window.Views";
const char G_WINDOW_SPLIT[]        = "Liger.Group.Window.Split";
const char G_WINDOW_NAVIGATE[]     = "Liger.Group.Window.Navigate";
const char G_WINDOW_OTHER[]        = "Liger.Group.Window.Other";

// Help groups (global)
const char G_HELP_HELP[]           = "Liger.Group.Help.Help";
const char G_HELP_SUPPORT[]        = "Liger.Group.Help.Supprt";
const char G_HELP_ABOUT[]          = "Liger.Group.Help.About";

const char ICON_MINUS[]              = ":/core/images/minus.png";
const char ICON_PLUS[]               = ":/core/images/plus.png";
const char ICON_NEWFILE[]            = ":/core/images/filenew.png";
const char ICON_OPENFILE[]           = ":/core/images/fileopen.png";
const char ICON_SAVEFILE[]           = ":/core/images/filesave.png";
const char ICON_UNDO[]               = ":/core/images/undo.png";
const char ICON_REDO[]               = ":/core/images/redo.png";
const char ICON_COPY[]               = ":/core/images/editcopy.png";
const char ICON_PASTE[]              = ":/core/images/editpaste.png";
const char ICON_CUT[]                = ":/core/images/editcut.png";
const char ICON_NEXT[]               = ":/core/images/next.png";
const char ICON_PREV[]               = ":/core/images/prev.png";
const char ICON_DIR[]                = ":/core/images/dir.png";
const char ICON_CLEAN_PANE[]         = ":/core/images/clean_pane_small.png";
const char ICON_CLEAR[]              = ":/core/images/clear.png";
const char ICON_RESET[]              = ":/core/images/reset.png";
const char ICON_MAGNIFIER[]          = ":/core/images/magnifier.png";
const char ICON_TOGGLE_SIDEBAR[]     = ":/core/images/sidebaricon.png";
const char ICON_TOGGLE_TOPBAR[]      = ":/core/images/topbaricon.png";
const char ICON_CLOSE_DOCUMENT[]     = ":/core/images/button_close.png";
const char ICON_CLOSE[]              = ":/core/images/closebutton.png";
const char ICON_CLOSE_DARK[]         = ":/core/images/darkclosebutton.png";
const char ICON_SPLIT_HORIZONTAL[]   = ":/core/images/splitbutton_horizontal.png";
const char ICON_SPLIT_VERTICAL[]     = ":/core/images/splitbutton_vertical.png";
const char ICON_CLOSE_SPLIT_TOP[]    = ":/core/images/splitbutton_closetop.png";
const char ICON_CLOSE_SPLIT_BOTTOM[] = ":/core/images/splitbutton_closebottom.png";
const char ICON_CLOSE_SPLIT_LEFT[]   = ":/core/images/splitbutton_closeleft.png";
const char ICON_CLOSE_SPLIT_RIGHT[]  = ":/core/images/splitbutton_closeright.png";
const char ICON_SIDE_BY_SIDE_DIFF[]  = ":/core/images/sidebysidediff.png";
const char ICON_TEXT_DIFF[]          = ":/core/images/textdiff.png";
const char ICON_FILTER[]             = ":/core/images/filtericon.png";
const char ICON_LINK[]               = ":/core/images/linkicon.png";
const char ICON_PAUSE[]              = ":/core/images/pause.png";
const char ICON_LIGERLOGO_16[]       = ":/core/images/logo/16/Liger_logo.png";
const char ICON_LIGERLOGO_24[]       = ":/core/images/logo/24/Liger_logo.png";
const char ICON_LIGERLOGO_32[]       = ":/core/images/logo/32/Liger_logo.png";
const char ICON_LIGERLOGO_48[]       = ":/core/images/logo/48/Liger_logo.png";
const char ICON_LIGERLOGO_64[]       = ":/core/images/logo/64/Liger_logo.png";
const char ICON_LIGERLOGO_128[]      = ":/core/images/logo/128/Liger_logo.png";
const char ICON_LIGERLOGO_256[]      = ":/core/images/logo/256/Liger_logo.png";
const char ICON_LIGERLOGO_512[]      = ":/core/images/logo/512/Liger_logo.png";
const char ICON_MODE_EDIT_SVG[]      = ":/core/images/mode_Edit.png";

const char WIZARD_CATEGORY_QT[] = "R.Qt";
const char WIZARD_TR_CATEGORY_QT[] = QT_TRANSLATE_NOOP("Core", "Qt");

const char SETTINGS_CATEGORY_CORE[] = "A.Core";
const char SETTINGS_CATEGORY_CORE_ICON[] = ":/core/images/category_core.png";
const char SETTINGS_TR_CATEGORY_CORE[] = QT_TRANSLATE_NOOP("Core", "Environment");
const char SETTINGS_ID_ENVIRONMENT[] = "A.General";
const char SETTINGS_ID_SHORTCUTS[] = "B.Keyboard";
const char SETTINGS_ID_TOOLS[] = "C.ExternalTools";
const char SETTINGS_ID_MIMETYPES[] = "D.MimeTypes";

const char SETTINGS_DEFAULTTEXTENCODING[] = "General/DefaultFileEncoding";

const char ALL_FILES_FILTER[]      = QT_TRANSLATE_NOOP("Core", "All Files (*)");


const char TR_CLEAR_MENU[]         = QT_TRANSLATE_NOOP("Core", "Clear Menu");

const char DEFAULT_BUILD_DIRECTORY[] = "../build-%{CurrentProject:Name}-%{CurrentKit:FileSystemName}-%{CurrentBuild:Name}";

const int TARGET_ICON_SIZE = 32;

} // namespace Constants
} // namespace Core

#endif // CORECONSTANTS_H
