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
#include <tigonconstants.h>
#include <tigon/Representation/Functions/SurrogateModelling/FannNetwork.h>

namespace Tigon {
namespace Representation {

FannNetwork::FannNetwork()
{
    m_nInputs = 0;
    m_nOutputs = 0;
    m_hiddenLayerNeurons = 0;
    m_hiddenLayers = 0;
    m_hiddenFunc = FANN::activation_function_enum::SIGMOID;
    m_outputFunc = FANN::activation_function_enum::LINEAR;

    m_maxEpochs = 100000;
    m_scaling = false;
    m_display = true;
}

FannNetwork::FannNetwork(const TString &networkPath)
{
    m_nInputs = 0;
    m_nOutputs = 0;
    m_hiddenLayerNeurons = 0;
    m_hiddenLayers = 0;
    m_hiddenFunc = FANN::activation_function_enum::SIGMOID;
    m_outputFunc = FANN::activation_function_enum::LINEAR;

    m_maxEpochs = 100000;
    m_scaling = false;
    m_display = true;

    defineFromFile(networkPath);
}

FannNetwork::~FannNetwork()
{
    clearNetwork();
}

void FannNetwork::defineNumInputs(int nInputs)
{
    clearNetwork();
    m_nInputs = nInputs;
}

int FannNetwork::numInputs() const
{
    return m_nInputs;
}

void FannNetwork::defineNumOutputs(int nOutputs)
{
    clearNetwork();
    m_nOutputs = nOutputs;
}

int FannNetwork::numOutputs() const
{
    return m_nOutputs;
}

void FannNetwork::defineOutputFunction(FannNetwork::ActivationFunction func)
{
    m_outputFunc = static_cast<FANN::activation_function_enum>(func);
}

FannNetwork::ActivationFunction FannNetwork::outputFunction()
{
    return static_cast<ActivationFunction>(m_outputFunc);
}

void FannNetwork::defineNumHiddenLayers(int layers)
{
    clearNetwork();
    m_hiddenLayers = layers;
}

int FannNetwork::hiddenLayers() const
{
    return m_hiddenLayers;
}

void FannNetwork::defineSizeHiddenLayers(int neurons) {
    clearNetwork();
    m_hiddenLayerNeurons = neurons;
}

int FannNetwork::sizeHiddenLayers() const
{
    return m_hiddenLayerNeurons;
}

void FannNetwork::defineHiddenFunction(FannNetwork::ActivationFunction func)
{
    m_hiddenFunc = static_cast<FANN::activation_function_enum>(func);
}

FannNetwork::ActivationFunction FannNetwork::hiddenFunction()
{
    return static_cast<ActivationFunction>(m_hiddenFunc);
}

void FannNetwork::defineFromFile(const TString &path)
{
    if(m_network.create_from_file(path)) {
        m_nInputs = m_network.get_num_input();
        m_nOutputs = m_network.get_num_output();
        // -2 for input and output layres
        m_hiddenLayers = m_network.get_num_layers() - 2;
        m_hiddenLayerNeurons = std::floor(((double)m_network.get_total_neurons()
                              -(m_hiddenLayers + 1 + m_nInputs + m_nOutputs)) /
                                          (double)m_hiddenLayers);
        // First neuron in first hidden layer
        m_hiddenFunc = m_network.get_activation_function(1,0);
        m_outputFunc = m_network.get_activation_function(m_hiddenLayers+1,0);
        m_isTrained = true;
    } else {
        m_nInputs = 0;
        m_nOutputs = 0;
        m_hiddenLayerNeurons = 0;
        m_hiddenLayers = 0;
        m_hiddenFunc = FANN::activation_function_enum::SIGMOID;
        m_outputFunc = FANN::activation_function_enum::LINEAR;
        m_isTrained = false;
    }
}

void FannNetwork::saveNetwork(const TString &path)
{
    if(m_isTrained) {
        m_network.save(path);
    }
}

void FannNetwork::scaleNetwork(const TVector<TVector<double>> &input,
                               const TVector<TVector<double>> &output)
{
    int numSamples = input.size();
    if(output.size() != numSamples) {
        //ERROR
        // TODO ERROR qDebug() << "Error: Input and output samples numbers are not equal";
        return;
    }

    FANN::training_data data;

    // Copy the data from the input vectors to c-style arrays for FANN
    fann_type** inputData = (fann_type**)calloc(numSamples, sizeof(fann_type*));
    fann_type** outputData = (fann_type**)calloc(numSamples, sizeof(fann_type*));

    for(int i = 0; i < numSamples; i++) {
        if(input[i].size() == m_nInputs && output[i].size() == m_nOutputs) {
            // Allocate the second dimension of the array
            inputData[i] = (fann_type*)calloc(m_nInputs, sizeof(fann_type));
            outputData[i] = (fann_type*)calloc(m_nOutputs, sizeof(fann_type));

            for(int in = 0; in < m_nInputs; in++) {
                inputData[i][in] = (fann_type)input[i][in];
            }

            for(int out = 0; out < m_nOutputs; out++) {
                outputData[i][out] = (fann_type)output[i][out];
            }
        } else {
            // TODO ERROR qDebug() << "Input or output data dimensions do not match the network.";
        }
    }

    data.set_train_data(numSamples, m_nInputs, inputData, m_nOutputs, outputData);

    //free the training data
    for(int i = 0; i < numSamples; i++) {
        free(inputData[i]);
        free(outputData[i]);
    }
    free(inputData);
    free(outputData);

    if(m_scaling) {
        m_network.set_scaling_params(data, 0, 1, 0, 1);
    }
}

TVector<double> FannNetwork::inputUpperBounds()
{
    return m_inputUpperBound;
}

void FannNetwork::defineInputUpperBounds(const TVector<double> &bounds)
{
    if(bounds.size() == 0) {
        m_scaling = false;
    }else if(bounds.size() == m_nInputs) {
        m_scaling = true;
        m_inputUpperBound = bounds;
    } else {
        // TODO ERROR qDebug() << "Dimensions of \"bounds\" does not match the network input dimension";
    }
}
TVector<double> FannNetwork::inputLowerBounds()
{
    return m_inputLowerBound;
}

void FannNetwork::defineInputLowerBounds(const TVector<double> &bounds)
{
    if(bounds.size() == 0) {
        m_scaling = false;
    }else if(bounds.size() == m_nInputs) {
        m_scaling = true;
        m_inputLowerBound = bounds;
    } else {
        // TODO ERROR qDebug() << "Dimensions of \"bounds\" does not match the network input dimension";
    }
}

TVector<double> FannNetwork::outputUpperBounds()
{
    return m_outputUpperBound;
}

void FannNetwork::defineOutputUpperBounds(const TVector<double> &bounds)
{
    if(bounds.size() == 0) {
        m_scaling = false;
    }else if(bounds.size() == m_nOutputs) {
        m_scaling = true;
        m_outputUpperBound = bounds;
    } else {
        // TODO ERROR qDebug() << "Dimensions of \"bounds\" does not match the network output dimension";
    }
}
TVector<double> FannNetwork::outputLowerBounds()
{
    return m_outputLowerBound;
}

void FannNetwork::defineOutputLowerBounds(const TVector<double>& bounds)
{
    if(bounds.size() == 0) {
        m_scaling = false;
    }else if(bounds.size() == m_nOutputs) {
        m_scaling = true;
        m_outputLowerBound = bounds;
    } else {
        // TODO ERROR qDebug() << "Dimensions of \"bounds\" does not match the network output dimension";
    }
}

void FannNetwork::defineInputBounds(const TVector<double>& lowerBounds,
                                    const TVector<double>& upperBounds)
{
    defineInputLowerBounds(lowerBounds);
    defineInputUpperBounds(upperBounds);
}

void FannNetwork::defineOutputBounds(const TVector<double> &lowerBounds,
                                     const TVector<double> &upperBounds)
{
    defineOutputLowerBounds(lowerBounds);
    defineOutputUpperBounds(upperBounds);
}

void FannNetwork::defineMaxEpochs(int epochs)
{
    m_maxEpochs = epochs;
}

int FannNetwork::maxEpochs()
{
    return m_maxEpochs;
}

void FannNetwork::defineScaling(bool scale)
{
    m_scaling = scale;
}

bool FannNetwork::scaling()
{
    return m_scaling;
}

void FannNetwork::defineDisplay(bool disp)
{
    m_display = disp;
}

bool FannNetwork::display()
{
    return m_display;
}

double FannNetwork::trainingError()
{
    if(m_isTrained) {
        return (double)m_network.get_MSE();
    } else {
        return Tigon::Highest;
    }
}

void FannNetwork::defineBoundsFromData(const TVector<TVector<double>> &input,
                                       const TVector<TVector<double>> &output)
{
    int numSamples = input.size();
    if(output.size() != numSamples) {
        // TODO ERROR qDebug() << "Error in sample sizes for rescale.";
        return;
    }

    m_scaling = true;

    fill(m_inputLowerBound, Tigon::Highest, numInputs());
    fill(m_inputUpperBound, Tigon::Lowest, numInputs());
    fill(m_outputLowerBound, Tigon::Highest, numOutputs());
    fill(m_outputUpperBound, Tigon::Lowest, numOutputs());

    for(int i = 0; i < numSamples; i++) {
        for(int in = 0; in < m_nInputs; in++) {
            if(m_inputLowerBound[in] > input[i][in]) {
                m_inputLowerBound[in] = input[i][in];
            }
            if(m_inputUpperBound[in] < input[i][in]) {
                m_inputUpperBound[in] = input[i][in];
            }
        }

        for(int out = 0; out < m_nOutputs; out++) {
            if(m_outputLowerBound[out] > output[i][out]) {
                m_outputLowerBound[out] = output[i][out];
            }
            if(m_outputUpperBound[out] < output[i][out]) {
                m_outputUpperBound[out] = output[i][out];
            }
        }
    }
}

void FannNetwork::clearBounds()
{
    m_scaling = false;
    // Fill up the bounds arrays just in case.
    fill(m_inputLowerBound, Tigon::Highest, numInputs());
    fill(m_inputUpperBound, Tigon::Lowest, numInputs());
    fill(m_outputLowerBound, Tigon::Highest, numOutputs());
    fill(m_outputUpperBound, Tigon::Lowest, numOutputs());
}

void FannNetwork::trainSurrogate(const TVector<TVector<double>>& input,
                                 const TVector<TVector<double>>& output,
                                 bool rescale)
{
    // Destroy the original network if one exists
    m_network.destroy();
    m_isTrained = false;

    // Check the data
    int numSamples = input.size();
    if(output.size() != numSamples) {
        //ERROR
        // TODO ERROR qDebug() << "Error: Input and output samples numbers are not equal";
        return;
    }

    if(m_scaling) {
        if(rescale || m_inputLowerBound.empty() || m_inputUpperBound.empty() ||
                m_outputLowerBound.empty() || m_outputUpperBound.empty()) {
           defineBoundsFromData(input,output);
        }
    }

    FANN::training_data data;

    // Copy the data from the input vectors to c-style arrays for FANN
    fann_type** inputData = (fann_type**)calloc(numSamples, sizeof(fann_type*));
    fann_type** outputData = (fann_type**)calloc(numSamples, sizeof(fann_type*));

    for(int i = 0; i < numSamples; i++) {
        if(input[i].size() == m_nInputs && output[i].size() == m_nOutputs) {
            // Allocate the second dimension of the array
            inputData[i] = (fann_type*)calloc(m_nInputs, sizeof(fann_type));
            outputData[i] = (fann_type*)calloc(m_nOutputs, sizeof(fann_type));

            TVector<double> scaledInput  = input[i];
            TVector<double> scaledOutput = output[i];
            if(m_scaling) {
                scaleInput(scaledInput);
                scaleOutput(scaledOutput);
            }

            for(int in = 0; in < m_nInputs; in++) {
                inputData[i][in] = (fann_type)scaledInput[in];
            }

            for(int out = 0; out < m_nOutputs; out++) {
                outputData[i][out] = (fann_type)scaledOutput[out];
            }
        } else {
            // TODO ERROR qDebug() << "Input or output data dimensions do not match the network.";
        }
    }

    data.set_train_data(numSamples, m_nInputs, inputData, m_nOutputs, outputData);

    //free the training data
    for(int i = 0; i < numSamples; i++) {
        free(inputData[i]);
        free(outputData[i]);
    }
    free(inputData);
    free(outputData);

    data.save_train(TString("trainingData.train"));

    constructNetwork();

    // Train the network
    int eReports = 0;
    if (m_display) eReports = 1000;
    m_network.train_on_data(data, m_maxEpochs, eReports, 0.0001f);

    m_isTrained = true;

    //Clean up the data
    data.destroy_train();
}

void FannNetwork::saveFANNData(const TVector<TVector<double>> &input,
                               const TVector<TVector<double>> &output,
                               TString file)
{
    int numSamples = input.size();
    if(output.size() != numSamples) {
        //ERROR
        // TODO ERROR qDebug() << "Error: Input and output samples numbers are not equal";
        return;
    }

    FANN::training_data data;

    // Copy the data from the input vectors to c-style arrays for FANN
    fann_type** inputData = (fann_type**)calloc(numSamples, sizeof(fann_type*));
    fann_type** outputData = (fann_type**)calloc(numSamples, sizeof(fann_type*));

    for(int i = 0; i < numSamples; i++) {
        if(input[i].size() == m_nInputs && output[i].size() == m_nOutputs) {
            // Allocate the second dimension of the array
            inputData[i] = (fann_type*)calloc(m_nInputs, sizeof(fann_type));
            outputData[i] = (fann_type*)calloc(m_nOutputs, sizeof(fann_type));

            for(int in = 0; in < m_nInputs; in++) {
                inputData[i][in] = (fann_type)input[i][in];
            }

            for(int out = 0; out < m_nOutputs; out++) {
                outputData[i][out] = (fann_type)output[i][out];
            }
        } else {
            // TODO ERROR qDebug() << "Input or output data dimensions do not match the network.";
        }
    }

    data.set_train_data(numSamples, m_nInputs, inputData, m_nOutputs, outputData);

    //free the training data
    for(int i = 0; i < numSamples; i++) {
        free(inputData[i]);
        free(outputData[i]);
    }
    free(inputData);
    free(outputData);

    constructNetwork();

    data.save_train(file);
    data.destroy_train();
}

TVector<double> FannNetwork::evaluate(const TVector<double>& input)
{
    TVector<double> output(m_nOutputs);
    if(m_isTrained && input.size() == m_nInputs) {

        TVector<double> scaledInput = input;
        if(m_scaling) {
            scaleInput(scaledInput);
        }

        fann_type* in = (fann_type*)calloc(m_nInputs, sizeof(fann_type));
        for (int i = 0; i < scaledInput.size(); i++) {
            in[i] = (fann_type)scaledInput[i];
        }

        fann_type* out;
        out = m_network.run(in);

        for(int i = 0; i < m_nOutputs; i++) {
            output[i] = (double)out[i];
        }

        if(m_scaling) {
            descaleOutput(output);
        }

        free(in);
    } else {
        // TODO ERROR qDebug() << "Incorrect input or untrained network";
    }
    return output;
}

TVector<TVector<double>> FannNetwork::evaluate(
        const TVector<TVector<double>>& input)
{
    TVector<TVector<double>> output(input.size());
    if(m_isTrained) {
        for(int i=0; i < input.size(); i++) {
            output[i] = evaluate(input[i]);
        }
    }
    return output;
}

float FannNetwork::lastError()
{
    return m_network.get_MSE();
}

void FannNetwork::clearNetwork()
{
    m_network.destroy();
    m_isTrained = false;
}

void FannNetwork::constructNetwork()
{
    clearNetwork();

    //Create the network layers array
    int numLayers = 2+m_hiddenLayers;
    unsigned int* layers = (unsigned int*)calloc(numLayers, sizeof(unsigned int));

    layers[0] = m_nInputs;
    for(int i = 1; i < numLayers - 1; i++) {
        layers[i] = m_hiddenLayerNeurons;
    }
    layers[numLayers-1] = m_nOutputs;

    m_network.create_standard_array(numLayers, layers);
    free(layers);

    m_network.set_activation_function_hidden(m_hiddenFunc);
    m_network.set_activation_function_output(m_outputFunc);

    m_network.set_training_algorithm(FANN::training_algorithm_enum::TRAIN_RPROP);
    m_network.set_train_error_function(FANN::error_function_enum::ERRORFUNC_LINEAR);
}

void FannNetwork::scaleInput(TVector<double> &in)
{
    if(!m_inputLowerBound.empty() && !m_inputUpperBound.empty() &&
            m_inputLowerBound.size() == numInputs() &&
            m_inputUpperBound.size() == numInputs()) {
        for(int i = 0; i < m_nInputs; i++) {
            in[i] = (in[i] - m_inputLowerBound[i]) /
                    (m_inputUpperBound[i] - m_inputLowerBound[i]);
        }
    }
}

void FannNetwork::scaleOutput(TVector<double> &out)
{
    if(!m_outputLowerBound.empty() && !m_outputUpperBound.empty() &&
            m_outputLowerBound.size() == numOutputs() &&
            m_outputUpperBound.size() == numOutputs()) {
        for(int i = 0; i < m_nOutputs; i++) {
            out[i] = (out[i] - m_outputLowerBound[i]) /
                    (m_outputUpperBound[i] - m_outputLowerBound[i]);
        }
    }
}

void FannNetwork::descaleOutput(TVector<double> &out)
{
    if(!m_outputLowerBound.empty() && !m_outputUpperBound.empty() &&
            m_outputLowerBound.size() == numOutputs() &&
            m_outputUpperBound.size() == numOutputs()) {
        for(int i = 0; i < m_nOutputs; i++) {
            out[i] = (out[i] * (m_outputUpperBound[i] - m_inputLowerBound[i]) +
                      m_inputLowerBound[i]);
        }
    }
}

} // namespace Representation
} // namespace Tigon

