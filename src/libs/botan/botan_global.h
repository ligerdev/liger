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
#ifndef BOTAN_GLOBAL
#define BOTAN_GLOBAL

#ifdef _WIN32
#   ifdef LIGER_BOTAN_LIB
#       define BOTAN_DLL __declspec(dllexport)
#   elif defined(LIGER_BOTAN_STATIC_LIB)
#       define BOTAN_DLL
#   else
#       define BOTAN_DLL __declspec(dllimport)
#   endif
#elif __GNUC__ >= 4
#   define BOTAN_DLL __attribute__((visibility("default")))
#else
#   define BOTAN_DLL
#endif


#endif // BOTAN_GLOBAL

