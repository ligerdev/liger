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
#include "designerwidgetcontainer.h"

#include <QDebug>

#include <QResizeEvent>
#include <QPalette>
#include <QLayout>
#include <QFrame>

enum { debugFormResizer = 0 };

using namespace SharedTools::Internal;

DesignerWidgetContainer::DesignerWidgetContainer(QWidget *parent) :
    QWidget(parent),
    m_frame(new QFrame),
    m_designWindow(0)
{
    // Make the resize grip of a mainwindow form find us as resizable window.
    setWindowFlags(windowFlags() | Qt::SubWindow);
    setBackgroundRole(QPalette::Base);

    QVBoxLayout *handleLayout = new QVBoxLayout(this);
    handleLayout->addWidget(m_frame);
    handleLayout->setMargin(0);
    handleLayout->setSpacing(0);

    // Uncomment once dev is complete.
//    m_frame->setFrameStyle(QFrame::NoFrame);
    m_frame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

    QVBoxLayout *layout = new QVBoxLayout(m_frame);
    layout->setMargin(0);
    layout->setSpacing(0);
}

DesignerWidgetContainer::~DesignerWidgetContainer()
{
    delete m_frame;
}

void DesignerWidgetContainer::setFormWindow(Designer::LDesigner *fw)
{
    if (debugFormResizer)
        qDebug() << "DesignerWidgetContainer::setFormWindow " << fw;
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_frame->layout());
    Q_ASSERT(layout);
    if (layout->count())
        delete layout->takeAt(0);
    m_designWindow = fw;

    if (m_designWindow)
        layout->addWidget(m_designWindow->widget());

    layout->setMargin(0);
    layout->setSpacing(0);
}

QWidget *DesignerWidgetContainer::mainContainer()
{
    if (m_designWindow)
        return m_designWindow->widget();
    return 0;
}
