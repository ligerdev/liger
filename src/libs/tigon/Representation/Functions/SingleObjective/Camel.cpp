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
#include <tigon/Representation/Functions/SingleObjective/Camel.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/SingleObjective/ConstrainedProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(Camel)

namespace Tigon {
namespace Representation {

Camel::Camel()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(2);
    TString name("Camel function");
    TString description("Camel benchmark problem. A constrained problem where.\n"
                        " a constraint makes the vicinity of the Pareto-optimal"
                        " front infeasible.\n"
                        "The problem consists of one objective functions, two design"
                        " variables, and a inequality constraint.\n"
                        "The constraint is multimodal and the global optimum "
                        " is located close to the local minimum.\n"
                        "This function is usually evaluated on the square [-2, 2]^2.\n");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

Camel::Camel(const Camel& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

Camel::~Camel()
{

}

void Camel::evaluate(const TVector<IElementSPtr> &inputs,
                     const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = ConstrainedProblems::Camel(x);

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(), IncorrectProblemFormatException);
    }
}

void Camel::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, -2.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 2.0));

    varNames << "x1" << "x2";
    varDescriptions << "Decision_variable_1" << "Decision_variable_2";
    typeVec << RealType << RealType;
    varUnits << "" << "";

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void Camel::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    varNames << "f" << "g";
    varDescriptions << "Objective_function" <<
                       "Inequality_constraint";
    typeVec  << RealType << RealType;
    varUnits << "" << "";
    optTypes << Minimization << Minimization;

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
