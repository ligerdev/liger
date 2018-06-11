/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/tigon_global.h>
#include <boost/shared_ptr.hpp>
#include <boost/dll/alias.hpp>

/// Factory method for the plugin
#define DECLARE_TIGON_PLUGIN_FACTORY(ClassName) \
public: \
    static boost::shared_ptr<ClassName> create() { \
        return boost::shared_ptr<ClassName>(new ClassName()); \
    }

/// Export plugin factory method
#define EXPORT_TIGON_PLUGIN(ClassName) \
BOOST_DLL_ALIAS(ClassName::create, createPlugin)

namespace Tigon {
/// Tigon plugin API
class LIGER_TIGON_EXPORT IPlugin
{
public:
    IPlugin() {}
    virtual ~IPlugin() {}

    virtual TString name() = 0;

    /*!
     * \brief Initialise the plugin right after it gets loaded,
     * including registering functions, operators, or algorithms.
     * \return true: success; false: failure
     */
    virtual bool initialise() = 0;

    /*!
     * \brief Operations needed to perform before the plugin is unloaded.
     * E.g. shown down the MATLAB engine if it is a matlab interface.
     * \return true on success and false on failure
     */
    virtual bool shutdown()   = 0;
};

} // Tigon

#include <tigon/Engine/TigonFunctionFactory.h>
#define REGISTER_FACTORY(className) \
Tigon::TigonFunctionFactory::instance()->registerFunctionFactory( \
                             "Tigon::Representation::"#className, \
                             new className##Factory); \

/// Factory method for operators
