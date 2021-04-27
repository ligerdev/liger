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
#pragma once
#include <tigon/tigon_global.h>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

namespace Tigon {
class IPlugin;

typedef boost::shared_ptr<IPlugin> (pluginCreator)();

class LIGER_TIGON_EXPORT PluginManager
{
public:
    static PluginManager* instance();
    ~PluginManager();

    void definePath(const TString &folder);
    TString path() const;
    void loadPlugins();
    void unloadPlugins();
    int nplugins() const;

    TMap<TString, TString> errorStates() const;

private:
    PluginManager();
    TString m_path;

    /*!
     * \brief m_libRefs funtions referecing to the shared library.
     * They will be cleared when class is destroyed and the shared library
     *  will then be unloaded
     */
    TVector<boost::function<pluginCreator>> m_libRefs;
    TVector<boost::shared_ptr<IPlugin>>     m_plugins;
    TMap<TString, TString>                  m_errorStates;
};

} // Tigon
