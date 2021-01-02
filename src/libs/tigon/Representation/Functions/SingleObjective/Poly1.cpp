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
#include <tigon/Representation/Functions/SingleObjective/Poly1.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Representation {

Poly1::Poly1()
{
    TP_defineNInputs(1);
    TP_defineNOutputs(1);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

Poly1::Poly1(const Poly1& func)
    : Poly1()
{

}


Poly1::~Poly1()
{

}

void Poly1::evaluate(const TVector<IElementSPtr> &inputs,
                     const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> iReal = IElementVecToRealVec(inputs);

        double x = iReal[0];
        double f = std::pow(6.0*x - 2.0, 2.0) * std::sin(12.0*x - 4.0);

        outputs[0]->defineValue(f);
    }
}

void Poly1::defineInputPrpts()
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
        varDescriptions.push_back("Input_VarDesc" + std::to_string(i));
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void Poly1::defineOutputPrpts()
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

TString Poly1::name()
{
    return("Poly1");
}

TString Poly1::description()
{
    return("Poly1 benchmark problem. Single-objective.");
}

} // namespace Representation
} // namespace Tigon
