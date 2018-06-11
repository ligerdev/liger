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
#include "designerconstants.h"
#include "ldesigner.h"
#include "ldesigndocument.h"

#include <coreplugin/coreconstants.h>
#include <coreplugin/mimedatabase.h>

#include <QSharedPointer>
#include <QFileInfo>

#include <QDebug>

using namespace Designer;
using namespace Designer::Internal;

LDesigner::LDesigner(LDesignerWidget *designer)
    : e(designer),
      m_toolBar(new QToolBar),
      m_stretchWidget(new QWidget)
{
    setWidget(e);
    m_stretchWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_toolBar->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    m_toolBar->addWidget(m_stretchWidget);

    setContext(Core::Context(Core::Constants::K_DEFAULT_DESIGNER_ID,
                             Designer::Constants::C_DESIGNER));
}

LDesigner::~LDesigner()
{
    delete m_toolBar;
    delete e;
}

bool LDesigner::open(QString *errorString, const QString &fileName, const QString &realFileName)
{
    return e->open(errorString, fileName, realFileName);
}

Core::IDocument *LDesigner::document()
{
    return e->designerDocument();
}

QWidget *LDesigner::toolBar()
{
    return m_toolBar;
}

bool LDesigner::isTemporary() const
{
    return false;
}

Core::Id LDesigner::id() const
{
    return Core::Constants::K_DEFAULT_DESIGNER_ID;
}

LDesignerWidget *LDesigner::widget() const
{
    return e;
}

bool LDesigner::hasFocus() const
{
    return e->scene()->hasFocus();
}

bool LDesigner::isActive () const
{
    return e->scene()->isActive();
}

QString LDesigner::displayName() const
{
    return e->displayName();
}
void LDesigner::setDisplayName(const QString &title)
{
    e->setDisplayName(title);
    /*emit changed();*/
}

//QByteArray LDesigner::saveState() const
//{
//    return e->saveState();
//}

//bool LDesigner::restoreState(const QByteArray &state)
//{
//    return e->restoreState(state);
//}

void LDesigner::insertExtraToolBarWidget(LDesigner::Side side,
                                         QWidget *widget)
{
    if (widget->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag) {
        if (m_stretchWidget)
            m_stretchWidget->deleteLater();
        m_stretchWidget = 0;
    }

    if (side == Right) {
        //        m_toolBar->insertWidget(m_cursorPositionLabelAction, widget);
    } else {
        m_toolBar->insertWidget(m_toolBar->actions().first(), widget);
    }
}

QString LDesigner::contextHelpId() const
{
    return 0;
}

bool LDesigner::loadDesign()
{
    return e->loadDesign();
}
