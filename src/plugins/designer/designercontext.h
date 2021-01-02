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
#ifndef DESIGNERCONTEXT_H
#define DESIGNERCONTEXT_H

#include <coreplugin/icontext.h>

namespace Designer {
namespace Internal {

class DesignerContext : public Core::IContext
{
public:
    explicit DesignerContext(const Core::Context &contexts,
                             QWidget *widget,
                             QObject *parent = 0);

    virtual QString contextHelpId() const;
};

} // namespace Internal
} // namespace Designer

#endif // DESIGNERCONTEXT_H
