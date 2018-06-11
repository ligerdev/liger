/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef MASTERENDNODEFACTORY_H
#define MASTERENDNODEFACTORY_H

#include <qtigon/qtigon_global.h>
#include <qtigon/masterendnode.h>

#include <designer/iengine/iengine.h>
#include <designer/iengine/ienginefactory.h>
#include <designer/iengine/portlinkfactory.h>
#include <designer/iengine/iprocessnode.h>
#include <designer/iengine/iprocessnodefactory.h>

#include <designer/widgets/designertoolbox.h>
#include <extensionsystem/pluginmanager.h>

#include <QToolButton>
#include <QObject>

namespace QTigon {

class QTIGON_EXPORT MasterEndNodeFactory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    MasterEndNodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("QTigon::MasterEndNode"); }
    bool isSelected();
    void unSelect();

public slots:
    void processButtonTriggered();

private:
    QToolButton* m_toolBoxButton;
};

} // namespace QTigon

#endif // MASTERENDNODEFACTORY_H
