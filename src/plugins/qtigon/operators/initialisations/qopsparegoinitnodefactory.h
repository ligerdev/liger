/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#pragma once 

#include <qtigon/qtigon_global.h>
#include <qtigon/operators/initialisations/qopsparegoinitnode.h>

#include <designer/iengine/iprocessnodefactory.h>

#include <QToolButton>
#include <QObject>

namespace QTigon {

class QTIGON_EXPORT QOpSparegoInitNodeFactory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    QOpSparegoInitNodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("QTigon::QOpSparegoInitNode"); }
    bool isSelected();
    void unSelect();

public slots:
    void processButtonTriggered();

private:
    QToolButton* m_toolBoxButton;
};

} // namespace QTigon