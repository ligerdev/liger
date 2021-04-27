/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef MASTERSTARTNODEFACTORY_H
#define MASTERSTARTNODEFACTORY_H

#include <qtigon/qtigon_global.h>

#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>
#include <designer/iengine/iportlink.h>
#include <designer/iengine/inputport.h>
#include <designer/iengine/outputport.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iprocessnodefactory.h>

#include <designer/widgets/designertoolbox.h>
#include <extensionsystem/pluginmanager.h>

#include <QToolButton>
#include <QObject>

namespace QTigon {

class QTIGON_EXPORT MasterStartNodeFactory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    MasterStartNodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("QTigon::MasterStartNode"); }
    bool isSelected();
    void unSelect();

public slots:
    void processButtonTriggered();

private:
    QToolButton* m_toolBoxButton;
};

} // namespace QTigon

#endif // MASTERSTARTNODEFACTORY_H
