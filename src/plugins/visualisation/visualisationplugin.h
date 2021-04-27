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
#ifndef VISUALISATION_H
#define VISUALISATION_H
#include "visualisation_global.h"
#include <extensionsystem/iplugin.h>


namespace Visualisation {
class QvizParallelCoordinatesNodeFactory;
class QVizScatterPlotNodeFactory;
class QVizMScatterPlotNodeFactory;

namespace Internal {

class VisualisationPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "net.liger-project.Liger.LigerPlugin" FILE "Visualisation.json")
    
public:
    VisualisationPlugin();
    ~VisualisationPlugin();
    
    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    QvizParallelCoordinatesNodeFactory* m_qvizparallelcoordinatesnodefactory;
    QVizScatterPlotNodeFactory* m_qvizScatterPlotNodeFactory;
    QVizMScatterPlotNodeFactory* m_qvizMScatterPlotNodeFactory;
};

} // namespace Internal
} // namespace Visualisation

#endif // VISUALISATION_H

