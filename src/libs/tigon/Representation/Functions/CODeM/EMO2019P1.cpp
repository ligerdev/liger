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
#include <tigon/Representation/Functions/CODeM/EMO2019P1.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/CODeM/CODeMProblems.h>
#include <tigon/tigonengineregistry.h>
REGISTER_IFUNCTION_FACTORY(EMO2019P1)

#include <cmath>

namespace Tigon {
namespace Representation {

EMO2019P1::EMO2019P1()
{
    TP_defineNInputs(3);
    TP_defineNOutputs(2);
    defineNumDirVars(0);
    TString name("EMO2019P1");
    TString description("EMO2019P1 benchmark problem. A scalable uncertain problem.\n"
               "Composed of a deterministic multimodal problem with a concave "
               "Pareto front (WFG4Tunable), and uncertainty in the perpendicular direction "
               "(away from the objective space origin).\n"
               "Larger uncertainty for objective vectors with similar components.");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
}

EMO2019P1::EMO2019P1(const EMO2019P1& func)
{
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    defineNumDirVars(func.numDirVars());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
}

EMO2019P1::~EMO2019P1()
{

}

void EMO2019P1::defineNumDirVars(int k)
{
    if(k<=0) {
        // Default value
        k = std::round(TP_nInputs() / 1.2);
    }
    if(k>=TP_nInputs()) {
        // Largest value of k
        k = TP_nInputs() - 1;
    }

    // Make sure k % (TP_nOutputs()-1) == 0
    int dff = k % (TP_nOutputs()-1);
    m_nDirVar = k - dff;
}

int EMO2019P1::numDirVars() const
{
    return m_nDirVar;
}

void EMO2019P1::evaluate(const TVector<IElementSPtr> &inputs,
                               const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs()) &&
            (TP_nInputs() > TP_nOutputs())) {
        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> oReal = CODeM::EMO2019P1(iReal, m_nDirVar,
                                                 TP_nOutputs());

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    }
}

void EMO2019P1::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, 0.0));
    TVector<IElement>    upperBounds;

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
        if(i < m_nDirVar) {
            varDescriptions.push_back("Direction variable");
        } else {
            varDescriptions.push_back("Distance variable");
        }
        upperBounds.push_back(IElement(RealType, 2.0*(i+1.0)));
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void EMO2019P1::defineOutputPrpts()
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

