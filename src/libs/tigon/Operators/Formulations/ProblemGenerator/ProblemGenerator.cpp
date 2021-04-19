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
#include <tigon/Operators/Formulations/ProblemGenerator/ProblemGenerator.h>
#include <tigon/Engine/TigonFunctionFactory.h>
#include <tigon/Engine/TigonEngineConstants.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Properties/ProblemPropertiesFactory.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Container/SampleVectors.h>
#include <tigon/Factories/DistributionFactory.h>
#include <tigon/Utils/JsonUtils.h>

#include <tigon/tigonengineregistry.h>
REGISTER_IPSET_FACTORY(Operators, ProblemGenerator)

#include <tinyxml2/tinyxml2.h>
using namespace tinyxml2;

using namespace Json;

#include <algorithm>
using namespace std;

namespace Tigon {
namespace Operators {

using namespace Tigon::Representation;

ProblemGenerator::ProblemGenerator()
    : m_groupedDataProcessed(false)
{
    initialise();
}

ProblemGenerator::ProblemGenerator(IPSet *ipset)
    : IFormulation(ipset)
    , m_groupedDataProcessed(false)
{
    initialise();
}

TString ProblemGenerator::name()
{
    return TString("Problem Generator");
}

TString ProblemGenerator::description()
{
    return TString("Operator that configures the optimization problem");
}

///\todo reduce the complexity here. This function can be simplified.
IFunctionSPtr ProblemGenerator::createFunction(const TString &funcTypeStr,
                                               const TString &funcPath)
{
    IFunctionSPtr func;
    if(funcTypeStr == FuncTypeStrInternal) {
        func = createFunctionByName(funcPath, funcPath);
    } else if(funcTypeStr == FuncTypeStrSharedLib) {
        /// \todo need to restore loading from sharedlib
        /// \deprecated This is replaced by tigon plugin.
        TException("Tigon::Operators::ProblemGenerator",
                   "Loading from shared lib not implemented yet");
        //func = IFunctionSPtr(loadFunctionFromLib(funcPath));
        //func->definePath(funcPath);
    } else if (funcTypeStr == FuncTypeStrMatlab) {
        func = createFunctionByName("Tigon::Representation::MatlabFunction",
                                    funcPath);
    } else if (funcTypeStr == FuncTypeStrPython) {
        func = createFunctionByName("Tigon::Representation::PythonFunction",
                                    funcPath);
    }
    return func;
}

IFunctionSPtr ProblemGenerator::createFunctionByName(const TString &className,
                                                     const TString &funcPath)
{
    IFunction* funPtr = TigonFunctionFactory::instance()->createFunction(className);
    if(funPtr && (!funcPath.empty())) {
        funPtr->definePath(funcPath);
    }
    IFunctionSPtr funSptr(funPtr);
    return funSptr;
}

/*!
 * \todo update the following info when the class is stable
 */
void ProblemGenerator::initialise()
{
    addProperty("funcTypes", "Function Types", getTType(TString));
    addProperty("funcPathes", "Function Pathes", getTType(TString));
    addProperty("funcPrpts", "Function Properties", getTType(TString));
    addProperty("iprts", "iprts", getTType(TString));
    addProperty("oprts", "oprts", getTType(TString));
    addProperty("lbounds", "lbounds", getTType(TString));
    addProperty("ubounds", "ubounds", getTType(TString));
    addProperty("paramValueVec", "paramValueVec", getTType(TString));
    addProperty("externalParam", "externalParam", getTType(TString));
    addProperty("externalParamGroups", "externalParamGroups", getTType(TString));
    addProperty("externalParamGroupDataPathes", "externalParamGroupDataPathes",
                getTType(TString));
    addProperty("setGoals", "setGoals", getTType(TString));
    addProperty("goals", "goals", getTType(TString));
    addProperty("priorities", "priorities", getTType(TString));
    addProperty("thresholds", "thresholds", getTType(TString));
    addProperty("dVecUncertainties", "dVecUncertaintiesJson", getTType(TString));
    addProperty("funcOutUncertainties", "funcOutUncertaintiesJson", getTType(TString));
    addProperty("f2dMap", "f2dMap", getTType(TString));
    addProperty("f2pMap", "f2pMap", getTType(TString));
    addProperty("f2oMap", "f2oMap", getTType(TString));
    addProperty("f2cMap", "f2cMap", getTType(TString));
    addProperty("f2uMap", "f2uMap", getTType(TString));
}

void ProblemGenerator::evaluateNode()
{
    if(!m_groupedDataProcessed) {
        processExternalParamGroupData();
        m_groupedDataProcessed=true;
    }
    IFormulation::evaluateNode();
}

TString ProblemGenerator::problemDataJson() const
{
    JsonObject jobj = problemToJsonObject(this->problem());
    JsonDocument jdoc;
    jdoc.setObject(jobj);
    return jdoc.toJson();
}

ProblemDefinitionStatus ProblemGenerator::setProblemDataJson(const TString &str)
{
    JsonDocument jdoc = JsonDocument::fromJson(str);
    JsonObject jobj = jdoc.object();

    // Check if all required properties are there
    if(!jobj.contains("funcTypes"))   return Undefined;
    if(!jobj.contains("funcPathes"))  return Undefined;
    if(!jobj.contains("funcPrpts"))   return Undefined;
    if(!jobj.contains("iprts"))       return Undefined;
    if(!jobj.contains("oprts"))       return Undefined;
    if(!jobj.contains("lbs"))         return Undefined;
    if(!jobj.contains("ubs"))         return Undefined;
    if(!jobj.contains("pVector"))     return Undefined;

    if(!jobj.contains("setGoals"))    return Undefined;
    if(!jobj.contains("goals"))       return Undefined;
    if(!jobj.contains("priorities"))  return Undefined;
    if(!jobj.contains("thresholds"))  return Undefined;

    if(!jobj.contains("f2dMap"))      return Undefined;
    if(!jobj.contains("f2pMap"))      return Undefined;
    if(!jobj.contains("f2oMap"))      return Undefined;
    if(!jobj.contains("f2cMap"))      return Undefined;
    if(!jobj.contains("f2uMap"))      return Undefined;

    if(!jobj.contains("dVecUncertainties"))     return Undefined;
    if(!jobj.contains("funcOutUncertainties"))  return Undefined;

    // Extract data
    TStringList funcTypes;
    fromJsonArray(jobj["funcTypes"].toArray(), funcTypes);
    TStringList funcPathes;
    fromJsonArray(jobj["funcPathes"].toArray(), funcPathes);
    JsonArray funcPrpts = jobj["funcPrpts"].toArray();

    if(!(funcPrpts.size() == funcTypes.size()
         && funcTypes.size() == funcPathes.size())) {
        return ProblemDefinitionStatus::Undefined;
    }

    TVector<double> lbs;
    fromJsonArray(jobj["lbs"].toArray(), lbs);
    TVector<double> ubs;
    fromJsonArray(jobj["ubs"].toArray(), ubs);
    if(lbs.size() != ubs.size()) return Undefined;

    TVector<TVector<ElementProperties> > iprts;
    fromJsonArray(jobj["iprts"].toArray(), iprts);
    TVector<TVector<ElementProperties> > oprts;
    fromJsonArray(jobj["oprts"].toArray(), oprts);

    TVector<IElementSPtr> pVector;
    fromJsonArrayFullInfo(jobj["pVector"].toArray(), pVector);

    TVector<bool> setGoals;
    fromJsonArray(jobj["setGoals"].toArray(), setGoals);

    TVector<double> goals;
    fromJsonArray(jobj["goals"].toArray(), goals);

    TVector<int> priorities;
    fromJsonArray(jobj["priorities"].toArray(), priorities);

    TVector<double> thresholds;
    fromJsonArray(jobj["thresholds"].toArray(), thresholds);

    TVector<TVector<int> > f2dMap;
    fromJsonArray(jobj["f2dMap"].toArray(), f2dMap);
    TVector<TVector<int> > f2pMap;
    fromJsonArray(jobj["f2pMap"].toArray(), f2pMap);
    TVector<TVector<int> > f2oMap;
    fromJsonArray(jobj["f2oMap"].toArray(), f2oMap);
    TVector<TVector<int> > f2cMap;
    fromJsonArray(jobj["f2cMap"].toArray(), f2cMap);
    TVector<TVector<int> > f2uMap;
    fromJsonArray(jobj["f2uMap"].toArray(), f2uMap);

    int nFuncs = funcTypes.size();
    ProblemSPtr prob(new Problem);

    for(int i=0; i<nFuncs; i++) {
        /// [0] Create function
        IFunctionSPtr func = createFunction(funcTypes[i], funcPathes[i]);

        /// [1] Process function properties
        if(func) {
            JsonObject prtmap = funcPrpts[i].toObject();
            assignFunctionProperties(func, prtmap);
        } else {
            // Error throw exception: Undefined function
        }

        /// [2] Process function input and output properties
        //qDebug() << "[2] Process function input and output properties" << i;
        func->defineInputPrpts(iprts[i]);
        func->defineOutputPrpts(oprts[i]);

        /// [3] Extract inParamIdx, outConstrIdx, outObjIdx
        TVector<int>::iterator itr;
        // Param
        TVector<int> inParamIdx = f2pMap[i];
        itr = remove_if(inParamIdx.begin(), inParamIdx.end(),
                        [](int itr){return itr==-1;});
        inParamIdx.erase(itr, inParamIdx.end());

        // Con
        TVector<int> outConstrIdx = f2cMap[i];
        itr = remove_if(outConstrIdx.begin(), outConstrIdx.end(),
                        [](int itr){return itr==-1;});
        outConstrIdx.erase(itr, outConstrIdx.end());

        // Obj
        TVector<int> outObjIdx = f2oMap[i];
        itr = remove_if(outObjIdx.begin(), outObjIdx.end(),
                        [](int itr){return itr==-1;});
        outObjIdx.erase(itr, outObjIdx.end());

        // Unu
        TVector<int> outUnuIdx = f2uMap[i];
        itr = remove_if(outUnuIdx.begin(), outUnuIdx.end(),
                        [](int itr){return itr==-1;});
        outUnuIdx.erase(itr, outUnuIdx.end());

        // Append
        prob->appendFunction(func, inParamIdx, outConstrIdx, outObjIdx, outUnuIdx);
    }

    ProblemDefinitionStatus status = prob->processProblemDefinition();
    if(status != FullyDefined) {
        return status;
    }

    /// [4] Boxconstraints
    //qDebug() << "[4] Boxconstraints";
    BoxConstraintsDataSPtr box = prob->boxConstraints();
    TVector<IElement> lowerBounds, upperBounds;
    for(int i=0; i<lbs.size(); i++) {
        lowerBounds.push_back(IElement(lbs[i]));
        upperBounds.push_back(IElement(ubs[i]));
    }
    box->defineLowerBounds(lowerBounds);
    box->defineUpperBounds(upperBounds);

    /// [5] ParamerValue
    //qDebug() << "[5] ParamerValue";
    // This should also involve chaning the properties in functions
    prob->defineParameterVector(pVector);

    /// [6] Goals and thresholds
    //qDebug() << "[6] Goals and thresholds";
    prob->defineSetGoalVector(setGoals);
    prob->defineGoalVector(realVecToElemVec(goals));
    prob->definePriorityVector(priorities);
    prob->defineThresholdVector(realVecToElemVec(thresholds));

    /// [7] Uncertainties
    //qDebug() << "[7] Uncertainties";
    TVector<UncertaintyMapping*> dVecUncertainties;
    fromJsonArray(jobj["dVecUncertainties"].toArray(), dVecUncertainties);
    prob->defineDVecUncertainties(dVecUncertainties);

    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties;
    fromJsonArray(jobj["funcOutUncertainties"].toArray(), funcOutUncertainties);
    prob->defineFuncOutUncertainties(funcOutUncertainties);

    /// [8] External parameters
    TVector<bool> isExtParams;
    fromJsonArray(jobj["isExternalParameters"].toArray(), isExtParams);
    prob->defineExternalParameters(isExtParams);

    /// [8] Process problem
    //qDebug() << "[8] Process problem";
    status = prob->processProblemDefinition();

    if(status == FullyDefined) {
        defineProblem(prob);
    }
    return status;
}

TString ProblemGenerator::f2dMap()
{
    TVector<TVector<int> > map = this->function2VariablesMap();
    return mapToString(map);
}

void ProblemGenerator::definef2dMap(const TString &mapString)
{
    m_f2dMap = stringToMap(mapString);
}

TString ProblemGenerator::f2pMap()
{
    TVector<TVector<int> > map = this->function2ParametersMap();
    return mapToString(map);
}

void ProblemGenerator::definef2pMap(const TString &mapString)
{
    m_f2pMap = stringToMap(mapString);
}

TString ProblemGenerator::f2oMap()
{
    TVector<TVector<int> > map = this->function2ObjectivesMap();
    return mapToString(map);
}

void ProblemGenerator::definef2oMap(const TString &mapString)
{
    m_f2oMap = stringToMap(mapString);
}

TString ProblemGenerator::f2cMap()
{
    TVector<TVector<int> > map = this->function2ConstraintsMap();
    return mapToString(map);
}

void ProblemGenerator::definef2cMap(const TString &mapString)
{
    m_f2cMap = stringToMap(mapString);
}

TString ProblemGenerator::f2uMap()
{
    TVector<TVector<int> > map = this->function2UnusedMap();
    return mapToString(map);
}

void ProblemGenerator::definef2uMap(const TString &mapString)
{
    m_f2uMap = stringToMap(mapString);
}

ProblemDefinitionStatus ProblemGenerator::processFormulation()
{
    ProblemSPtr problem = this->problem();

    if(!(m_funcPrpts.size() == m_funcTypes.size()
         && m_funcTypes.size() == m_funcPathes.size())) {
        throw TException("Tigon::Operators::ProblemGenerator",
                         Tigon::IncorrectProblemFormatException);
    }

    int nFuncs = m_funcTypes.size();
    for(int i=0; i<nFuncs; i++) {
        /// [0] Create function
        IFunctionSPtr func = createFunction(m_funcTypes[i], m_funcPathes[i]);

        /// [1] Process function properties
        if(func) {
            JsonObject prtmap = m_funcPrpts[i].toObject();
            assignFunctionProperties(func, prtmap);
        } else {
            throw TException(this->className(), Tigon::UnDefinedFunction);
        }

        /// [2] Process function input and output properties
        //qDebug() << "[2] Process function input and output properties" << i;
        func->defineInputPrpts(m_iprts[i]);
        func->defineOutputPrpts(m_oprts[i]);

        /// [3] Extract inParamIdx, outConstrIdx, outObjIdx
        TVector<int>::iterator itr;
        auto lambadFunc = [](int itr){return itr==-1;};
        // Param
        TVector<int> inParamIdx = m_f2pMap[i];
        itr = remove_if(inParamIdx.begin(), inParamIdx.end(), lambadFunc);
        inParamIdx.erase(itr, inParamIdx.end());

        // Con
        TVector<int> outConstrIdx = m_f2cMap[i];
        itr = remove_if(outConstrIdx.begin(), outConstrIdx.end(), lambadFunc);
        outConstrIdx.erase(itr, outConstrIdx.end());

        // Obj
        TVector<int> outObjIdx = m_f2oMap[i];
        itr = remove_if(outObjIdx.begin(), outObjIdx.end(), lambadFunc);
        outObjIdx.erase(itr, outObjIdx.end());

        // Unu
        TVector<int> outUnuIdx = m_f2uMap[i];
        itr = remove_if(outUnuIdx.begin(), outUnuIdx.end(), lambadFunc);
        outUnuIdx.erase(itr, outUnuIdx.end());

        problem->appendFunction(func, inParamIdx, outConstrIdx, outObjIdx, outUnuIdx);
    }

    ProblemDefinitionStatus status = problem->processProblemDefinition();
    if(status != FullyDefined) {
        return status;
    }

    /// [4] Boxconstraints
    //qDebug() << "[4] Boxconstraints";
    BoxConstraintsDataSPtr box = problem->boxConstraints();
    box->defineLowerBounds(m_lowerBounds);
    box->defineUpperBounds(m_upperBounds);

    /// [5] ParamerValue
    //qDebug() << "[5] ParamerValue";
    // This should also involve chaning the properties in functions
    problem->defineParameterVector(m_paramValues);
    problem->defineExternalParameters(m_isExternalParams);
    problem->defineParameterGroup(m_externalParamGroups);

    ///\todo Checkout groupDataPathes
    {
        // std::cout << "todo Checkout groupDataPathes" << std::endl;
    }

    /// [6] Goals, priorities and thresholds
    //qDebug() << "[6] Goals and thresholds";
    problem->defineSetGoalVector(m_setGoalVec);
    problem->defineGoalVector(m_goalVec);
    problem->definePriorityVector(m_priorityVec);
    problem->defineThresholdVector(m_threholdVec);

    /// [7] Uncertainties
    //qDebug() << "[7] Uncertainties";

    problem->defineDVecUncertainties(m_dVecUncertainties);
    problem->defineFuncOutUncertainties(m_funcOutUncertainties);

    /// [8] Process problem
    //qDebug() << "[8] Process problem";
    status = problem->processProblemDefinition();
    if(status == FullyDefined) {
        defineProblem(problem);
    }
    return status;
}

TString ProblemGenerator::iprts() const
{
    JsonDocument jdoc;
    JsonArray jarray;
    TVector<IFunctionSPtr> funcs = functionVec();
    for(int i=0; i<funcs.size(); i++) {
        IFunctionSPtr func = funcs[i];
        JsonArray innerArray = toJsonArray(func->inputPrpts());
        jarray.push_back(innerArray);
    }
    jdoc.setArray(jarray);
    return jdoc.toJson(JsonDocument::Compact);
}

void ProblemGenerator::defineIprts(const TString &iprtsString)
{
    JsonDocument jdoc = JsonDocument::fromJson(iprtsString);
    JsonArray jarray = jdoc.array();
    for(int i=0; i<jarray.size(); i++) {
        TVector<ElementProperties> prts = fromJsonArray(jarray[i].toArray());
        m_iprts.push_back(prts);
    }
}

TString ProblemGenerator::lbounds() const
{
    if(boxConstraints()) {
        return elementVecToJsonString(boxConstraints()->lowerBounds());
    } else {
        return TString();
    }
}

void ProblemGenerator::defineLbounds(const TString &lbString)
{
    m_lowerBounds = JsonStringToElementVec(lbString);
}

TString ProblemGenerator::ubounds() const
{
    if(boxConstraints()) {
        return elementVecToJsonString(boxConstraints()->upperBounds());
    } else {
        return TString();
    }
}

void ProblemGenerator::defineUbounds(const TString &ubString)
{
    m_upperBounds = JsonStringToElementVec(ubString);
}

TString ProblemGenerator::oprts() const
{
    JsonDocument jdoc;
    JsonArray jarray;
    TVector<IFunctionSPtr> funcs = functionVec();
    for(int i=0; i<funcs.size(); i++) {
        IFunctionSPtr func = funcs[i];
        JsonArray innerArray = toJsonArray(func->outputPrpts());
        jarray.push_back(innerArray);
    }
    jdoc.setArray(jarray);
    return jdoc.toJson(JsonDocument::Compact);
}

void ProblemGenerator::defineOprts(const TString &oprtsString)
{
    JsonDocument jdoc = JsonDocument::fromJson(oprtsString);
    JsonArray jarray = jdoc.array();
    for(int i=0; i<jarray.size(); i++) {
        TVector<ElementProperties> prts = fromJsonArray(jarray[i].toArray());
        m_oprts.push_back(prts);
    }
}

TString ProblemGenerator::paramValueVec() const
{
    return elementVecToJsonString(parameterVector());
}

void ProblemGenerator::defineParamValues(const TString &value)
{
    m_paramValues = JsonStringToElementSPtrVec(value);
}

TString ProblemGenerator::externalParam() const
{
    JsonDocument jdoc;
    jdoc.setArray(toJsonArray(isExternalParameters()));
    return jdoc.toJson(JsonDocument::Compact);
}

void ProblemGenerator::defineExternalParam(const TString &value)
{
    JsonDocument jdoc = JsonDocument::fromJson(value);
    fromJsonArray(jdoc.array(), m_isExternalParams);
}

TString ProblemGenerator::setGoals() const
{
    return boolVecToJsonString(setGoalVec());
}

void ProblemGenerator::defineSetGoals(const TString &setGoalString)
{
    m_setGoalVec = JsonStringToBoolVec(setGoalString);
}

TString ProblemGenerator::goals() const
{
    return elementVecToJsonString(goalVec());
}

void ProblemGenerator::defineGoals(const TString &goalString)
{
    m_goalVec = JsonStringToElementSPtrVec(goalString);
}

TString ProblemGenerator::priorities() const
{
    return intVecToJsonString(priorityVec());
}

void ProblemGenerator::definePriorities(const TString &priorityString)
{
    m_priorityVec = JsonStringToIntVec(priorityString);
}

TString ProblemGenerator::thresholds() const
{
    return elementVecToJsonString(thresholdVec());
}

void ProblemGenerator::defineThresholds(const TString &thresholdString)
{
    m_threholdVec = JsonStringToElementSPtrVec(thresholdString);
}

TString ProblemGenerator::dVecUncertaintiesJson()
{
    return uncertainMappingsToJsonString(dVecUncertainties());
}

void ProblemGenerator::defineDVecUncertaintiesFromJson(const TString &uncertaintiesString)
{
    m_dVecUncertainties = JsonStringToUncertaintyMapping(uncertaintiesString);
}

TString ProblemGenerator::funcOutUncertaintiesJson()
{
    return uncertainMappingsToJsonString2D(funcOutUncertainties());
}

void ProblemGenerator::defineFuncOutVecUncertaintiesFromJson(const TString &uncertaintiesString)
{
    m_funcOutUncertainties = JsonStringToUncertaintyMapping2D(uncertaintiesString);
}

TString ProblemGenerator::functionsPropertiesJson() const
{
    JsonDocument jdoc;
    JsonArray jarray = functionsToJsonArray(functionVec());
    jdoc.setArray(jarray);
    return jdoc.toJson(JsonDocument::Compact);
}

void ProblemGenerator::defineFunctionsPropertiesFromJson(const TString &funcString)
{
    JsonDocument jdoc = JsonDocument::fromJson(funcString);
    m_funcPrpts = jdoc.array();
}

TString ProblemGenerator::functionTypesJson() const
{
    TStringList list;
    TVector<IFunctionSPtr> funcs = functionVec();
    for(int i=0; i<funcs.size(); i++) {
        list.push_back(funcs[i]->typeStr());
    }
    return join(list, TagDelimiter);
}

void ProblemGenerator::defineFunctionTypesFromJson(const TString &typesStr)
{
    m_funcTypes = split(typesStr, TagDelimiter);
}

TString ProblemGenerator::functionPathes() const
{
    TStringList list;
    TVector<IFunctionSPtr> funcs = functionVec();
    for(int i=0; i<funcs.size(); i++) {
        list.push_back(funcs[i]->path());
    }
    return join(list, TagDelimiter);
}

void ProblemGenerator::defineFunctionPathes(const TString &pathes)
{
    m_funcPathes = split(pathes, TagDelimiter);
}

TString ProblemGenerator::mapToString(const TVector<TVector<int> >& map) const
{
    JsonDocument jdoc;
    JsonArray jarray = toJsonArray(map);
    jdoc.setArray(jarray);
    return jdoc.toJson(JsonDocument::Compact);
}

TVector<TVector<int> > ProblemGenerator::stringToMap(const TString &mapString)
{
    TVector<TVector<int> > vec;
    JsonDocument jdoc = JsonDocument::fromJson(mapString);
    JsonArray jarray = jdoc.array();
    fromJsonArray(jarray, vec);
    return vec;
}

TString ProblemGenerator::elementVecToJsonString(const TVector<IElement> &vec) const
{
    JsonDocument jdoc;
    JsonArray array = toJsonArrayFullInfo(vec);
    jdoc.setArray(array);
    return jdoc.toJson(JsonDocument::Compact);
}

TString ProblemGenerator::boolVecToJsonString(const TVector<bool> &vec) const
{
    JsonDocument jdoc;
    JsonArray array = toJsonArray(vec);
    jdoc.setArray(array);
    return jdoc.toJson(JsonDocument::Compact);
}

TString ProblemGenerator::intVecToJsonString(const TVector<int> &vec) const
{
    JsonDocument jdoc;
    JsonArray array = toJsonArray(vec);
    jdoc.setArray(array);
    return jdoc.toJson(JsonDocument::Compact);
}

TString ProblemGenerator::elementVecToJsonString(const TVector<IElementSPtr> &vec) const
{
    /// \todo handle nonparamtric distribution
    JsonDocument jdoc;
    JsonArray array = toJsonArrayFullInfo(vec);
    jdoc.setArray(array);
    return jdoc.toJson(JsonDocument::Compact);
}

TVector<IElement> ProblemGenerator::JsonStringToElementVec(const TString &JsonString)
{
    TVector<IElement> vec;
    JsonDocument jdoc = JsonDocument::fromJson(JsonString);
    JsonArray array = jdoc.array();
    for(int i=0; i<array.size(); i++) {
        JsonObject jobj = array[i].toObject();
        ElementType type = stringToElemType(jobj["type"].toString());
        IElement elm(type);
        elm.defineValue(jobj["value"].toDouble());
        if(jobj.contains("dist")) {
            JsonObject distObj = jobj["dist"].toObject();
            IDistribution* dist = JsonObjectToDistribution(distObj);
            elm.defineDist(dist);
        }
        vec.push_back(elm);
    }
    return vec;
}

TVector<IElementSPtr> ProblemGenerator::JsonStringToElementSPtrVec(const TString &JsonString)
{
    TVector<IElementSPtr> vec;
    JsonDocument jdoc = JsonDocument::fromJson(JsonString);
    JsonArray array = jdoc.array();
    fromJsonArrayFullInfo(array, vec);
    return vec;
}

TVector<bool> ProblemGenerator::JsonStringToBoolVec(const TString &JsonString)
{
    TVector<bool> vec;
    JsonDocument jdoc = JsonDocument::fromJson(JsonString);
    JsonArray array = jdoc.array();
    fromJsonArray(array,vec);
    return vec;
}

TVector<int> ProblemGenerator::JsonStringToIntVec(const TString &JsonString)
{
    TVector<int> vec;
    JsonDocument jdoc = JsonDocument::fromJson(JsonString);
    JsonArray array = jdoc.array();
    fromJsonArray(array,vec);
    return vec;
}

TString ProblemGenerator::uncertainMappingsToJsonString(TVector<UncertaintyMapping*> mapping)
{
    /// Using jason format for passing uncertain mapping;
    /// considering convert the setting completely to json instead of xml
    JsonDocument jDoc;
    JsonArray array = toJsonArray(mapping);
    jDoc.setArray(array);
    return TString(jDoc.toJson(JsonDocument::Compact));
}

TString ProblemGenerator::uncertainMappingsToJsonString2D(TVector<TVector<UncertaintyMapping*> > mapping)
{
    JsonDocument jDoc;
    JsonArray array = toJsonArray(mapping);
    jDoc.setArray(array);
    return TString(jDoc.toJson(JsonDocument::Compact));
}

TVector<UncertaintyMapping*> ProblemGenerator::JsonStringToUncertaintyMapping(
        const TString &mappingString)
{
    TVector<UncertaintyMapping*> vec;
    JsonDocument jdoc = JsonDocument::fromJson(mappingString);
    JsonArray outArray = jdoc.array();
    fromJsonArray(outArray, vec);

    return vec;
}

TVector<TVector<UncertaintyMapping*> > ProblemGenerator::JsonStringToUncertaintyMapping2D(const TString &mappingString)
{
    TVector<TVector<UncertaintyMapping*> > vec;
    JsonDocument jdoc = JsonDocument::fromJson(mappingString);
    JsonArray outArray = jdoc.array();
    fromJsonArray(outArray, vec);
    return vec;
}

TVector<IElementSPtr> ProblemGenerator::realVecToElemVec(const TVector<double> &vec)
{
    TVector<IElementSPtr> res;
    std::transform(vec.cbegin(), vec.cend(), std::back_inserter(res),
                   [](double val) {return IElementSPtr(new IElement(val));});
    return res;
}

TString ProblemGenerator::externalParamGroupDataPathes() const
{
    return join(m_groupDataPathes, TagDelimiter);
}

void ProblemGenerator::defineExternalParamGroupDataPathes(const TString &pathes)
{
    m_groupDataPathes = split(pathes, TagDelimiter);
}

void ProblemGenerator::processExternalParamGroupData()
{
    for(const TString &path : m_groupDataPathes) {
        XMLDocument xmlDoc;

        if(xmlDoc.LoadFile(path.c_str()) == XML_SUCCESS) {

            XMLElement* dataElm = xmlDoc.FirstChildElement("GroupData");
            XMLElement* endDataElm = xmlDoc.LastChildElement("GroupData");
            if(dataElm) {
                XMLElement* group = dataElm->FirstChildElement("Group");
                while(group) {
                    if(group == endDataElm) {
                        break;
                    }

                    int idx = toInt(TString(group->Attribute("index")));

                    // data vector
                    XMLElement* vec = group->FirstChildElement("RealVector");
                    XMLElement* endVec = group->LastChildElement();
                    SampleVectorsSPtr priorDataGroup = SampleVectorsSPtr(new SampleVectors());

                    while(vec) {
                        TString vecString(vec->GetText());
#ifdef TIGON_DEBUG
                        std:: cout << vecString << endl;
#endif
                        TStringList vecList = split(vecString, ",");
                        TVector<double> priorData;
                        std::transform(vecList.begin(), vecList.end(),
                                       std::back_inserter(priorData),
                                       [](const TString & str) {return toDouble(str);});

                        priorDataGroup->addSample(priorData);

                        if(vec == endVec) {
                            break;
                        } else {
                            vec = vec->NextSiblingElement();
                        }
                    }
                    problem()->addGroupData(idx, priorDataGroup);

                    group = group->NextSiblingElement();
                }
            }
        }
    }
}

TString ProblemGenerator::externalParamGroups() const
{
    JsonDocument jdoc;
    JsonArray array = toJsonArray(problem()->externalParameterGroups());
    jdoc.setArray(array);
    return jdoc.toJson(JsonDocument::Compact);
}

void ProblemGenerator::defineExternalParamGroups(const TString &groupsString)
{
    JsonDocument jdoc = JsonDocument::fromJson(groupsString);
    JsonArray outArray = jdoc.array();
    fromJsonArray(outArray, m_externalParamGroups);
    return;
}

} // namespace Operators
} // namespace Tigon

