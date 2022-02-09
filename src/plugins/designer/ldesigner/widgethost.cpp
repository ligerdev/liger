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
#include "widgethost.h"
#include "designerwidgetcontainer.h"
//#include "widgethostconstants.h"

#include <QPalette>
#include <QLayout>
#include <QFrame>
#include <QResizeEvent>
#include <QDebug>

using namespace SharedTools;

// ---------- WidgetHost
WidgetHost::WidgetHost(QWidget *parent, Designer::LDesigner *formWindow) :
    QScrollArea(parent),
    m_designWindow(formWindow),
    m_designerWidgetContainer(new Internal::DesignerWidgetContainer)
{
    setWidget(m_designerWidgetContainer);
    setWidgetResizable(true);

    // Disable the scrollbars of the host since the designerWidget has its own scroll bars.
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Re-set flag (gets cleared by QScrollArea): Make the resize grip of a mainwindow form find the resizer as resizable window.
    m_designerWidgetContainer->setWindowFlags(m_designerWidgetContainer->windowFlags() | Qt::SubWindow);
    setFormWindow(formWindow);
}

WidgetHost::~WidgetHost()
{
    if (m_designWindow)
        delete m_designWindow;
}

void WidgetHost::setFormWindow(Designer::LDesigner *fw)
{
    m_designWindow = fw;
    if (!fw)
        return;

    m_designerWidgetContainer->setFormWindow(fw);

    setBackgroundRole(QPalette::Base);
    m_designWindow->widget()->setAutoFillBackground(true);
    m_designWindow->widget()->setBackgroundRole(QPalette::Background);
}

QSize WidgetHost::formWindowSize() const
{
    if (!m_designWindow)
        return QSize();    
    return m_designWindow->widget()->size();
}

QWidget *WidgetHost::integrationContainer() const
{
    return m_designerWidgetContainer;
}
