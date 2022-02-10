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
#ifndef QVIZOBJECTIVERANKNODEFACTORY_H
#define QVIZOBJECTIVERANKNODEFACTORY_H
#include <visualisation/visualisation_global.h>
#include <visualisation/qvizobjectiveranknode.h>

#include <designer/iengine/iprocessnodefactory.h>

#include <QObject>

namespace Designer {
class IProcessNode;
}

namespace Visualisation {

class VISUALISATION_EXPORT QVizObjectiveRankNodeFactory
        : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    QVizObjectiveRankNodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("Visualisation::QVizObjectiveRankNode"); }
    bool isSelected();
    void unSelect();

private:
    QToolButton* m_toolBoxButton;
};

} // namespace Visualisation

#endif // QVIZOBJECTIVERANKNODEFACTORY_H
