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
#ifndef DESIGNEDITORFACTORY_H
#define DESIGNEDITORFACTORY_H

#include <designer/designer_global.h>
#include <coreplugin/editormanager/ieditorfactory.h>

namespace Designer {
namespace Internal {

class DESIGNER_EXPORT DesignEditorFactory : public Core::IEditorFactory
{
    Q_OBJECT

public:
    DesignEditorFactory();

    Core::IEditor *createEditor();

private slots:
    void designerModeClicked();

};

} // namespace Internal
} // namespace Designer

#endif // DESIGNEDITORFACTORY_H
