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
#include <tigon/Representation/Functions/SingleObjective/CBranin.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/SingleObjective/ConstrainedProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(CBranin)

namespace Tigon {
namespace Representation {

CBranin::CBranin()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(2);
    TString name("Constrained Branin function");
    TString description("A constraint variant of the Branin benchmark problem.\n"
                        "The problem consists of one objective function, two "
                        "design variables, and one inequality constraint.\n"
                        "The constraint is a smooth function that makes a part "
                        "of the search space infeasible and the global "
                        "constrained optimum is located at the constraint "
                        "boundary.\n"
                        "The design variables take values in [0, 1]^2.\n");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CBranin::CBranin(const CBranin& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

CBranin::~CBranin()
{

}

void CBranin::evaluate(const TVector<IElementSPtr> &inputs,
                       const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs())) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = ConstrainedProblems::CBranin(x);

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(), IncorrectProblemFormatException);
    }
}

void CBranin::defineInputPrpts()
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

void CBranin::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes({Minimization, Minimization});

    varNames << "f1" << "c1";
    varDescriptions << "Objective_function" <<
                       "Inequality_constraint";
    typeVec  << RealType << RealType;
    varUnits << "" << "";

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
