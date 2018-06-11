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
#ifndef {% filter upper %} {{ ClassName }}_H {% endfilter %}
#define {% filter upper %} {{ ClassName }}_H {% endfilter %}

#include <{{ plugin_name }}/{{ plugin_name }}_global.h>
#include <{{ plugin_name }}/{{ plugin_name }}constants.h>
#include <designer/designerconstants.h>
#include <arma/include/armadillo>
#include <designer/iengine/iport.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <designer/iengine/iprocessnode.h>

#include <QList>
#include <QString>
#include <QColor>
#include <QSet>
#include <QObject>

using namespace Designer;

namespace {{ Namespace }} {
class {{ ClassName }}Form;

class {% filter upper %}{{ plugin_name }}{% endfilter %}_EXPORT {{ ClassName }} : public IProcessNode
{
    Q_OBJECT

public:
    {{ ClassName }}();
    ~{{ ClassName }}();

    void compute();

public slots:
    void updateParameters();

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    IProblem* m_problem;
    qint32 m_nvar;
    qint32 m_nobj;
    QMap<qint32, qreal>  m_realParams;
    QMap<qint32, qint32> m_intParams;

    {{ ClassName }}Form*  m_problemForm;
};

} // namespace {{ Namespace }}

#endif // {% filter upper %} {{ ClassName }}_H {% endfilter %}