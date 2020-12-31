/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Functions/SingleObjective/Alpine2.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Representation {

Alpine2::Alpine2()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(1);
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

Alpine2::Alpine2(const Alpine2& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(1);
    defineParallelisable();
}

Alpine2::~Alpine2()
{

}

TString Alpine2::name()
{
    return("Alpine2");
}

TString Alpine2::description()
{
    return("Alpine2 benchmark problem. Single-objective, continuous, "
           "differentiable, separable, scalable, multimodal.\n"
           "The global minimum is x_i=7.917052, f(x*)=-2.80813^D, (i=1,..D).");
}


void Alpine2::evaluate(const TVector<IElementSPtr> &inputs,
                       const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> iReal = IElementVecToRealVec(inputs);

        double f = -1.0;
        for(int i=0; i<iReal.size(); i++) {
            f *= std::sqrt(iReal[i])*std::sin(iReal[i]);
        }

        outputs[0]->defineValue(f);
    }
}

void Alpine2::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 10.0));

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        varDescriptions.push_back("Input_VarDesc" + std::to_string(i));
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void Alpine2::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    varNames.push_back("Output_Var");
    varDescriptions.push_back("Output_VarDesc");
    typeVec.push_back(RealType);
    varUnits.push_back("");
    optTypes.push_back(Minimization);

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
