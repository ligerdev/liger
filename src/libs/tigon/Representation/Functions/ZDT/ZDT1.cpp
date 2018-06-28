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
#include <tigon/Representation/Functions/ZDT/ZDT1.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/ZDT/ZDTProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(ZDT1)

namespace Tigon {
namespace Representation {

ZDT1::ZDT1()
{
    TP_defineNInputs(2);
    TP_defineNOutputs(2);
    TString name("ZDT1");
    TString description("ZDT1 benchmark problem. A two objectives problem that "
                        "are to be minimised.\n"
                        "The problem scalable in terms of the number of "
                        "variables, which is given by n, and all variables lie "
                        "in the range [0,1].\n "
                        "The Pareto front corresponds to 0<=x1<=1 and xi=0 for "
                        "i=2,..,n.\n, and its shape in objective space is "
                        "convex.");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

ZDT1::ZDT1(const ZDT1& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(2);
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
    defineIsNumoutputsModifiable(false);
}

ZDT1::~ZDT1()
{

}

void ZDT1::evaluate(const TVector<IElementSPtr> &inputs,
                     const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == 2) &&
            (TP_nInputs() > 1)) {
        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> oReal = ZDT::ZDT1(iReal);

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    } else {
        throw TException(className(), IncorrectProblemFormatException);
    }
}

void ZDT1::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 1.0));

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("x_" + std::to_string(i+1));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        varDescriptions.push_back("Decision variable n." + std::to_string(i+1));
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void ZDT1::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    for(int i = 0; i < 2; i++) {
        varNames.push_back("f_" + std::to_string(i+1));
        varDescriptions.push_back("Objective function n." + std::to_string(i+1));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        optTypes.push_back(Minimization);
    }
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
