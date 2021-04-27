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
#include <tigon/Representation/Functions/MinEx.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(MinEx)

namespace Tigon {
namespace Representation {

MinEx::MinEx()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(2);
    TString name("Min-Ex");
    TString description("Min-Ex test problem. Is a simple 2-objective problem "
                        "with only two real-valued decision variables."
                        "The test problem is taken from Deb's book, page 166:\n"
                        "K. Deb, Multi-Objective Optimization using Evolutionary "
                        "Algorithms, Wiley, ISBN 0-471-87339-X.");

    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

MinEx::MinEx(const MinEx& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

MinEx::~MinEx()
{

}

void MinEx::evaluate(const TVector<IElementSPtr> &inputs,
                     const TVector<IElementSPtr> &outputs)
{
    int n = inputs.size();
    int m = outputs.size();

    if( (n == TP_nInputs()) && (m = TP_nOutputs()) ) {
        IElement x1 = *inputs[0];
        IElement x2 = *inputs[1];

        outputs[0]->defineValue(x1);
        outputs[1]->defineValue((1.0 + x2)/x1);
    }
}

void MinEx::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds;
    TVector<IElement>    upperBounds;

    int n = TP_nInputs();
    for(int i = 0; i < n; i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        varDescriptions.push_back("Real-valued decision variable n." +
                                  std::to_string(i));
    }

    lowerBounds.push_back(IElement(0.1)); lowerBounds.push_back(IElement(0.0));
    upperBounds.push_back(IElement(1.0)); upperBounds.push_back(IElement(5.0));

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void MinEx::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    int m = TP_nOutputs();
    for(int i = 0; i < m; i++) {
        varNames.push_back("Output_Var_" + std::to_string(i));
        varDescriptions.push_back("Output_VarDesc_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        optTypes.push_back(Minimization);
    }

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits,
                           optTypes);
}

} // namespace Representation
} // namespace Tigon
