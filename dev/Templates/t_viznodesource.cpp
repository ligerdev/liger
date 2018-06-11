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
#include <{{plugin_name}}/qviz{% filter lower %}{{StringName}}{% endfilter %}node.h>
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

#include <{{plugin_name}}/forms/{% filter lower %}{{StringName}}{% endfilter %}form.h>

using namespace {{Namespace}};
using namespace Designer;

QViz{{StringName}}Node::QViz{{StringName}}Node()
    : m_vizdataobject(0),
      m_webview(0),
      m_form(0),
      m_url("qrc:/{{plugin_name}}/html/{% filter lower %}{{StringName}}{% endfilter %}.html")
{
    m_text         = QString("C");
    m_procType     = SINK_NODE;
    m_textColor    = Qt::darkGreen;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;
    m_numberOfOutputs = 0;
    m_numberOfInputs  = 1;
    m_numberOfConnectedOutputs = 0;
    m_numberOfConnectedInputs  = 0;    

    m_form = new {{StringName}}Form();

    m_form->setUnifiedTitleAndToolBarOnMac(true);
    m_webview = m_form->webView();    


    m_vizdataobject = new VizDataObject(m_webview);
    m_vizdataobject->setWebView(m_webview);
    QUrl baseUrl = QUrl("qrc:/{{plugin_name}}/html/");
    QFile source(":/{{plugin_name}}/html/{% filter lower %}{{StringName}}{% endfilter %}.html");
    source.open(QIODevice::ReadOnly);

    QString content = QString::fromUtf8(source.readAll().constData());

    setSvg(":/{{plugin_name}}/images/qviz{% filter lower %}{{StringName}}{% endfilter %}node.svg");

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    initializeNode();
}

void QViz{{StringName}}Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    m_outputdata = m_data.objectiveVectors();
    m_vizdataobject->setData(m_outputdata);
    m_webview->load(m_url);
    m_form->show();
    m_form->raise();
}

QViz{{StringName}}Node::~QViz{{StringName}}Node()
{
    if(m_form)
        delete m_form;

}