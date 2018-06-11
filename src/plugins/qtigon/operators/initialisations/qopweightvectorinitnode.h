/****************************************************************************
**
** Copyright (C) 2012-2016 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef  QOPWEIGHTVECTORINITNODE_H 
#define  QOPWEIGHTVECTORINITNODE_H 

#include <qtigon/qtigon_global.h>
#include <qtigon/qtigonconstants.h>
#include <designer/designerconstants.h>
#include <designer/iengine/iprocessnode.h>
#include <qtigon/dialogs/qoperatordiag.h>

#include <QObject>

namespace QTigon {

class QTIGON_EXPORT QOpWeightVectorInitNode : public Designer::IProcessNode
{
    Q_OBJECT

public:
    QOpWeightVectorInitNode();
    ~QOpWeightVectorInitNode() {}

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private: 
    QOperatorDiag* m_dialog;
};

} // namespace QTigon

#endif //  QOPWEIGHTVECTORINITNODE_H 
