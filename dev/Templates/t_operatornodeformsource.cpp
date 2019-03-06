/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
**
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
#include "{% filter lower %}{{ ClassName }}{% endfilter%}form.h"
#include "ui_{% filter lower %}{{ ClassName }}{% endfilter%}form.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QPushButton>

using namespace {{ Namespace }};

{{ ClassName }}Form::{{ ClassName }}Form(QWidget *parent) :
    QWidget(parent),    
    ui(new Ui::{{ ClassName }}Form)
{
    ui->setupUi(this);
}

{{ ClassName }}Form::~{{ ClassName }}Form()
{
    delete ui;
}
