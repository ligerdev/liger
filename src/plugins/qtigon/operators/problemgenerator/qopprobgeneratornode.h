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
#ifndef  QOPPROBGENERATORNODE_H 
#define  QOPPROBGENERATORNODE_H 

#include <qtigon/qtigon_global.h>
#include <qtigon/qtigonconstants.h>
#include <designer/designerconstants.h>
#include <designer/iengine/iprocessnode.h>

#include <QObject>

namespace QTigon {
class QOpProbFormulationNodeForm;

class QTIGON_EXPORT QOpProbGeneratorNode : public Designer::IProcessNode
{
    Q_OBJECT

public:
    QOpProbGeneratorNode();
    ~QOpProbGeneratorNode();
    void updateProcessState(Designer::ProcessState state);

protected:
    void readDataProperties(QXmlStreamReader &xmlReader);
    void writeDataProperties(QXmlStreamWriter &xmlWriter);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private: 
    QOpProbFormulationNodeForm* m_qOpForm;

};

} // namespace QTigon

#endif //  QOPPROBGENERATORNODE_H 
