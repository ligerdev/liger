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
#include <tigon/Representation/Functions/CTP/CTP4.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/CTP/CTPProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(CTP4)

namespace Tigon {
namespace Representation {

CTP4::CTP4()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(3);
    TString name("CTP4");
    TString description("CTP4 benchmark problem. A constrained problem where.\n"
                        " a constraint makes the vicinity of the Pareto-optimal"
                        " front infeasible.\n"
                        "The problem consists of two objective functions, two design"
                        " variables, and one inequality constraint.\n"
                        "The constraint only allows the existence of a few feasible"
                        " Pareto-optimal solutions.\n"
                        "The Pareto-optimal solutions lie on a straight line"
                        " given by (f2(x)-e)cos(theta) = f1(x)sin(theta).\n"
                        "For obtaining the Pareto-optimal solutions that lie"
                        " in the line considered that:\n"
                        "1) x1 is a free variable that can be varied"
                        " between 0 and 1; and\n"
                        "2) x2 is posed as a function of x1, that is,"
                        " x2 = x1*sin(theta)/cos(theta)+e-1+x1.\n"
                        "However, note that not all values of x1 are"
                        " Pareto optimal\n."
                        "There is 13 disconnected Pareto-optimal solutions"
                        " across the straight line, which lie inside a very"
                        " narrow tunnel, which makes this the"
                        " hardest problem amongst CTP2-CTP5.\n");

    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CTP4::CTP4(const CTP4& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CTP4::~CTP4()
{

}

void CTP4::evaluate(const TVector<IElementSPtr> &inputs,
                    const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = CTP::CTP4(x);

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(),
                         IncorrectProblemFormatException);
    }
}

void CTP4::defineInputPrpts()
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

void CTP4::defineOutputPrpts()
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
