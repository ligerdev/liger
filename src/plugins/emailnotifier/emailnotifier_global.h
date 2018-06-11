/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef EMAILNOTIFIER_GLOBAL_H
#define EMAILNOTIFIER_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(EMAILNOTIFIER_LIBRARY)
#   define EMAILNOTIFIER_EXPORT Q_DECL_EXPORT
#else
#   define EMAILNOTIFIER Q_DECL_IMPORT
#endif

#endif // EMAILNOTIFIER_GLOBAL_H

