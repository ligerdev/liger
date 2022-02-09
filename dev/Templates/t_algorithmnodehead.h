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

#include <{% filter lower %}{{ Namespace }}{% endfilter %}/{% filter lower %}{{ Namespace }}{% endfilter %}_global.h>
#include <{% filter lower %}{{ Namespace }}{% endfilter %}/{% filter lower %}{{ Namespace }}{% endfilter %}constants.h>
#include <{% filter lower %}{{ Namespace }}{% endfilter %}/dialogs/qalgorithmdialog.h>
#include <designer/iengine/iprocessnode.h>

#include <QObject>

namespace {{ Namespace }} {

class {{ ClassName }} : public Designer::IProcessNode
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
    QAlgorithmDialog* m_dialog;
};

} // namespace {{ Namespace }}
