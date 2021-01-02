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
#include <tigon/Representation/Functions/CTP/CTP6.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/CTP/CTPProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(CTP6)

namespace Tigon {
namespace Representation {

CTP6::CTP6()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(3);
    TString name("CTP6");
    TString description("CTP6 benchmark problem. A constrained problem where.\n"
                        " a constraint makes many regions of the entire search"
                        " space infeasible. The constraint makes infeasible holes"
                        " of differing widths towards the Pareto-optimal region.\n"
                        "The problem consists of two objective functions, two design"
                        " variables, and one inequality constraint.\n"
                        "The Pareto-optimal region corresponds to all solutions "
                        "satisfying 1<=((f2-e)sin(theta)+f1cos(theta))<=2.\n");

    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CTP6::CTP6(const CTP6& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CTP6::~CTP6()
{

}

void CTP6::evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr>& outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = CTP::CTP6(x);

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(),
                         IncorrectProblemFormatException);
    }
}

void CTP6::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 1.0));

    varNames << "x1" << "x2";
    varDescriptions << "Decision_variable_1" << "Decision_variable_2";
    typeVec << RealType << RealType;
    varUnits << "" << "";

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void CTP6::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes({Minimization, Minimization, Minimization});

    varNames << "f1" << "f2" << "c1";
    varDescriptions << "Objective_function_1" << "Objective_function_2" <<
                       "Inequality_constraint";
    typeVec << RealType << RealType << RealType;
    varUnits << "" << "" << "";

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
