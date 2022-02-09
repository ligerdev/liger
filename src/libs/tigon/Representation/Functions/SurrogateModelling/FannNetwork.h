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
#ifndef FANNNETWORK_H
#define FANNNETWORK_H
#include <tigon/tigon_global.h>

#define FANN_NO_DLL
#include <fann/doublefann.h>
#include <fann/fann_cpp.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT FannNetwork
{

public:

enum ActivationFunction {
    LINEAR = FANN::activation_function_enum::LINEAR,
    SIGMOID = FANN::activation_function_enum::SIGMOID,
    SIGMOID_SYMETRIC = FANN::activation_function_enum::SIGMOID_SYMMETRIC,
    THRESHOLD = FANN::activation_function_enum::THRESHOLD,
    THRESHOLD_LINEAR = FANN::activation_function_enum::THRESHOLD_SYMMETRIC,
    GAUSSIAN = FANN::activation_function_enum::GAUSSIAN,
    GAUSSIAN_SYMETRIC = FANN::activation_function_enum::GAUSSIAN_SYMMETRIC,
    ELLIOT = FANN::activation_function_enum::ELLIOT,
    ELLIOT_SYMMETRIC = FANN::activation_function_enum::ELLIOT_SYMMETRIC,
    LINEAR_PIECE = FANN::activation_function_enum::LINEAR_PIECE,
    LINEAR_PIECE_SYMMETRIC = FANN::activation_function_enum::LINEAR_PIECE_SYMMETRIC,
    SIN_SYMMETRIC = FANN::activation_function_enum::SIN_SYMMETRIC,
    COS_SYMMETRIC = FANN::activation_function_enum::COS_SYMMETRIC
};

    FannNetwork();
    FannNetwork(const Tigon::TString &networkPath);
    ~FannNetwork();

    void defineNumInputs(int nInputs);
    int numInputs() const;

    void defineNumOutputs(int nOutputs);
    int numOutputs() const;
    void defineOutputFunction(ActivationFunction func);
    ActivationFunction outputFunction();

    void defineNumHiddenLayers(int layers);
    int hiddenLayers() const;
    void defineSizeHiddenLayers(int neurons);
    int sizeHiddenLayers()const;
    void defineHiddenFunction(ActivationFunction func);
    ActivationFunction hiddenFunction();

    void defineFromFile(const TString &path);
    void saveNetwork(const TString &path);

    void scaleNetwork(const TVector<TVector<double>> &input,
                      const TVector<TVector<double>> &output);

    // Network bounds or scaling
    TVector<double> inputUpperBounds();
    void defineInputUpperBounds(const TVector<double> &bounds);
    TVector<double> inputLowerBounds();
    void defineInputLowerBounds(const TVector<double> &bounds);
    TVector<double> outputUpperBounds();
    void defineOutputUpperBounds(const TVector<double> &bounds);
    TVector<double> outputLowerBounds();
    void defineOutputLowerBounds(const TVector<double> &bounds);

    void defineInputBounds(const TVector<double> &lowerBounds,
                           const TVector<double> &upperBounds);
    void defineOutputBounds(const TVector<double> &lowerBounds,
                            const TVector<double> &upperBounds);
    void defineBoundsFromData(const TVector<TVector<double>> &input,
                              const TVector<TVector<double>> &output);
    void clearBounds();

    void defineMaxEpochs(int epochs);
    int maxEpochs();
    void defineScaling(bool scale);
    bool scaling();

    void defineDisplay(bool disp);
    bool display();
    double trainingError();

    void trainSurrogate(const TVector<TVector<double>> &input,
                        const TVector<TVector<double>> &output,
                        bool rescale = true);

    void saveFANNData(const TVector<TVector<double>> &input,
                      const TVector<TVector<double>> &output,
                      TString file);

    // Evaluate a single set of inputs
    TVector<double> evaluate(const TVector<double> &input);
    // Evaluate a number of inputs
    TVector<TVector<double>> evaluate(const TVector<TVector<double>> &input);

    float lastError();

private:
    void clearNetwork();
    void constructNetwork();
    void scaleInput(TVector<double> &in);
    void scaleOutput(TVector<double> &out);
    void descaleOutput(TVector<double> &out);

    FANN::neural_net m_network;

    // Network Architecture
    int m_nInputs;
    int m_nOutputs;
    int m_hiddenLayers;
    int m_hiddenLayerNeurons;
    FANN::activation_function_enum m_hiddenFunc;
    FANN::activation_function_enum m_outputFunc;

    // Network status
    bool m_isTrained;

    int m_maxEpochs;
    bool m_scaling;
    bool m_display;
    TVector<double> m_inputLowerBound;
    TVector<double> m_inputUpperBound;
    TVector<double> m_outputLowerBound;
    TVector<double> m_outputUpperBound;
};

} // namespace Representation
} // namespace Tigon

#endif // FANNNETWORK_H
