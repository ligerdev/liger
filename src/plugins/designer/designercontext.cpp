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
#include "designercontext.h"
#include "designeditorw.h"

#include <QWidget>
#include <QDebug>

enum { debug = 0 };

namespace Designer {
namespace Internal {

DesignerContext::DesignerContext(const Core::Context &context,
                                 QWidget *widget, QObject *parent)
  : Core::IContext(parent)
{
    setContext(context);
    setWidget(widget);
}

QString DesignerContext::contextHelpId() const
{
    return QString();
}

} // namespace Internal
} // namespace Designer
