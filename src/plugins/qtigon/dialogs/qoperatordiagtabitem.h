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
#ifndef QOPERATORDIAGTABITEM_H
#define QOPERATORDIAGTABITEM_H

#include <QWidget>

class QOperatorDiagTabItem : public QWidget
{
    Q_OBJECT

public:
    explicit QOperatorDiagTabItem(QWidget *parent = 0);
    virtual void reset() = 0;
    virtual void save()  = 0;
};

#endif // QOPERATORDIAGTABITEM_H
