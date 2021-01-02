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
****************************************************************************/
#pragma once

#ifdef _WIN32
#   ifdef LIGER_JSON_LIB
#       define LIGER_JSON_EXPORT __declspec(dllexport)
#   elif defined(LIGER_JSON_STATIC_LIB)
#       define LIGER_JSON_EXPORT
#   else
#       define LIGER_JSON_EXPORT __declspec(dllimport)
#   endif
#elif __GNUC__ >= 4
#   define LIGER_JSON_EXPORT __attribute__((visibility("default")))
#else
#   define LIGER_JSON_EXPORT
#endif
