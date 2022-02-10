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
****************************************************************************/
#ifndef CSDP_GLOBAL
#define CSDP_GLOBAL

#ifdef _WIN32
#   ifdef LIGER_CSDP_LIB
#       define LIGER_CSDP_EXPORT __declspec(dllexport)
#   elif defined(LIGER_CSDP_STATIC_LIB)
#       define LIGER_CSDP_EXPORT
#   else
#       define LIGER_CSDP_EXPORT __declspec(dllimport)
#   endif
#elif __GNUC__ >= 4
#   define LIGER_CSDP_EXPORT __attribute__((visibility("default")))
#else
#   define LIGER_CSDP_EXPORT
#endif

#endif // CSDP_GLOBAL
