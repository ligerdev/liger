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
#ifndef  {% filter upper %}{{ StringName }}{% endfilter %}NODE_H 
#define  {% filter upper %}{{ StringName }}{% endfilter %}NODE_H 

#include <{{plugin_name}}/{{plugin_name}}_global.h>
#include <{{plugin_name}}/{{plugin_name}}constants.h>
#include <designer/designerconstants.h>
#include <designer/iengine/iport.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <designer/iengine/iprocessnode.h>
#include <{{plugin_name}}/vizdataobject.h>
#include <QUrl>
#include <QWebView>


#include <QList>
#include <QString>
#include <QColor>
#include <QSet>
#include <QObject>

namespace {{Namespace}} {
class {{StringName}}Form;
class QViz{{StringName}}Node : public Designer::IProcessNode
{
    Q_OBJECT

public:
    QViz{{StringName}}Node();
    ~QViz{{StringName}}Node();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    VizDataObject* m_vizdataobject;    
    QWebView*      m_webview;
    arma::mat      m_outputdata;
    QUrl           m_url;
    {{StringName}}Form* m_form;
};

} // namespace {{Namespace}}

#endif //  {% filter upper %}{{StringName}}{% endfilter %}NODE_H 
