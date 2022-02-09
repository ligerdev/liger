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
#include <tigon/tigonengineregistry.h>
#include <tigon/Core/IPlugin.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Representation {
class LIGER_TIGON_EXPORT ExampleFunctionFromPlugin : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::ExampleFunctionFromPlugin)
public:
    ExampleFunctionFromPlugin();
    ExampleFunctionFromPlugin(const ExampleFunctionFromPlugin& func);
    ~ExampleFunctionFromPlugin() { }

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);
protected:
    void defineInputPrpts();
    void defineOutputPrpts();
};

} // Representation
} // Tigon

FUNCTION_FACTORY(ExampleFunctionFromPlugin)

///=============================================================================
/// Plugin
/// ============================================================================
namespace Tigon {
class ExamplePlugin : public IPlugin
{
    DECLARE_TIGON_PLUGIN_FACTORY(Tigon::ExamplePlugin)
public:
    ExamplePlugin() {}
    ~ExamplePlugin() {}

    TString name() {return "ExamplePlugin";}

    bool initialise()
    {
        TigonFunctionFactory::instance()->registerFunctionFactory(
                    "Tigon::Representation::ExampleFunctionFromPlugin",
                    new ExampleFunctionFromPluginFactory);
        return true;
    }

    bool shutdown()
    {
        IFunctionFactory * fact = TigonFunctionFactory::instance()->unregisterFactory(
                    "Tigon::Representation::ExampleFunctionFromPlugin");
        delete fact;
        return true;
    }
};
}

EXPORT_TIGON_PLUGIN(Tigon::ExamplePlugin)

