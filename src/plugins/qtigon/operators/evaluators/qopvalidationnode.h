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
#pragma once 

#include <qtigon/qtigon_global.h>
#include <qtigon/qtigonconstants.h>
#include <designer/designerconstants.h>
#include <designer/iengine/iprocessnode.h>

#include <QObject>

class QOperatorDiag;

namespace QTigon {

class QTIGON_EXPORT QOpValidationNode : public Designer::IProcessNode
{
    Q_OBJECT

public:
    QOpValidationNode();
    ~QOpValidationNode();
    void updateProcessState(Designer::ProcessState state);

protected:
	void readDataProperties(QXmlStreamReader &xmlReader);
	void writeDataProperties(QXmlStreamWriter &xmlWriter);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private: 
	QOperatorDiag* m_dialog;
};

} // namespace QTigon