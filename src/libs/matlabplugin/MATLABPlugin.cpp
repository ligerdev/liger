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
#include <matlabplugin/MATLABPlugin.h>
#include <matlabplugin/Utils/MatlabPool.h>
#include <matlabplugin/Representation/Functions/Matlab/MatlabFunction.h>

namespace Tigon {
  
MATLABPlugin::MATLABPlugin()
{

}

MATLABPlugin::~MATLABPlugin()
{
    shutdown();
}

TString MATLABPlugin::name()
{
    return "MATLAB specific test functions";
}

bool MATLABPlugin::initialise()
{
    TigonFunctionFactory* fact = TigonFunctionFactory::instance();
    fact->registerFunctionFactory("Tigon::Representation::MatlabFunction"
                                  , new MatlabFunctionFactory);

    return true;
}

bool MATLABPlugin::shutdown()
{
    /// Remove the factory from factory pool
    TigonFunctionFactory* fact = TigonFunctionFactory::instance();
    IFunctionFactory* funcFact = fact->unregisterFactory("Tigon::Representation::MatlabFunction");
    delete funcFact;

    ///\note This is a fixed applied to prevent Matlab engine from crashing
    /// on windows systems (win 7, 10). Empty the engine pool before delete the
    /// MatlabPool object.
#ifdef _WIN32
#ifdef MATLAB_API
    if(MatlabPool::useMatlab()) {
        MatlabPool::getInstance().emptyPool();
    }
#endif
#endif
    return true;
}

}
