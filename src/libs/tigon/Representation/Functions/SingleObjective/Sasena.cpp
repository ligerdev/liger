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
#include <tigon/Representation/Functions/SingleObjective/Sasena.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/SingleObjective/ConstrainedProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(Sasena)

namespace Tigon {
namespace Representation {

Sasena::Sasena()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(4);
    TString name("Sasena function");
    TString description("Sasena benchmark problem. A constrained problem where.\n"
                        " a constraint makes the vicinity of the Pareto-optimal"
                        " front infeasible.\n"
                        "The problem consists of one objective functions, two design"
                        " variables, and three inequality constraints.\n"
                        "Two constraints are active and one inactive.\n"
                        "This function is usually evaluated on the square [0, 1]^2.\n");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

Sasena::Sasena(const Sasena& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

Sasena::~Sasena()
{

}

void Sasena::evaluate(const TVector<IElementSPtr> &inputs,
                      const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = ConstrainedProblems::Sasena(x);

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(), IncorrectProblemFormatException);
    }
}

void Sasena::defineInputPrpts()
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

void Sasena::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    varNames << "f" << "g1" << "g2" << "g3";
    varDescriptions << "Objective_function" <<
                       "Inequality_constraint_1" <<
                       "Inequality_constraint_2" <<
                       "Inequality_constraint_3";
    typeVec  << RealType << RealType << RealType << RealType;
    varUnits << "" << "" << "" << "";
    optTypes << Minimization << Minimization << Minimization << Minimization;

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
