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
#ifndef DESIGNER_GLOBAL_H
#define DESIGNER_GLOBAL_H

#include <qglobal.h>
#include <QSharedPointer>

#if defined(DESIGNER_LIBRARY)
#  define DESIGNER_EXPORT Q_DECL_EXPORT
#else
#  define DESIGNER_EXPORT Q_DECL_IMPORT
#endif

namespace Designer {
class InputPort;
class OutputPort;
class IPort;
typedef  QSharedPointer<IPort>      IPortSP;
typedef  QSharedPointer<InputPort>  InputPortSP;
typedef  QSharedPointer<OutputPort> OutputPortSP;
}

#endif // DESIGNER_GLOBAL_H
