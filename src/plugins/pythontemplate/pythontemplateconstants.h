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
#ifndef PYTHONTEMPLATECONSTANTS_H
#define PYTHONTEMPLATECONSTANTS_H

namespace PythonTemplate {
namespace Constants {

const char * const ACTION_ID = "PythonTemplate.Action";
const char * const MENU_ID   = "PythonTemplate.Menu";

const char* const WIZARD_CATEGORY = "T.Template";
const char* const WIZARD_TR_CATEGORY = QT_TRANSLATE_NOOP("Designer", "Liger Templates");
const char* const WIZARD_DISPLAY_NAME = "Python Template";
const char* const WIZARD_ID = "F.CustomLigerPlugin";
const char* const WIZARD_DESCRIPTIONS = "Create a Python template file for Liger";
const char* const EXTENSION = ".py";

} // namespace Constants
} // namespace PythonTemplate


#endif // PYTHONTEMPLATECONSTANTS_H
