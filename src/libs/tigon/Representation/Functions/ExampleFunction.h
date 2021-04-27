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
#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ExampleFunction : public IFunction
{

public:
    ExampleFunction();
    ~ExampleFunction();

    void evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs);

    void redefineInputsType (ElementType             type);
    void redefineInputType  (int idx,    ElementType type);
    void redefineOutputsType(ElementType             type);
    void redefineOutputType (int idx,    ElementType type);

private:
    void defineFunctionProperties();
    void defineInputPrpts();
    void defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // EXAMPLEFUNCTION_H
