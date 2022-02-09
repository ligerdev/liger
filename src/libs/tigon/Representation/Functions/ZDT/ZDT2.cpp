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
#include <tigon/Representation/Functions/ZDT/ZDT2.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/ZDT/ZDTProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(ZDT2)

namespace Tigon {
namespace Representation {

ZDT2::ZDT2()
{
    TP_defineNInputs(30);
    TP_defineNOutputs(2);
    TString name("ZDT2");
    TString description("ZDT2 benchmark problem. A two-objective problem having "
                        "a nonconvex Pareto-optimal set.\n"
                        "All variables lie in the range [0,1]. The Pareto-optimal "
                        "region corresponds to 0 <= x1 <= 1 and xi=0 for i=2,...,n.");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

ZDT2::ZDT2(const ZDT2& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

ZDT2::~ZDT2()
{

}

void ZDT2::evaluate(const TVector<IElementSPtr> &inputs,
                    const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs()) &&
            (TP_nInputs() > TP_nOutputs())) {
        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> oReal = ZDT::ZDT2(iReal);

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    } else {
        throw TException(className(), IncorrectProblemFormatException);
    }
}

void ZDT2::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 1.0));

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        varDescriptions.push_back("Decision variable");
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void ZDT2::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    for(int i = 0; i < TP_nOutputs(); i++) {
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
