/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <designer/designerconstants.h>
#include <designer/ldesigner/ldesigner.h>
#include <designer/ldesigner/ldesignerfactory.h>
#include <designer/ldesigner/ldesigndocument.h>

#include <coreplugin/coreconstants.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/infobar.h>

#include <QCoreApplication>
#include <QDebug>

using namespace Designer;
using namespace Designer::Internal;

LDesignerFactory::LDesignerFactory(QObject *parent)
  : Core::IEditorFactory(parent)
{
    m_mimeTypes << QLatin1String(Designer::Constants::C_DESIGNER_MIMETYPE_TEXT);
}

LDesignerFactory::~LDesignerFactory()
{

}

Core::Id LDesignerFactory::id() const
{
    return Core::Constants::K_DEFAULT_DESIGNER_ID;
}

QString LDesignerFactory::displayName() const
{
    return qApp->translate("OpenWith::Designers",
                           Core::Constants::K_DEFAULT_TEXT_EDITOR_DISPLAY_NAME);
}

Core::IEditor *LDesignerFactory::createEditor()
{
    LDesignerWidget* rc = new LDesignerWidget(0);

    connect(rc, SIGNAL(configured(Core::IEditor*)),
            this, SLOT(updateDesignerInfoBar(Core::IEditor*)));

    LDesigner *designer = new LDesigner(rc);
    connect(rc, SIGNAL(changed()), designer, SIGNAL(contentsChanged()));

    updateDesignerInfoBar(designer);
    return designer;
}

// DEPRECATED - MARKED FOR DELETION
void LDesignerFactory::updateDesignerInfoBar(Core::IEditor *designer)
{
    Q_UNUSED(designer)

}

void LDesignerFactory::addMimeType(const QString &type)
{
    m_mimeTypes.append(type);
}

QStringList LDesignerFactory::mimeTypes() const
{
    return m_mimeTypes;
}
