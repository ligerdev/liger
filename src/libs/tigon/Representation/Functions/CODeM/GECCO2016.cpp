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
#include <tigon/Representation/Functions/CODeM/GECCO2016.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Functions/CODeM/CODeMProblems.h>
#include <tigon/tigonengineregistry.h>
REGISTER_IFUNCTION_FACTORY(GECCO2016)

namespace Tigon {
namespace Representation {

GECCO2016::GECCO2016()
{
    defineParallelisable();
    TP_defineNInputs(3);
    TP_defineNOutputs(2);
    TString name("GECCO2016");
    TString description("GECCO2016 benchmark problem. A scalable uncertain problem.\n"
               "Composed of a deterministic multimodal problem with a planar "
               "Pareto front (DTLZ1), and uncertainty in both radial "
               "and perpendicular directions.\n"
               "Radial uncertainty increase as the deterministic objective "
               "vector is moving away from the true pareto front, and as its "
               "components are more similar.");
    createFunctionProperties(name, description, FunctionType::Internal);
    defineParallelisable();
}

GECCO2016::GECCO2016(const GECCO2016& func)
{
    defineParallelisable();
    TP_defineNInputs(func.TP_nInputs());
    TP_defineNOutputs(func.TP_nOutputs());
    createFunctionProperties(func.name(), func.description(), func.type());
    defineParallelisable();
}

GECCO2016::~GECCO2016()
{

}

void GECCO2016::evaluate(const TVector<IElementSPtr> &inputs,
                               const TVector<IElementSPtr> &outputs)
{
    if((inputs.size() == TP_nInputs()) && (outputs.size() == TP_nOutputs()) &&
            (TP_nInputs() > TP_nOutputs())) {
        TVector<double> iReal = IElementVecToRealVec(inputs);
        TVector<double> oReal = CODeM::GECCOExample(iReal, TP_nOutputs());

        for(int i=0; i<outputs.size(); i++) {
            outputs[i]->defineValue(oReal[i]);
        }
    }
}

void GECCO2016::defineInputPrpts()
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

void GECCO2016::defineOutputPrpts()
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
