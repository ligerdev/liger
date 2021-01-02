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
#include <tigon/Representation/Functions/SurrogateModelling/KrigingSurrogate.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>



namespace Tigon {
namespace Representation {

KrigingSurrogate::KrigingSurrogate()
{
    TP_defineNInputs(1);
    TP_defineNOutputs(2);
    defineParallelisable(false);
}

KrigingSurrogate::KrigingSurrogate(KrigingSPtr model)
{
    TP_defineNInputs(1);
    TP_defineNOutputs(2);
    defineModel(model);
    defineParallelisable(false);
}


KrigingSurrogate::~KrigingSurrogate()
{

}

TString KrigingSurrogate::name()
{
    return("Kriging Surrogate");
}

TString KrigingSurrogate::description()
{
    return("A function that evaluates a solution by a Kriging model.\n"
           "The Kriging model is provided after the function is constructed, "
           "together with the properties of the input and output variables.\n"
           "Number of inputs and outputs match the Kriging model.");
}

void KrigingSurrogate::evaluate(const TVector<IElementSPtr> &inputs,
                               const TVector<IElementSPtr> &outputs)
{
    if(m_model) {
        TVector<double> iReal = IElementVecToRealVec(inputs);

        outputs[0]->defineValue(m_model->interpolate(iReal));
        outputs[1]->defineValue(m_model->errorEstimate());
    }
}

void KrigingSurrogate::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(0); // -inf
    TVector<IElement>    upperBounds(0); // inf

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        varDescriptions.push_back("Input_VarDesc_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
    }
    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void KrigingSurrogate::defineOutputPrpts()
{
    if(TP_nOutputs() != 2) {
        return;
    }
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    varNames << "Expected_Value" << "Expected_Error";
    varDescriptions << "The estimated value by the Kriging estimator"  <<
                       "The expected prediction error";
    typeVec << RealType << RealType;
    varUnits << "" << "";
    optTypes << Minimization << Minimization;
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

KrigingSPtr KrigingSurrogate::model() const
{
    return m_model;
}

void KrigingSurrogate::defineModel(KrigingSPtr model)
{
    m_model = model;
    TP_defineNInputs(m_model->inputData()[0].size());
}


} // namespace Representation
} // namespace Tigon
