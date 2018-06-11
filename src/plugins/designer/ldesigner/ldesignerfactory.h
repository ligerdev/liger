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
#ifndef LDESIGNERFACTORY_H
#define LDESIGNERFACTORY_H

#include <designer/designer_global.h>
#include <coreplugin/editormanager/ieditorfactory.h>

#include <QStringList>

namespace Designer {
namespace Internal {

class DESIGNER_EXPORT LDesignerFactory : public Core::IEditorFactory
{
    Q_OBJECT

public:
    LDesignerFactory(QObject *parent = 0);
    virtual ~LDesignerFactory();

    void addMimeType(const QString &type);
    virtual QStringList mimeTypes() const;
    //Core::IEditorFactory
    Core::Id id() const;
    QString displayName() const;
    Core::IEditor *createEditor();

private slots:
    void updateDesignerInfoBar(Core::IEditor *designer);

private:
    QStringList m_mimeTypes;
};

} // namespace Internal
} // namespace Designer

#endif // LDESIGNERFACTORY_H
