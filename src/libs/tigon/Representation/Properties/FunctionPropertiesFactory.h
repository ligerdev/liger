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
#ifndef FUNCTIONPROPERTIESFACTORY_H
#define FUNCTIONPROPERTIESFACTORY_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class FunctionProperties;

class LIGER_TIGON_EXPORT FunctionPropertiesFactory
{
public:
    static FunctionPropertiesFactory* instance();
    FunctionProperties createFunctionProperties(TString name, TString desc,
                                                FunctionType type = FunctionType::Other);

private:
    FunctionPropertiesFactory();
    ~FunctionPropertiesFactory();

    static FunctionPropertiesFactory* m_instance;
};

} // namespace Represenation
} // namespace Tigon
#endif // FUNCTIONPROPERTIESFACTORY_H
