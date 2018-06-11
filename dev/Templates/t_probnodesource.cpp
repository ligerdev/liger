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
#include <{{ plugin_name }}/problems/{% filter lower %}{{ ClassName }}{% endfilter %}.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/messagemanager.h>
#include <coreplugin/editormanager/editormanager.h>
#include <coreplugin/icore.h>

#include <QtGui>
#include <QDir>
#include <QChar>
#include <QMessageBox>
#include <QString>
#include <iostream>

#include <QDebug>

#include <metal/problems/{{ StringName }}.h>
#include <{{ plugin_name }}/dialogs/{% filter lower %}{{ ClassName }}{% endfilter %}form.h>

using namespace {{ Namespace }};

{{ ClassName }}::{{ ClassName }}()
    : m_problem(0),
      m_nvar({{MinNvar}}),
      m_nobj({{MinNobj}})
{
    m_text         = QString("C");
    m_procType     = PROBLEM_NODE;
    m_textColor    = Qt::darkGreen;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;
    m_numberOfOutputs = 1;
    m_numberOfInputs  = 1;
    m_numberOfConnectedOutputs = 0;
    m_numberOfConnectedInputs  = 0;

    QFile img_file(":/{{ plugin_name }}/images/{% filter lower %}{{ ClassName }}{% endfilter %}.svg");
    if(img_file.exists()) {
        setSvg(":/{{ plugin_name }}/images/{% filter lower %}{{ ClassName }}{% endfilter %}.svg");
    } else {
        setSvg(":/{{ plugin_name }}/images/default_qprobnode.svg");
    }
    img_file.close();

    m_problemForm = new {{ ClassName }}Form();

    updateParameters();

    connect(m_problemForm->m_buttonBox,SIGNAL(accepted()),
            this,SLOT(updateParameters()));

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();
}

{{ ClassName }}::~{{ ClassName }}()
{
    if(m_problem) {
        delete m_problem;
        m_problem = 0;
    }
    if(m_problemForm) {
        delete m_problemForm;
        m_problemForm;
    }
}

void {{ ClassName }}::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event)

    updateParameters();

    m_problemForm->show();
    m_problemForm->raise();
}

void {{ ClassName }}::compute()
{
    updateParameters();

    if(!m_problem) {
        {% if StringName in ['ZDT1','ZDT2','ZDT3','ZDT4','ZDT5','ZDT6'] %}
        m_problem = new {{ StringName }}("Real");
        {% else %}
        m_problem = new {{ StringName }}("Real",m_nvar,m_nobj);
        {% endif %}
    }
    m_data.setProblem(m_problem);
}

void {{ ClassName }}::updateParameters()
{
    m_problemForm->nvarEditEnabled(true);
    m_problemForm->nobjEditEnabled(true);
    if(m_globalData->numberOfVariables() != 0) {
        m_nvar = m_globalData->numberOfVariables();
        m_problemForm->setNvar(m_nvar);
        m_problemForm->nvarEditEnabled(false);
    } else {
        m_nvar = m_problemForm->nvar();
    }
    if(m_globalData->numberOfObjectives() != 0) {
        m_nobj = m_globalData->numberOfObjectives();
        m_problemForm->setNobj(m_nobj);
        m_problemForm->nobjEditEnabled(false);
    } else {
        m_nobj = m_problemForm->nobj();
    }

    if(m_problem) {
        delete m_problem;
        {% if StringName in ['ZDT1','ZDT2','ZDT3','ZDT4','ZDT5','ZDT6'] %}
        m_problem = new {{ StringName }}("Real",m_nvar);
        {% else %}
        m_problem = new {{ StringName }}("Real",m_nvar,m_nobj);
        {% endif %}
    } else {
        {% if StringName in ['ZDT1','ZDT2','ZDT3','ZDT4','ZDT5','ZDT6'] %}
        m_problem = new {{ StringName }}("Real",m_nvar);
        {% else %}
        m_problem = new {{ StringName }}("Real",m_nvar,m_nobj);
        {% endif %}
    }
}
