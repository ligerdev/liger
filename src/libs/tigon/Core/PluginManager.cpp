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
#include <tigon/Core/PluginManager.h>
#include <tigon/Core/IPlugin.h>

#include <boost/filesystem.hpp>
#include <boost/dll/import.hpp> // for import_alias
#include <boost/function.hpp>
#include <boost/make_shared.hpp>

namespace dll = boost::dll;
namespace fs = boost::filesystem;

namespace Tigon {
PluginManager::PluginManager()
{

}

PluginManager *PluginManager::instance()
{
    static PluginManager instance;
    return &instance;
}

PluginManager::~PluginManager()
{
    unloadPlugins();
}

void PluginManager::definePath(const TString &folder)
{
    m_path = folder;
}

TString PluginManager::path() const
{
    return m_path;
}

void PluginManager::loadPlugins()
{
    // load all plugins in the search path
    fs::path sharedLibsPath(m_path);

    if (fs::is_directory(sharedLibsPath)) {

        /// find all dll (windows) or .so (linux) or dylb (mac)
        /// in the search folder.
        fs::directory_iterator endIter;
        for (fs::directory_iterator dirIter(sharedLibsPath);
             dirIter != endIter;
             ++dirIter) {

            if(fs::is_regular_file(dirIter->status())) {
#ifdef WIN32
                if(fs::extension(dirIter->path()) != ".dll") {
                    continue;
                }
#elif __APPLE__
                if(fs::extension(dirIter->path()) != ".dylib") {
                    continue;
                }
#elif __linux__
                if(fs::extension(dirIter->path()) != ".so") {
                    continue;
                }
#endif

                boost::system::error_code ec; // error code
                dll::shared_library lib(dirIter->path(), ec,
                                        dll::load_mode::append_decorations);

                TString libName = fs::basename(dirIter->path());
                if(ec) {
                    m_errorStates[libName] = ec.message();
                    continue;
                }

                if (!lib.has("createPlugin")) {
                    m_errorStates[libName] = TString("No createPlugin symbol");
                    continue;
                }

                // library has symbol, import
                typedef boost::shared_ptr<Tigon::IPlugin> (pluginCreator)();
                boost::function<pluginCreator> creator
                        =  dll::import_alias<pluginCreator>(boost::move(lib),
                                                            "createPlugin");

                m_libRefs.push_back(creator);

                // create the plugins
                boost::shared_ptr<IPlugin> plugin = creator();
                m_plugins.push_back(plugin);

                // register functions/operators
                plugin->initialise();

                m_errorStates[libName] = "";
            }
        }
    }

    return;
}

void PluginManager::unloadPlugins()
{
    // unload all plugins
    for(boost::shared_ptr<IPlugin> plugin : m_plugins) {
        plugin->shutdown();
    }
    m_plugins.clear();
}

int PluginManager::nplugins() const
{
    return static_cast<int>(m_plugins.size());
}

TMap<TString, TString> PluginManager::errorStates() const
{
    return m_errorStates;
}

} // Tigon
