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
#include <matlabplugin/Representation/Functions/Matlab/MatlabFunction.h>
#include <matlabplugin/Utils/MatlabPool.h>
#include <matlabplugin/Utils/MatlabEngine.h>

// Tigon
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/JsonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

// Boost
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
using namespace boost::filesystem;

// Json
#include <json/json.h>

namespace Tigon {
namespace Representation {

// Constructor
MatlabFunction::MatlabFunction()
    : m_engineVisible(false)
    , m_workDir("")
    , m_command("")
{
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
}

MatlabFunction::~MatlabFunction()
{
}

void MatlabFunction::definePath(const TString &path)
{
    boost::filesystem::path bp(path.c_str());
    if(exists(bp)) {
        IFunction::definePath(path);
        m_workDir = bp.parent_path().string();
        defineCommand(bp.stem().string());
        defineIOPrpts();
    }
}

void MatlabFunction::defineInputPrpts()
{

}
void MatlabFunction::defineOutputPrpts()
{

}

/// Matlab function template contains two parts:
///     - "The Header". Start with special characters and end with special characters.
///        This should contain all information needed in defineIOPrpts.
///     - "The Boday". The actual implementation part. This part should contain
///       a function whcih has the same name as the M-file and this function should
///       be used to perform the function evaluation.
///     - Supported element types: real, int, ordinal, nominal
///
void MatlabFunction::defineIOPrpts()
{
    std::ifstream ifs;
    ifs.open(path(), std::ifstream::in);
    if(ifs.is_open()) {
        TString headerContent;
        bool start = false;

        TString line;
        while(std::getline(ifs, line)) {
            replaceAll(line, HEADER_LABEL, "");
            trim(line);
            // Identify the start of the header
            if(contains(line, TString("<TIGON_MATLAB_FUNC_HEADER_START>"))) {
                start = true;
                continue;
            }

            // Idenfity the end of the header
            if(contains(line, TString("<TIGON_MATLAB_FUNC_HEADER_END>"))) {
                break;
            }

            if(start) {
                headerContent += line;
            }
        }

        ifs.clear();
        ifs.close();

        /// Parsing json file
        JsonDocument jdoc = JsonDocument::fromJson(headerContent);

        if(jdoc.isNull()) {
            return;
        }

        JsonObject obj = jdoc.object();

        // Function Propertes
        JsonObject funcPrptsObj = obj["functionProperties"].toObject();
        TString tmppath = path();

        createFunctionProperties(funcPrptsObj["name"].toString(),
                funcPrptsObj["description"].toString(), FunctionType::Matlab);

        // Hacking: restoring the path;
        IFunction::definePath(tmppath);

        // Inputs
        JsonObject inputObj = obj["input"].toObject();

        TStringList inputNames = toStringList(inputObj["name"].toArray());
        TStringList inputDescriptions = toStringList(inputObj["description"].toArray());
        TStringList inputUnits = toStringList(inputObj["unit"].toArray());

        TVector<ElementType> inputTypes;
        JsonArray jtypes = inputObj["type"].toArray();
        for(int i=0; i<jtypes.size(); i++) {
            inputTypes.push_back(stringToElemTypeMATLAB(jtypes.at(i).toString()));
        }

        TVector<IElement> lowerBounds;
        TVector<IElement> upperBounds;
        JsonArray lbs = inputObj["lbounds"].toArray();
        JsonArray ubs = inputObj["ubounds"].toArray();

        std::transform(lbs.begin(), lbs.end(), inputTypes.begin(),
                       std::back_inserter(lowerBounds),
                       [](const JsonValue& val, ElementType type) {
            if(val.isDouble()) {
                return IElement(type, val.toDouble());
            } else if(val.isString()) {
                return IElement(type, toDouble(val.toString()));
            } else {
                return IElement();
            }
        });

        std::transform(ubs.begin(), ubs.end(), inputTypes.begin(),
                       std::back_inserter(upperBounds),
                       [](const JsonValue& val, ElementType type) {
            if(val.isDouble()) {
                return IElement(type, val.toDouble());
            } else if(val.isString()) {
                return IElement(type, toDouble(val.toString()));
            } else {
                return IElement();
            }
        });

        // Outputs
        JsonObject outputObj = obj["output"].toObject();
        TStringList outputNames = toStringList(outputObj["name"].toArray());
        TStringList outputDescriptions = toStringList(outputObj["description"].toArray());
        TStringList outputUnits = toStringList(outputObj["unit"].toArray());

        TVector<ElementType> outputTypes;
        jtypes = outputObj["type"].toArray();
        for(int i=0; i<jtypes.size(); i++) {
            outputTypes.push_back(stringToElemTypeMATLAB(jtypes.at(i).toString()));
        }

        // Create input and output properties
        TP_defineNInputs(inputNames.size());
        TP_defineNOutputs(outputNames.size());
        createInputProperties(inputNames, inputDescriptions, inputTypes,
                              inputUnits, lowerBounds, upperBounds);

        TVector<OptimizationType> optTypes(outputNames.size(), Minimization);
        createOutputProperties(outputNames, outputDescriptions, outputTypes,
                               outputUnits, optTypes);
    }
}

ElementType MatlabFunction::stringToElemTypeMATLAB(const TString &type)
{
    if(type == "real") {
        return ElementType::RealType;
    } else if(type == "int") {
        return ElementType::IntegerType;
    } else if(type == "ordinal") {
        return ElementType::OrdinalType;
    } else if(type == "nominal") {
        return ElementType::NominalType;
    } else {
        // Error unsupported type or 'type' field not found
        // Exit without doing anything
        throw TException(className(), UnrecognisedTypeException);
    }
}

// Command accessor function.
TString MatlabFunction::command() const
{
    return m_command;
}

// Command accessor function. Runs the IO definition when the command is changed.
void MatlabFunction::defineCommand(const TString &command)
{
    if(command != m_command) {
        m_command = command;
    }
}

// Evaluate the Matlab function with the required inputs and outputs.
// Evaluate doesn't close the engine. Engine is closed when
// MatlabFunction is destroyed or the command is changed.
void MatlabFunction::evaluate(const TVector<IElementSPtr> &inputs,
                              const TVector<IElementSPtr> &outputs)
{
    if(TP_nOutputs() <= 0) {
        throw TException(className(),
                         IncorrectMATLABFormulation);
    } else if(inputs.size() != TP_nInputs() || outputs.size() != TP_nOutputs()) {
        throw TException(className(), InputOutputSizeMissMatch);
    } else {

        MatlabEngine* eng = MatlabPool::getInstance().aquireEngine();

        if(!m_workDir.empty()){
            eng->evaluateString("cd(\'" + m_workDir + "\');", 0);
        }

        if(interactive() & !eng->Interactive()) {
            eng->setInteractive(interactive());
        }

        TString inputString;
        for(int i = 0; i < inputs.size(); i++) {
            TString inputName = "in(" + std::to_string(i+1) + ")";
            if(i == 0) {
                inputString += inputName;
            } else {
                inputString += (", "+ inputName);
            }
        }
        TVector<double> inVec;
        inVec.reserve(inputs.size());
        std::transform(inputs.begin(), inputs.end(), std::back_inserter(inVec),
                       [](const IElementSPtr& sptr) { return sptr->value<double>();});
        eng->placeVectorRow("in", inVec);

        TString outputString;
        for(int o = 0; o < outputs.size(); o++) {
            TString outputName = "out(" + std::to_string(o+1) + ")";
            if(o == 0) {
                outputString += outputName;
            } else {
                outputString += (", "+ outputName);
            }
        }

        // Runs the matlab command: [out(1), out(1)...] = command(in(1), in(2), in(3)...);
        eng->evaluateString("[" + outputString + "] = " + command() + "(" + inputString + ");");
        TVector<double> outVec;
        eng->getWorkspaceVariable("out", outVec);
        if(outVec.size() != 0){
            for(int o = 0; o < outputs.size(); o++) {
                outputs[o]->defineValue(IElement(outVec[o]));
            }
        } else {
            throw TException(className(), EvaluationFailed);
        }

        MatlabPool::getInstance().releaseEngine(eng);
    }
}

void MatlabFunction::batchEvaluate(const TVector<TVector<IElementSPtr> > &inputs,
                                   const TVector<TVector<IElementSPtr> > &outputs)
{
    // Check
    if(TP_nOutputs() <= 0) {
        throw TException(className(), IncorrectMATLABFormulation);
    }

    if(inputs.size() == 0 || outputs.size() ==0) {
        throw TException(className(), InputOutputSizeMissMatch);
    }

    if(inputs[0].size() != TP_nInputs() || outputs[0].size() != TP_nOutputs()) {
        throw TException(className(), InputOutputSizeMissMatch);
    }

    MatlabEngine* eng = MatlabPool::getInstance().aquireEngine();

    TVector<TVector<double> > X;
    TVector<TVector<double> > Y;
    X.reserve(inputs.size());
    Y.reserve(outputs.size());

    std::transform(inputs.begin(), inputs.end(), std::back_inserter(X),
                   [](const TVector<IElementSPtr>& vec) {return IElementVecToRealVec(vec); });

    eng->evaluateString("cd('" + m_workDir + "');", 0);
    eng->placeMatrix("X", X);
    eng->evaluateString("Y = " + command() + "(X)");
    eng->getWorkspaceVariable("Y", Y);

    if(Y.size() != 0){
        int row = 0;
        BOOST_FOREACH(const TVector<double>& vec, Y) {
            int col = 0;
            BOOST_FOREACH(double val, vec) {
                outputs[row][col]->defineValue(val);
                ++col;
            }
            ++row;
        }
    } else {
        throw TException(className(), EvaluationFailed);
    }
}

bool MatlabFunction::interactive() const
{
    return m_engineVisible;
}

// Set the matlab engine to visible
void MatlabFunction::defineInteractive(bool visible)
{
    m_engineVisible = visible;
}

} // namespace Representation
} // namespace Tigon
