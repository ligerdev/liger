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
#ifndef DESIGNERCONSTANTS_H
#define DESIGNERCONSTANTS_H

#include <QtGlobal>
#include <QCursor>

namespace Designer {

enum ProcessType {
    MASTER_START_NODE,
    MASTER_END_NODE,
    SOURCE_NODE,
    SINK_NODE,
    ABSTRACT_NODE,
    ALGORITHM_NODE,
    OPERATOR_NODE,
    PROBLEM_NODE,
    UN_KNOWN
};

enum ProcessState {
    DETACHED_NODE,
    NODE_IN_QUEUE,
    NODE_READY,
    NODE_RUNNING,
    NODE_DONE,
    NODE_ERROR
};

enum PortType {
    INPUT_PORT,
    OUTPUT_PORT,
    INPUT_MOEO_PORT,
    OUTPUT_MOEO_PORT,
    ABSTRACT_PORT
};

enum PortState {
    DATA_RECEIVED,
    AWAITING_DATA,
    DATA_SENT,
    DETACHED_PORT,
    NO_STATE
};

// Run modes
enum RunMode {
    NoRunMode,
    NormalRunMode
};

// Engine status
enum EngineStatus {
    RUNNING,
    PAUSED,
    IDLE
};


namespace Constants {

// UNUSED - TO MARK AS DEPRECATED ?
const char DESIGNER_FILE_EXTENSION[] = "lgr";

const char INFO_READ_ONLY[] = "DesignerXmlEditor.ReadOnly";
const char K_DESIGNER_XML_EDITOR_ID[] = "DesignEditor.DesignerXmlEditor";
const char C_DESIGNER_XML_EDITOR[] = "Liger Xml Editor";
const char C_DESIGNER[] = "Liger Workflow Designer";
const char C_DESIGNER_XML_DISPLAY_NAME[]  = QT_TRANSLATE_NOOP("Designer", "Optimization Workflow Designer");

const char SETTINGS_CATEGORY[]          = "P.Designer";
const char SETTINGS_CATEGORY_ICON[]     = ":/core/images/category_design.png";
const char SETTINGS_TR_CATEGORY[]       = QT_TRANSLATE_NOOP("Designer", "Designer");
const char SETTINGS_CPP_SETTINGS_ID[]   = "Class Generation";
const char SETTINGS_CPP_SETTINGS_NAME[] = QT_TRANSLATE_NOOP("Designer", "Class Generation");

// Context
const char C_FORMEDITOR[] = "DesignEditor.DesignEditor";
const char M_FORMEDITOR[] = "DesignEditor.Menu";
const char M_FORMEDITOR_PREVIEW[] = "DesignEditor.Menu.Preview";

// Wizard type
const char DESIGNER_FILE_TYPE[]  = "LigerFiles";
const char DESIGNER_MIMETYPE[]   = "application/liger";
const char WIZARD_CATEGORY[]     = "R.Liger";
const char WIZARD_TR_CATEGORY[]  = QT_TRANSLATE_NOOP("Designer", "Liger");
const char C_DESIGNER_MIMETYPE_TEXT[] = "application/lgr";

// Menus
const char M_BUILDPROJECT[]  = "DesignEditor.Menu.Build";
const char M_VIEWPROJECT[]   = "DesignEditor.Menu.View";

// Menu groups
const char G_RUN_STOP[]      = "DesignEditor.Group.Stop";
const char G_RUN_RUN[]       = "DesignEditor.Group.Run";
const char G_RUN_PAUSE[]     = "DesignEditor.Group.Pause";
const char G_VIEW_ZOOM[]     = "DesignEditor.Group.Zoom";
const char G_VIEW_CENTRE[]   = "DesignEditor.Group.Centre";
const char G_VIEW_FIT[]      = "DesignEditor.Group.Fit";

// Context menu groups
const char RUNMENUCONTEXTMENU[]   = "Project.RunMenu";

const int NODE_DEFAULT_SIZE = 41;
const int PORT_DEFAULT_SIZE = 10;

// Tasks
const char TASK_OPEN_FILE[] = "DesignEditor.Task.OpenFile";
const char TASK_ENGINE_RUN[] = "DesignEditor.Task.EngineRun";

enum DesignerSubWindows
{
    DesignerToolBarWindow,
    //DesignerSideToolBarWindow,
    DesignerSubWindowCount
};

namespace Internal {
enum { debug = 0 };
}

// Default cursor for ports
const QCursor DEFAULT_PORT_CURSOR = Qt::PointingHandCursor;

// Actions
const char RUN[]                  = "DesignEditor.Run";
const char STOP[]                 = "DesignEditor.Stop";
const char PAUSE[]                 = "DesignEditor.Pause";
const char ZOOMIN[]               = "DesignEditor.ZoomIn";
const char ZOOMOUT[]              = "DesignEditor.ZoomOut";
const char RESETZOOM[]            = "DesignEditor.ResetZoom";
const char FITTOWINDOW[]          = "DesignEditor.FitToWindow";

// Action priorities
const int  P_ACTION_RUN           = 100;
const int  P_ACTION_PAUSE         = 99;
const int  P_ACTION_STOP          = 98;

// Icons
const char ICON_RUN[]             = ":/designer/images/run@2x.png";
const char ICON_RUN_SMALL[]       = ":/designer/images/run_small.png";
const char ICON_STOP[]            = ":/designer/images/stop.png";
const char ICON_STOP_SMALL[]      = ":/designer/images/stop_small.png";
const char ICON_PAUSE[]           = ":/designer/images/pause.png";
const char ICON_PAUSE_SMALL[]     = ":/designer/images/pause_small.png";
const char ICON_COPY[]            = ":/designer/images/copy.png";
const char ICON_CUT[]             = ":/designer/images/cut.png";

const char ICON_PASTE[] = ":/designer/images/paste.png";
const char ICON_DELETE[] = ":/designer/images/delete.png";

} // Constants
} // Designer

#endif //DESIGNERCONSTANTS_H
