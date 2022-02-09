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
#include <tigon/Representation/Functions/ExampleFunction.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Representation {

ExampleFunction::ExampleFunction()
{
    TP_defineNInputs(1);
    TP_defineNOutputs(1);
    defineParallelisable();
}

ExampleFunction::~ExampleFunction()
{

}

void ExampleFunction::evaluate(const TVector<IElementSPtr> &inputs,
                               const TVector<IElementSPtr> &outputs)
{
    for(int i=0; i<outputs.size(); i++) {
        IElement oVal(RealType, 1.0);
        for(int j=0; j<inputs.size(); j++) {
            oVal.defineValue(oVal + (*(inputs[j]) - 1.0/(i+1.0))
                             *  (*(inputs[j]) - 1.0/(i+1.0)));
        }
        outputs[i]->defineValue(oVal);
    }
}

void ExampleFunction::redefineInputsType(ElementType type)
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TStringList          varUnits;
    TVector<ElementType> typeVec(TP_nInputs(), type);
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(type, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(type, 1.0));

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back(inputPrpts()[i].name());
        varDescriptions.push_back(inputPrpts()[i].description());
        varUnits.push_back(inputPrpts()[i].unit());
    }
    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void ExampleFunction::redefineInputType(int idx, ElementType type)
{
    if(isInRange(idx, TP_nInputs())) {
        inputPrpts()[idx].setType(type);
    } else {
        // ERROR - out of bounds
    }
}

void ExampleFunction::redefineOutputsType(ElementType type)
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TStringList          varUnits;
    TVector<ElementType> typeVec(TP_nOutputs(), type);
    TVector<OptimizationType> optTypes;

    for(int i = 0; i < TP_nOutputs(); i++) {
        varNames.push_back(outputPrpts()[i].name());
        varDescriptions.push_back(outputPrpts()[i].description());
        varUnits.push_back(outputPrpts()[i].unit());
        optTypes.push_back(outputPrpts()[i].optimizationType());
    }
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

void ExampleFunction::redefineOutputType(int idx, ElementType type)
{
    if(isInRange(idx, TP_nOutputs())) {
        outputPrpts()[idx].setType(type);
    } else {
        // ERROR - out of bounds
    }
}

void ExampleFunction::defineFunctionProperties()
{
    TString name("Example Function");
    TString description("A description of the function, its input variables "
               "and its output variables.\n"
               "Number of inputs must be equal or larger than mumber of outputs.");
    createFunctionProperties(name, description, FunctionType::Internal);
}

void ExampleFunction::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TStringList          varUnits;
    TVector<ElementType> typeVec(TP_nInputs(), RealType);
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 1.0));

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        varDescriptions.push_back("Input_VarDesc_" + std::to_string(i));
        varUnits.push_back("cm");
    }
    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void ExampleFunction::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TStringList          varUnits;
    TVector<ElementType> typeVec(TP_nOutputs(), RealType);
    TVector<OptimizationType> optType(TP_nOutputs(), Minimization);

    for(int i = 0; i < TP_nOutputs(); i++) {
        varNames.push_back("Output_Var_" + std::to_string(i));
        varDescriptions.push_back("Output_VarDesc_" + std::to_string(i));
        varUnits.push_back("cm");
    }
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optType);
}

} // namespace Representation
} // namespace Tigon
