/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Functions/SurrogateModelling/FeedforwardSurrogate.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Representation {

FeedforwardSurrogate::FeedforwardSurrogate()
{
    m_network = NULL;
    TP_defineNInputs(0);
    TP_defineNOutputs(0);
    createFunctionProperties("Feedforward Neural Network",
                             "A Feedforward Neural network surrogate function",
                             FunctionType::Internal);
}

FeedforwardSurrogate::FeedforwardSurrogate(TString networkPath)
{
    m_network = new FannNetwork(networkPath);
    TP_defineNInputs(m_network->numInputs());
    TP_defineNOutputs(m_network->numOutputs());
    createFunctionProperties("Feedforward Neural Network",
                             "A Feedforward Neural network surrogate function",
                             FunctionType::Internal);
}

FeedforwardSurrogate::~FeedforwardSurrogate()
{
    delete m_network;
}

void FeedforwardSurrogate::evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs)
{
    if(m_network != NULL){

        // Convert the inputs
        TVector<double> nnInput = Tigon::IElementVecToRealVec(inputs);

        TVector<double> nnOutput = m_network->evaluate(nnInput); // Run the network.

        for(int o = 0; o < TP_nOutputs(); o++) {
            outputs[o]->defineValue(nnOutput[o]);
        }
    }
}

void FeedforwardSurrogate::defineNetwork(FannNetwork* network)
{
    m_network = network;
    TP_defineNInputs(m_network->numInputs());
    TP_defineNOutputs(m_network->numOutputs());
}

FannNetwork *FeedforwardSurrogate::network()
{
    return m_network;
}

void FeedforwardSurrogate::defineInputPrpts()
{
    if(m_network != NULL) {
        TStringList          varNames;
        TStringList          varDescriptions;
        TVector<ElementType> typeVec;
        TStringList          varUnits;

        for(int i = 0; i < TP_nInputs(); i++) {
            varNames.push_back("Input_" + std::to_string(i));
            varDescriptions.push_back("Input_VarDesc_" + std::to_string(i));
            typeVec.push_back(RealType);
            varUnits.push_back("");
        }

        TVector<IElement> lowerBounds =
                createIElementVector(m_network->inputLowerBounds(), typeVec);
        TVector<IElement> upperBounds =
                createIElementVector(m_network->inputUpperBounds(), typeVec);

        createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                              lowerBounds, upperBounds);
    }
}

void FeedforwardSurrogate::defineOutputPrpts()
{
    if(m_network != NULL) {
        TStringList          varNames;
        TStringList          varDescriptions;
        TVector<ElementType> typeVec;
        TStringList          varUnits;
        TVector<OptimizationType> optTypes;

        for(int i = 0; i < TP_nOutputs(); i++) {
            varNames.push_back("Output_" + std::to_string(i));
            varDescriptions.push_back("Output_VarDesc_" + std::to_string(i));
            typeVec.push_back(RealType);
            varUnits.push_back("");
            optTypes.push_back(Minimization);
        }
        createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
    }
}

} // namespace Representation
} // namespace Tigon


