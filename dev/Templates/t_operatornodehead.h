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

#include <{% filter lower %}{{ Namespace }}{% endfilter %}/{% filter lower %}{{ Namespace }}{% endfilter %}_global.h>
#include <{% filter lower %}{{ Namespace }}{% endfilter %}/{% filter lower %}{{ Namespace }}{% endfilter %}constants.h>
#include <designer/designerconstants.h>
#include <designer/iengine/iprocessnode.h>

#include <QObject>

class QOperatorDiag;

namespace {{ Namespace }} {

class {% filter upper %}{{ Namespace }}{% endfilter %}_EXPORT {{ ClassName }} : public Designer::IProcessNode
{
    Q_OBJECT

public:
    {{ ClassName }}();
    ~{{ ClassName }}();
    void updateProcessState(Designer::ProcessState state);

protected:
	void readDataProperties(QXmlStreamReader &xmlReader);
	void writeDataProperties(QXmlStreamWriter &xmlWriter);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private: 
	QOperatorDiag* m_dialog;
};

} // namespace {{ Namespace }}
