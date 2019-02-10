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
#include <tigon/Representation/Functions/Python/PythonFunction.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>
#include <json/json.h>
#include <tigon/Utils/JsonUtils.h>

#include <tigon/tigonengineregistry.h>
REGISTER_IFUNCTION_FACTORY(PythonFunction)

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include <pybind11/include/pybind11/pybind11.h>
#include <pybind11/include/pybind11/eval.h>

namespace py = pybind11;

using namespace Json;

namespace Tigon {
namespace Representation {

py::list IElementVecToPyList(const TVector<IElementSPtr> &vec)
{
    py::list list;
    /// \warning : deep  copy
    for (IElementSPtr elem : vec) {
        list.append(elem->value<double>());
    }
    return list;
}

py::list IElementVecToPyList(const TVector<TVector<IElementSPtr>> &vec)
{
    py::list list;
    /// \warning : deep  copy
    for (const TVector<IElementSPtr> &elem : vec) {
        list.append(IElementVecToPyList(elem));
    }
    return list;
}

void pyListToIElementSptrVec(const py::list & list,
                             const TVector<IElementSPtr> &vec)
{
    py::ssize_t n = py::len(list);
    if(n != vec.size()) {
        return;
    }

    for(py::ssize_t i=0; i<n; i++) {
        double val = py::cast<double>(list[i]);
        vec[i]->defineValue(val);
    }
    return;
}

void pyListToIElementSptrVec(const py::list & list,
                             const TVector<TVector<IElementSPtr>> &vec)
{
    py::ssize_t n = py::len(list);
    if(n != vec.size()) {
        return;
    }

    for(py::ssize_t i=0; i<n; i++) {
        pyListToIElementSptrVec(static_cast<py::list>(list[i]), vec[i]);
    }
    return;
}

PythonFunction::PythonFunction()
{
    defineIsNumInputsModifiable(false);
    defineIsNumoutputsModifiable(false);
    Py_Initialize();
}

PythonFunction::~PythonFunction()
{
    Py_Finalize();
}

void PythonFunction::definePath(const TString &path)
{
    boost::filesystem::path bp(path.c_str());
    if(exists(bp)) {
        IFunction::definePath(path);
        m_directory = bp.parent_path().string();
        m_funcName = bp.stem().string();
        loadProperties(path);
    }
}

void PythonFunction::evaluate(const TVector<IElementSPtr> &inputs,
                              const TVector<IElementSPtr> &outputs)
{
    if(TP_nOutputs() <= 0) {
        throw TException(className(), IncorrectPythonFormulation);
    } else if(inputs.size() != TP_nInputs() || outputs.size() != TP_nOutputs()) {
        throw TException(className(), InputOutputSizeMissMatch);
    } else {

        try{
            ///[] Initialise python env
            py::object main_module = py::module::import("__main__");
            py::object main_namespace = main_module.attr("__dict__");

            ///[] Add python file directory to path
            py::eval<py::eval_statements>("import sys", main_namespace);
            TString cmd = TString("sys.path.append(\'" + m_directory + "\')");
            py::eval<py::eval_statements>(cmd.c_str(), main_namespace);

            ///[] Load python function
            py::object pymodule  = py::module::import(m_funcName.c_str());
            py::object pyfunc    = pymodule.attr(m_funcName.c_str());

            ///[] Convert inputs to python list
            py::object pyInputs = IElementVecToPyList(inputs);

            ///[] Run python funciton
            py::object result = pyfunc(pyInputs);

            ///[] Assign the result to output sptr
            pyListToIElementSptrVec(static_cast<py::list>(result), outputs);
        } catch(py::error_already_set const &){
            PyErr_Print();
        }
    }
}

void PythonFunction::batchEvaluate(const TVector<TVector<IElementSPtr> > &inputs,
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

    try{
        ///[] Initialise python env
        py::object main_module = py::module::import("__main__");
        py::object main_namespace = main_module.attr("__dict__");

        ///[] Add python file directory to path
        py::exec("import sys", main_namespace);
        TString cmd = TString("sys.path.append(\'" + m_directory + "\')");
        py::exec(cmd.c_str(), main_namespace);

        ///[] Load python function
        py::object pymodule  = py::module::import(m_funcName.c_str());
        py::object pyfunc    = pymodule.attr(m_funcName.c_str());

        ///[] Convert inputs to python list
        py::object pyInputs = IElementVecToPyList(inputs);

        ///[] Run python funciton
        py::object result = pyfunc(pyInputs);

        ///[] Assign the result to output sptr
        pyListToIElementSptrVec(static_cast<py::list>(result), outputs);
    } catch(py::error_already_set const &){
        PyErr_Print();
    }

}

TString PythonFunction::pythonFunctionName() const
{
    return m_funcName;
}

TString PythonFunction::pythonFunctionDirectory() const
{
    return m_directory;
}

void PythonFunction::defineInputPrpts()
{

}

void PythonFunction::defineOutputPrpts()
{

}

ElementType PythonFunction::stringToElemTypePython(const TString &type)
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

void PythonFunction::loadProperties(const TString &path)
{
    std::ifstream ifs;
    ifs.open(path, std::ifstream::in);
    if(ifs.is_open()) {
        TString headerContent;
        bool start = false;

        TString line;
        while(std::getline(ifs, line)) {
            replaceAll(line, PYHEADER_LABEL, "");
            trim(line);
            // Identify the start of the header
            if(contains(line, TString("<TIGON_PYTHON_FUNC_HEADER_START>"))) {
                start = true;
                continue;
            }

            // Idenfity the end of the header
            if(contains(line, TString("<TIGON_PYTHON_FUNC_HEADER_END>"))) {
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

        createFunctionProperties(funcPrptsObj["name"].toString(),
                funcPrptsObj["description"].toString(), FunctionType::Python);

        // Define path
        IFunction::definePath(path);

        // Inputs
        JsonObject inputObj = obj["input"].toObject();

        TStringList inputNames = toStringList(inputObj["name"].toArray());
        TStringList inputDescriptions = toStringList(inputObj["description"].toArray());
        TStringList inputUnits = toStringList(inputObj["unit"].toArray());

        TVector<ElementType> inputTypes;
        JsonArray jtypes = inputObj["type"].toArray();
        for(int i=0; i<jtypes.size(); i++) {
            inputTypes.push_back(stringToElemTypePython(jtypes.at(i).toString()));
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
            outputTypes.push_back(stringToElemTypePython(jtypes.at(i).toString()));
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

}
}
