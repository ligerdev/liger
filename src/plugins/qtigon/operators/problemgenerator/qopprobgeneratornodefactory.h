/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef QOPPROBGENERATORNODEFACTORY_H
#define QOPPROBGENERATORNODEFACTORY_H

#include <qtigon/qtigon_global.h>
#include <qtigon/operators/problemgenerator/qopprobgeneratornode.h>

#include <designer/iengine/iprocessnodefactory.h>

#include <QToolButton>
#include <QObject>

namespace QTigon {

class QTIGON_EXPORT QOpProbGeneratorNodeFactory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    QOpProbGeneratorNodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("QTigon::QOpProbGeneratorNode"); }
    bool isSelected();
    void unSelect();

public slots:
    void processButtonTriggered();

private:
    QToolButton* m_toolBoxButton;
};

} // namespace QTigon

#endif // QOPPROBGENERATORNODEFACTORY_H
