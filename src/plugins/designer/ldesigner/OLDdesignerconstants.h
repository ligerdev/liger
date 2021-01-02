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
#ifndef DESIGNERCONSTANTS_H
#define DESIGNERCONSTANTS_H

#include <QtGlobal>

namespace Designer {

// Text color and style categories
enum TextStyle {
    C_TEXT,

    C_LINK,
    C_SELECTION,
    C_LINE_NUMBER,
    C_SEARCH_RESULT,
    C_SEARCH_SCOPE,
    C_PARENTHESES,
    C_CURRENT_LINE,
    C_CURRENT_LINE_NUMBER,
    C_OCCURRENCES,
    C_OCCURRENCES_UNUSED,
    C_OCCURRENCES_RENAME,

    C_NUMBER,
    C_STRING,
    C_TYPE,
    C_LOCAL,
    C_FIELD,
    C_ENUMERATION,
    C_VIRTUAL_METHOD,
    C_FUNCTION,
    C_KEYWORD,
    C_OPERATOR,
    C_PREPROCESSOR,
    C_LABEL,
    C_COMMENT,
    C_DOXYGEN_COMMENT,
    C_DOXYGEN_TAG,
    C_VISUAL_WHITESPACE,
    C_QML_LOCAL_ID,
    C_QML_EXTERNAL_ID,
    C_QML_TYPE_ID,
    C_QML_ROOT_OBJECT_PROPERTY,
    C_QML_SCOPE_OBJECT_PROPERTY,
    C_QML_EXTERNAL_OBJECT_PROPERTY,
    C_JS_SCOPE_VAR,
    C_JS_IMPORT_VAR,
    C_JS_GLOBAL_VAR,
    C_QML_STATE_NAME,
    C_BINDING,


    C_DISABLED_CODE,

    C_ADDED_LINE,
    C_REMOVED_LINE,
    C_DIFF_FILE,
    C_DIFF_LOCATION,

