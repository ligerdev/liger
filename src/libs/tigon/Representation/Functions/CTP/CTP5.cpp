/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Functions/CTP/CTP5.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/CTP/CTPProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(CTP5)

namespace Tigon {
namespace Representation {

CTP5::CTP5()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(3);
    TString name("CTP5");
    TString description("CTP5 benchmark problem. A constrained problem where.\n"
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
                        "There is 15 disconnected Pareto-optimal solutions"
                        " across the straight line.\n");

    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CTP5::CTP5(const CTP5& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CTP5::~CTP5()
{

}

void CTP5::evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr>& outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = CTP::CTP5(x);

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(),
                         IncorrectProblemFormatException);
    }
}

void CTP5::defineInputPrpts()
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

void CTP5::defineOutputPrpts()
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
