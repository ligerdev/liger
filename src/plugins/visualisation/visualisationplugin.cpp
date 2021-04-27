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
#include "visualisationplugin.h"
#include "visualisationconstants.h"
#include <visualisation/qvizmscatterplotnodefactory.h>
#include <visualisation/qvizscatterplotnodefactory.h>
#include <visualisation/qvizparallelcoordinatesnodefactory.h>

#include <coreplugin/icore.h>

#include <QtCore/QtPlugin>

using namespace Visualisation::Internal;

VisualisationPlugin::VisualisationPlugin()
    : m_qvizparallelcoordinatesnodefactory(0)
    , m_qvizScatterPlotNodeFactory(0)
    , m_qvizMScatterPlotNodeFactory(0)
{
    // Create your members
}

VisualisationPlugin::~VisualisationPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members - Remove in reverse order of registration!
    // Note that objects that have been added with: addAutoReleasedObject()
    // do not have to be manually removed.

}

bool VisualisationPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool   
    Q_UNUSED(arguments)
    Q_UNUSED(errorString)
    
    m_qvizparallelcoordinatesnodefactory = new Visualisation::QvizParallelCoordinatesNodeFactory;
    m_qvizScatterPlotNodeFactory = new Visualisation::QVizScatterPlotNodeFactory;
    m_qvizMScatterPlotNodeFactory = new Visualisation::QVizMScatterPlotNodeFactory;

    addAutoReleasedObject(m_qvizparallelcoordinatesnodefactory);
    addAutoReleasedObject(m_qvizScatterPlotNodeFactory);
    addAutoReleasedObject(m_qvizMScatterPlotNodeFactory);

    return true;
}

void VisualisationPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // "In the extensionsInitialized method, a plugin can be sure that all
    //  plugins that depend on it are completely initialized."
}

ExtensionSystem::IPlugin::ShutdownFlag VisualisationPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

Q_EXPORT_PLUGIN2(Visualisation, VisualisationPlugin)

