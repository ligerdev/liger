/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef QVIZMSCATTERPLOTNODEFACTORY_H
#define QVIZMSCATTERPLOTNODEFACTORY_H
#include <visualisation/visualisation_global.h>
#include <visualisation/qvizmscatterplotnode.h>

#include <designer/iengine/iprocessnodefactory.h>

#include <QToolButton>
#include <QObject>

namespace Designer {
class IProcessNode;
}

namespace Visualisation {

class VISUALISATION_EXPORT QVizMScatterPlotNodeFactory : public Designer::IProcessNodeFactory
{
    Q_OBJECT

public:
    QVizMScatterPlotNodeFactory();

    Designer::IProcessNode* createProcessNode();
    QString className() { return QString("Visualisation::QVizMScatterPlotNode"); }
    bool isSelected();
    void unSelect();

private:
    QToolButton* m_toolBoxButton;
};


} // namespace Visualisation


#endif // QVIZMSCATTERPLOTNODEFACTORY_H
