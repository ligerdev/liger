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
#include <tigon/Representation/Functions/DTLZ/DTLZ5_3.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>
#include <tigon/tigonengineregistry.h>
#include <tigon/ExceptionHandling/TException.h>
REGISTER_IFUNCTION_FACTORY(DTLZ5_3)

namespace Tigon {
namespace Representation {

DTLZ5_3::DTLZ5_3()
{
    int I = 3;                // dimensionality of the POF
    int M = 4;                // The mininum number of objectives
    int ncon = M - I + 1;     // The number of constraints
    int nFreeVariables = M-1; // number of free variables that describe the POF
    int k = 1;                // number of decision variables in the g function

    TP_defineNInputs(nFreeVariables + k);
    TP_defineNOutputs(M + ncon);
    TString name("DTLZ5_3");
    TString description(
                "This is a variant of the scalable DTLZ5 benchmark problem.\n"
                "The POF degenerates to I=3 dimensions, such that the objectives "
                "in {f1,...,fM-I+1} are non-conflicting among themselves, and "
                "each objective in {fM-I+2,...,fM} is in conflict with all the "
                "remaining objectives.");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
}

DTLZ5_3::DTLZ5_3(const DTLZ5_3& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
}

DTLZ5_3::~DTLZ5_3()
{
}

void DTLZ5_3::evaluate(const TVector<IElementSPtr> &inputs,
                       const TVector<IElementSPtr> &outputs)
{
    int I = 3;
    int M = (TP_nOutputs() + I - 1)/2;

    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs()) &&
            (TP_nInputs() >= M) && ((TP_nOutputs() + I - 1)%2==0) && (M>3) ) {
        TVector<double> x = IElementVecToRealVec(inputs);
        TVector<double> y = DTLZ::DTLZ5_I(x, I, TP_nOutputs());

        for(size_t i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(y[i]);
        }
    } else {
        throw TException(className(),
                         IncorrectProblemFormatException);
    }
}

void DTLZ5_3::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 1.0));

    int I = 3;
    int M = (TP_nOutputs() + I - 1)/2;

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("x" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        if(i < M-1) {
            varDescriptions.push_back("Direction variable");
        } else {
            varDescriptions.push_back("Distance variable");
        }
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void DTLZ5_3::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    int I = 3;
    int M = (TP_nOutputs() + I - 1)/2;
    int ncon = (TP_nOutputs() - I + 1)/2;

    for(int i = 0; i < M; i++) {
        varNames.push_back("f" + std::to_string(i));
        varDescriptions.push_back("Objective n." + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        optTypes.push_back(Minimization);
    }

    for(int i = 0; i < ncon; i++) {
        varNames.push_back("g" + std::to_string(i));
        varDescriptions.push_back("Constraint n." + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        optTypes.push_back(Minimization);
    }

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits,
                           optTypes);
}

} // namespace Representation
} // namespace Tigon
