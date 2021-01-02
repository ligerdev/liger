/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef QALGOSMSEMOANODEFACTORY_H
#define QALGOSMSEMOANODEFACTORY_H

#include <qtigon/qtigon_global.h>
#include <qtigon/algorithms/smsemoa/qalgosmsemoanode.h>

#include <designer/iengine/iprocessnodefactory.h>

#include <QToolButton>
#include <QObject>

namespace QTigon {

class QTIGON_EXPORT QAlgoSMSEMOANodeFactory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    QAlgoSMSEMOANodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("QTigon::QAlgoSMSEMOANode"); }
    bool isSelected();
    void unSelect();

public slots:
    void processButtonTriggered();

private:
    bool m_selected;
    QToolButton* m_toolBoxButton;
};

} // namespace QTigon

#endif // QALGOSMSEMOANODEFACTORY_H