    C_LAST_STYLE_SENTINEL
};

enum ProcessType {MASTER_START_NODE,
                  MASTER_END_NODE,
                  START_NODE,
                  END_NODE,
                  SPLIT_NODE,
                  JOIN_NODE,
                  COMPUTATION_NODE,
                  XCHOICE_NODE,
                  SIMPLE_MERGE_NODE,
                  SOURCE_NODE,
                  SINK_NODE,
                  ABSTRACT_NODE,
                  MATLAB_NODE,
                  ALGORITHM_NODE,
                  OPERATOR_NODE,
                  PROBLEM_NODE,
                  Process /* TODO: Remove this. Currently left for compatibility. */};

enum ProcessState {DETACHED_NODE,
                   NODE_IN_QUEUE,
                   NODE_READY,
                   NODE_RUNNING,
                   NODE_DONE,
                   NODE_ERROR};

enum PortType {INPUT_PORT,
               OUTPUT_PORT,
               INPUT_MOEO_PORT,
               OUTPUT_MOEO_PORT,
               ABSTRACT_PORT};

enum PortState {DATA_RECEIVED,
                AWAITING_DATA,
                DATA_SENT,
                DETACHED_PORT,
                NO_STATE};

namespace Constants {

const char C_DESIGNER[]            = "Designer";
const char MENU_ID[]               = "Designer.Menu";
const char ACTION_ID[]             = "Desinger.Action";
const char COMPLETE_THIS[]         = "Designer.CompleteThis";
const char QUICKFIX_THIS[]         = "Designer.QuickFix";
const char VISUALIZE_WHITESPACE[]  = "Designer.VisualizeWhitespace";
const char CLEAN_WHITESPACE[]      = "Designer.CleanWhitespace";
const char TEXT_WRAPPING[]         = "Designer.TextWrapping";
const char UN_COMMENT_SELECTION[]  = "Designer.UnCommentSelection";
const char FOLD[]                  = "Designer.Fold";
const char UNFOLD[]                = "Designer.Unfold";
const char UNFOLD_ALL[]            = "Designer.UnFoldAll";
const char AUTO_INDENT_SELECTION[] = "Designer.AutoIndentSelection";
const char INCREASE_FONT_SIZE[]    = "Designer.IncreaseFontSize";
const char DECREASE_FONT_SIZE[]    = "Designer.DecreaseFontSize";
const char RESET_FONT_SIZE[]       = "Designer.ResetFontSize";
const char GOTO_BLOCK_START[]      = "Designer.GotoBlockStart";
const char GOTO_BLOCK_START_WITH_SELECTION[] = "Designer.GotoBlockStartWithSelection";
const char GOTO_BLOCK_END[]        = "Designer.GotoBlockEnd";
const char GOTO_BLOCK_END_WITH_SELECTION[] = "Designer.GotoBlockEndWithSelection";
const char SELECT_BLOCK_UP[]       = "Designer.SelectBlockUp";
const char SELECT_BLOCK_DOWN[]     = "Designer.SelectBlockDown";
const char MOVE_LINE_UP[]          = "Designer.MoveLineUp";
const char MOVE_LINE_DOWN[]        = "Designer.MoveLineDown";
const char COPY_LINE_UP[]          = "Designer.CopyLineUp";
const char COPY_LINE_DOWN[]        = "Designer.CopyLineDown";
const char JOIN_LINES[]            = "Designer.JoinLines";
const char INSERT_LINE_ABOVE[]     = "Designer.InsertLineAboveCurrentLine";
const char INSERT_LINE_BELOW[]     = "Designer.InsertLineBelowCurrentLine";
const char UPPERCASE_SELECTION[]   = "Designer.UppercaseSelection";
const char LOWERCASE_SELECTION[]   = "Designer.LowercaseSelection";
const char CUT_LINE[]              = "Designer.CutLine";
const char COPY_LINE[]             = "Designer.CopyLine";
const char DELETE_LINE[]           = "Designer.DeleteLine";
const char DELETE_END_OF_WORD[]    = "Designer.DeleteEndOfWord";
const char DELETE_END_OF_WORD_CAMEL_CASE[] = "Designer.DeleteEndOfWordCamelCase";
const char DELETE_START_OF_WORD[]  = "Designer.DeleteStartOfWord";
const char DELETE_START_OF_WORD_CAMEL_CASE[] = "Designer.DeleteStartOfWordCamelCase";
const char SELECT_ENCODING[]       = "Designer.SelectEncoding";
const char REWRAP_PARAGRAPH[]      =  "Designer.RewrapParagraph";
const char GOTO_LINE_START[]       = "Designer.GotoLineStart";
const char GOTO_LINE_END[]         = "Designer.GotoLineEnd";
const char GOTO_NEXT_LINE[]        = "Designer.GotoNextLine";
const char GOTO_PREVIOUS_LINE[]    = "Designer.GotoPreviousLine";
const char GOTO_PREVIOUS_CHARACTER[] = "Designer.GotoPreviousCharacter";
const char GOTO_NEXT_CHARACTER[]   = "Designer.GotoNextCharacter";
const char GOTO_PREVIOUS_WORD[]    = "Designer.GotoPreviousWord";
const char GOTO_NEXT_WORD[]        = "Designer.GotoNextWord";
const char GOTO_PREVIOUS_WORD_CAMEL_CASE[] = "Designer.GotoPreviousWordCamelCase";
const char GOTO_NEXT_WORD_CAMEL_CASE[] = "Designer.GotoNextWordCamelCase";
const char GOTO_LINE_START_WITH_SELECTION[] = "Designer.GotoLineStartWithSelection";
const char GOTO_LINE_END_WITH_SELECTION[] = "Designer.GotoLineEndWithSelection";
const char GOTO_NEXT_LINE_WITH_SELECTION[] = "Designer.GotoNextLineWithSelection";
const char GOTO_PREVIOUS_LINE_WITH_SELECTION[] = "Designer.GotoPreviousLineWithSelection";
const char GOTO_PREVIOUS_CHARACTER_WITH_SELECTION[] = "Designer.GotoPreviousCharacterWithSelection";
const char GOTO_NEXT_CHARACTER_WITH_SELECTION[] = "Designer.GotoNextCharacterWithSelection";
const char GOTO_PREVIOUS_WORD_WITH_SELECTION[] = "Designer.GotoPreviousWordWithSelection";
const char GOTO_NEXT_WORD_WITH_SELECTION[] = "Designer.GotoNextWordWithSelection";
const char GOTO_PREVIOUS_WORD_CAMEL_CASE_WITH_SELECTION[] = "Designer.GotoPreviousWordCamelCaseWithSelection";
const char GOTO_NEXT_WORD_CAMEL_CASE_WITH_SELECTION[] = "Designer.GotoNextWordCamelCaseWithSelection";
const char C_DESIGNER_MIMETYPE_TEXT[] = "application/lgr";
const char C_DESIGNER_MIMETYPE_EXTENSION[] = "lgr";
const char INFO_SYNTAX_DEFINITION[] = "Designer.InfoSyntaxDefinition";
const char TASK_DOWNLOAD_DEFINITIONS[] = "Designer.Task.Download";
const char TASK_REGISTER_DEFINITIONS[] = "Designer.Task.Register";
const char TASK_OPEN_FILE[]        = "Designer.Task.OpenFile";
const char CIRCULAR_PASTE[]        = "Designer.CircularPaste";
const char SWITCH_UTF8BOM[]        = "Designer.SwitchUtf8bom";
const char INDENT[]        = "Designer.Indent";
const char UNINDENT[]        = "Designer.Unindent";
const char FOLLOW_SYMBOL_UNDER_CURSOR[] = "Designer.FollowSymbolUnderCursor";
const char JUMP_TO_FILE_UNDER_CURSOR[] = "Designer.JumpToFileUnderCursor";

const char *nameForStyle(TextStyle style);
TextStyle styleFromName(const char *name);

const char TEXT_DESIGNER_SETTINGS_CATEGORY[] = "C.Designer";
const char TEXT_DESIGNER_SETTINGS_CATEGORY_ICON[] = ":/core/images/category_design.png";
const char TEXT_DESIGNER_SETTINGS_TR_CATEGORY[] = QT_TRANSLATE_NOOP("Designer", "Optimization Designer");
const char TEXT_DESIGNER_FONT_SETTINGS[] = "A.FontSettings";
const char TEXT_DESIGNER_BEHAVIOR_SETTINGS[] = "B.BehaviourSettings";
const char TEXT_DESIGNER_DISPLAY_SETTINGS[] = "D.DisplaySettings";
const char TEXT_DESIGNER_HIGHLIGHTER_SETTINGS[] = "E.HighlighterSettings";
const char TEXT_DESIGNER_SNIPPETS_SETTINGS[] = "F.SnippetsSettings";

const char SNIPPET_DESIGNER_ID[]     = "Designer.SnippetDesigner";
const char TEXT_SNIPPET_GROUP_ID[] = "Text";

const char GLOBAL_SETTINGS_ID[]    = "Global";

/**
 * Delay before tooltip will be shown near completion assistant proposal
 */
const unsigned COMPLETION_ASSIST_TOOLTIP_DELAY = 100;

const int NODE_DEFAULT_SIZE = 40;
const int PORT_DEFAULT_SIZE = 8;
const qint32 MAGIC_HEADER   = 0xBAADF00D;
const qint32 DESIGNER_VERSION_MAJOR = 0;
const qint32 DESIGNER_VERSION_MINOR = 0;
const qint32 DESIGNER_VERSION_BUILD = 76;

} // namespace Constants
} // namespace Designer

#endif // DESIGNERCONSTANTS_H
