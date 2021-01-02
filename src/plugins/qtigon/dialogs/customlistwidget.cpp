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
#include <qtigon/dialogs/customlistwidget.h>
#include <QMouseEvent>
CustomListWidget::CustomListWidget(QWidget *parent)
    :QListWidget(parent)
{

}

void CustomListWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        QModelIndex index = indexAt(event->pos());
        if(index.row() == -1) {
            emit emptyClick();
        }
    }
    QListWidget::mousePressEvent(event);
}
