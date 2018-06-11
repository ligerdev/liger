/****************************************************************************
**
** Copyright (C) 2012-2013 The University of Sheffield (www.sheffield.ac.uk)
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
    ui(new Ui::{{ ClassName }}Form),
    m_nvar({{MinNvar}}),
    m_nobj({{MinNobj}})
{
    ui->setupUi(this);
    m_buttonBox = ui->buttonBox;
    connect(ui->buttonBox, SIGNAL(accepted()),this,SLOT(updateParameters()));
    setNvar(m_nvar);
    setNobj(m_nobj);
    {% if MaxNobj is sameas MinNobj -%}
    QRegExp nobjRegExp("[{{MaxNobj}}]");
    {% else -%}
    QRegExp nobjRegExp("[1-9][0-9]{0,3}");
    {% endif -%}
    QRegExp nvarRegExp("[1-9][0-9]{0,3}");

    {% if MaxNobj is sameas MinNobj -%}
    ui->m_nobjEdit->setEnabled(false);
    ui->m_nobjEdit->setReadOnly(true);
    
    {% endif -%}

    ui->m_nobjEdit->setValidator(new QRegExpValidator(nobjRegExp,this));    
    ui->m_nvarEdit->setValidator(new QRegExpValidator(nvarRegExp,this));    

    connect(ui->m_nobjEdit,SIGNAL(textChanged(QString)),
            this, SLOT(nobjEditTextChanged()));
    connect(ui->m_nvarEdit,SIGNAL(textChanged(QString)),
            this,SLOT(nvarEditTextChanged()));    
}

{{ ClassName }}Form::~{{ ClassName }}Form()
{
    delete ui;
}

qint32 {{ ClassName }}Form::nvar()
{
    return m_nvar;
}

qint32 {{ ClassName }}Form::nobj()
{
    return m_nobj;
}

void {{ ClassName }}Form::nobjEditEnabled(bool b)
{
    ui->m_nobjEdit->setEnabled(b);
}

void {{ ClassName }}Form::nvarEditEnabled(bool b)
{
    ui->m_nvarEdit->setEnabled(b);
}

qint32 {{ ClassName }}Form::p_nvar()
{
    QString text = ui->m_nvarEdit->text();
    return text.toInt();
}

qint32 {{ ClassName }}Form::p_nobj()
{
    QString text = ui->m_nobjEdit->text();
    return text.toInt();
}

void {{ ClassName }}Form::setNvar(qint32 nvar)
{
    QString snvar = QString("%1").arg(nvar);
    ui->m_nvarEdit->setText(snvar);
}

void {{ ClassName }}Form::setNobj(qint32 nobj)
{
    QString snobj = QString("%1").arg(nobj);
    ui->m_nobjEdit->setText(snobj);
}

void {{ ClassName }}Form::updateParameters()
{
    m_nvar = p_nvar();
    m_nobj = p_nobj();
}


void {{ ClassName }}Form::nobjEditTextChanged()
{
    QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
    button->setEnabled(ui->m_nobjEdit->hasAcceptableInput());
}

void {{ ClassName }}Form::nvarEditTextChanged()
{
    QPushButton* button = ui->buttonBox->button(QDialogButtonBox::Ok);
    button->setEnabled(ui->m_nvarEdit->hasAcceptableInput());
}