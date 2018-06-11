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
#ifndef {% filter upper %}{{ ClassName }}{% endfilter %}FACTORY_H
#define {% filter upper %}{{ ClassName }}{% endfilter %}FACTORY_H

#include <{{ plugin_name }}/{{ plugin_name }}_global.h>
#include <{{ plugin_name }}/problems/{% filter lower %}{{ ClassName }}{% endfilter %}.h>

#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iprocessnodefactory.h>

#include <designer/widgets/designertoolbox.h>

#include <QToolButton>
#include <QObject>

namespace {{ Namespace }} {

class {% filter upper %}{{ plugin_name }}{% endfilter %}_EXPORT {{ ClassName }}Factory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    {{ ClassName }}Factory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("{{ Namespace }}::{{ ClassName }}"); }
    bool isSelected();

public slots:
    void processButtonTriggered();

private:
    bool m_selected;
    QToolButton* m_toolBoxButton;
};


} // namespace {{ Namespace }}


#endif // {% filter upper %}{{ ClassName }}{% endfilter %}FACTORY_H
