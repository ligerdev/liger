/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Functions/DTLZ/DTLZ1.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(DTLZ1)

namespace Tigon {
namespace Representation {

DTLZ1::DTLZ1()
{
    TP_defineNInputs(3);
    TP_defineNOutputs(2);
    TString name("DTLZ1");
    TString description("DTLZ1 benchmark problem. A scalable multimodal problem.\n"
                        "The true Pareto front correspond to sum(f_i) = 0.5\n"
                        "The first M-1 decision variables determine the direction, "
                        "and the rest the remoteness from the origin, with the optimal "
                        " value at x_i = 0.5");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
}

DTLZ1::DTLZ1(const DTLZ1& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
}

DTLZ1::~DTLZ1()
{

}

void DTLZ1::evaluate(const TVector<IElementSPtr> &inputs,
                     const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs()) &&
            (TP_nInputs() > TP_nOutputs())) {
        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> oReal = DTLZ::DTLZ1(iReal, TP_nOutputs());

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    } else {
        throw TException(className(),
                         IncorrectProblemFormatException);
    }
}

void DTLZ1::defineInputPrpts()
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
        if(i < TP_nOutputs()-1) {
            varDescriptions.push_back("Direction variable");
        } else {
            varDescriptions.push_back("Distance variable");
        }
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void DTLZ1::defineOutputPrpts()
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
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
