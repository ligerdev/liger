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
#include <tigon/Utils/JsonUtils.h>

#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <tigon/Factories/DistributionFactory.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>

using namespace Tigon::Representation;
namespace Tigon {

JsonArray toJsonArrayDoubleValue(const TVector<IElementSPtr> &vec)
{
    JsonArray jArray;
    std::transform(vec.cbegin(), vec.cend(),std::back_inserter(jArray),
                   [](IElementSPtr t) {return t->value<double>();});
    return jArray;
}

JsonArray toJsonArrayDoubleValue(const TVector<IElement> &vec)
{
    JsonArray jArray;
    std::transform(vec.cbegin(), vec.cend(),std::back_inserter(jArray),
                   [](const IElement& t) {return t.value<double>();});
    return jArray;
}

void fromJsonArrayDoubleValue(JsonArray array, TVector<IElementSPtr> &elems)
{
    for(int i=0; i<array.size(); i++) {
        elems[i]->defineValue(array[i].toDouble());
    }
}

JsonArray toJsonArrayFullInfo(const TVector<IElementSPtr> &vec)
{
    JsonArray array;
    for(auto elem : vec) {
        JsonObject jobj;
        jobj["type"]  = elemTypeToString(elem->type());
        jobj["value"] = JsonValue(elem->value());
        if(elem->dist() != nullptr) {
            jobj["dist"]  = distToJsonObject(*(elem->dist()));
        }
        array.push_back(jobj);
    }
    return array;
}

JsonArray toJsonArrayFullInfo(const TVector<IElement>& vec)
{
    JsonArray array;
    for(auto elem : vec) {
        JsonObject jobj;
        jobj["type"]  = elemTypeToString(elem.type());
        jobj["value"] = JsonValue(elem.value());
        if(elem.dist()!=0) {
            jobj["dist"]  = distToJsonObject(*(elem.dist()));
        }
        array.push_back(jobj);
    }
    return array;
}

JsonArray toJsonArray(const TVector<double> &vec)
{
    JsonArray jArray;
    std::transform(vec.begin(), vec.end(),std::back_inserter(jArray),
                   [](double t) {return t;});
    return jArray;
}

JsonArray toJsonArray(const TVector<int> &vec)
{
    JsonArray jArray;
    std::transform(vec.begin(), vec.end(),std::back_inserter(jArray),
                   [](int t) {return t;});
    return jArray;
}

JsonArray toJsonArray(const TVector<bool> &vec)
{
    JsonArray jArray;
    std::transform(vec.begin(), vec.end(),std::back_inserter(jArray),
                   [](bool t) {return t;});
    return jArray;
}

JsonArray toJsonArray(ISet* solSet)
{
    JsonArray jArr;
    for(int i = 0; i < solSet->size(); i++) {
        IMappingSPtr cmap = solSet->at(i);
        if(cmap->isEvaluated()) {
            // Record only evaluated solutions
            jArr.push_back(toJsonObj(cmap));
        }
    }
    return jArr;
}

JsonObject toJsonObjWithNames(const TVector<ElementProperties> &prpts,
                              const TVector<IElementSPtr> &vec)
{
    JsonObject jObj;
    for(size_t i = 0; i < vec.size(); i++) {
        jObj[prpts[i].name()] = vec[i]->value();
    }

    return jObj;
}

JsonObject toJsonObj(IMappingSPtr imap)
{
    JsonObject jObj;

    // Decision vector
    jObj[Log::LDVec] = toJsonArrayFullInfo(imap->decisionVec());

    // Parameter vector
    jObj[Log::LPVec] = toJsonArrayDoubleValue(imap->parameterVec());

    // Weight vector
    JsonArray wVecArray;
    TVector<double> wvec = imap->weightingVec();
    std::transform(wvec.begin(), wvec.end(),std::back_inserter(wVecArray),
                   [](double t) {return t;});
    jObj[Log::LWVec] = wVecArray;

    // isEvaluated
    bool isEval = imap->isEvaluated();
    jObj[Log::LIsEval] = isEval;

    // isObjEvaluated
    bool isObjEval = imap->isObjectiveVecEvaluated();
    jObj[Log::LObjEval] = isObjEval;

    // Objective vector
    if(isObjEval && imap->objectiveVec().size() > 0) {
        jObj[Log::LOVec] = toJsonArrayFullInfo(imap->objectiveVec());
    }

    // isConstEvaluated
    bool isConstEval = imap->isConstraintVecEvaluated();
    jObj[Log::LConstEval] = isConstEval;

    // Constraint vector
    if(isConstEval && imap->constraintVec().size() > 0) {
        jObj[Log::LCVec] = toJsonArrayFullInfo(imap->constraintVec());
    }

    /// \todo isUnusedEvaluated
    //bool isUnusedEval = imap->isUnusedVecEvaluated();
    //jObj[Log::LConstEval] = isConstEval;

    // Unused vector
    jObj[Log::LUVec] = toJsonArrayFullInfo(imap->unusedVec());

    // isScalarised
    jObj[Log::LScalarised] = imap->isScalarised();

    // cost
    jObj[Log::LCost] = imap->doubleCost();

    jObj[Log::LRunNumber] = imap->runNumber();

    return jObj;
}

bool fromJSonObj(IMappingSPtr imap, const JsonObject &jObj)
{
    if(jObj.contains(Log::LFunc)) {
        /// Create an IMapping from an evaluation entry

        // Decision variables/Parameters from inputs
        if(!jObj.contains(Log::LInputs)) {
            return false;
        }
        JsonObject varsObject = jObj[Log::LInputs].toObject();
        TVector<ElementProperties> varsProps = imap->problem()->dPrpts();

        for(size_t i = 0; i < varsProps.size(); ++i) {
            imap->decisionVar(i)->
                    defineValue(varsObject[varsProps[i].name()].toDouble());
        }

        varsProps  = imap->problem()->pPrpts();

        for(size_t i = 0; i < varsProps.size(); ++i) {
            imap->parameterVar(i)->
                    defineValue(varsObject[varsProps[i].name()].toDouble());
        }

        // Objectives/Costraints/Unused from outputs
        if(!jObj.contains(Log::LOutputs)) {
            return false;
        }
        varsObject = jObj[Log::LOutputs].toObject();
        varsProps  = imap->problem()->oPrpts();
        for(size_t i = 0; i < varsProps.size(); ++i) {
            imap->objectiveVar(i)->
                    defineValue(varsObject[varsProps[i].name()].toDouble());
        }

        varsProps = imap->problem()->cPrpts();

        for(size_t i = 0; i < varsProps.size(); ++i) {
            imap->constraintVar(i)->
                    defineValue(varsObject[varsProps[i].name()].toDouble());
        }


        varsProps = imap->problem()->uPrpts();

        for(size_t i = 0; i < varsProps.size(); ++i) {
            imap->unusedVar(i)->
                    defineValue(varsObject[varsProps[i].name()].toDouble());
        }

        // Mark as evaluated
        imap->defineEvaluated(true);

    } else if(jObj.contains(Log::LDVec)) {

        /// Restore an IMapping logged as a JsonObject
        // Decision vector
        JsonArray dVecArray = jObj[Log::LDVec].toArray();
        TVector<IElementSPtr> dElems;
        fromJsonArrayFullInfo(dVecArray, dElems);
        imap->defineDecisionVec(dElems);

        // Parameter vector - value only
        JsonArray pVecArray = jObj[Log::LPVec].toArray();
        TVector<IElementSPtr> pElems = imap->parameterVec();
        fromJsonArrayDoubleValue(pVecArray, pElems);

        // Weight vector
        if(jObj.contains(Log::LWVec)) {
            JsonArray wVecArray = jObj[Log::LWVec].toArray();
            for(int i = 0; i < wVecArray.size(); i++) {
                imap->defineWeight(i, wVecArray[i].toDouble());
            }
        }

        // isEvaluated
        if(jObj.contains(Log::LIsEval)) {
            bool isEval = jObj[Log::LIsEval].toBool();
            imap->defineEvaluated(isEval);
        }

        // isObjEvaluated
        bool isObjEval = false;
        if(jObj.contains(Log::LObjEval)) {
            isObjEval = jObj[Log::LObjEval].toBool();
            imap->defineObjectiveVecEvaluated(isObjEval);
        }

        // Objective vector
        if(isObjEval && jObj.contains(Log::LOVec)) {
            JsonArray oVecArray = jObj[Log::LOVec].toArray();
            TVector<IElementSPtr> oElems;
            fromJsonArrayFullInfo(oVecArray, oElems);
            imap->defineObjectiveVec(oElems);
        }

        // isConstEvaluated
        bool isConstEval = false;
        if(jObj.contains(Log::LConstEval)) {
            isConstEval = jObj[Log::LConstEval].toBool();
            imap->defineConstraintVecEvaluated(isConstEval);
        }

        // Constraint vector
        if(isConstEval && jObj.contains(Log::LCVec)) {
            JsonArray cVecArray = jObj[Log::LCVec].toArray();
            TVector<IElementSPtr> cElems;
            fromJsonArrayFullInfo(cVecArray, cElems);
            imap->defineConstraintVec(cElems);
        }

        /// \todo isUnusedEvaluated
        //// isUnusedEvaluated
        bool isUnusedEval = true;
        //bool isUnusedEval = false;
        //if(jObj.contains(Log::LUnusedEval)) {
        //    isUnusedEval = jObj[Log::LUnusedEval].toBool();
        //    imap->defineUnusedVecEvaluated(isUnusedEval);
        //}

        // Unused vector
        if(isUnusedEval && jObj.contains(Log::LUVec)) {
            JsonArray uVecArray = jObj[Log::LUVec].toArray();
            TVector<IElementSPtr> cElems;
            fromJsonArrayFullInfo(uVecArray, cElems);
            imap->defineUnusedVec(cElems);
        }

        // isScalarised
        bool isScal = false;
        if(jObj.contains(Log::LScalarised)) {
            isScal = jObj[Log::LScalarised].toBool();
            imap->defineScalarised(isScal);
        }

        // cost
        if(isScal && jObj.contains(Log::LCost)) {
            imap->defineCost(jObj[Log::LCost].toDouble());
        }

        // run number
        if(jObj.contains(Log::LRunNumber)) {
            imap->defineRunNumber(jObj[Log::LRunNumber].toInt());
        }

    } else {
        return false;
    }

    return true;
}

JsonArray toIDJsonArray(const TVector<ElementProperties> &prpts)
{
    JsonArray array;
    std::transform(prpts.cbegin(), prpts.cend(), std::back_inserter(array),
                   [](const ElementProperties& p){return p.ID();});
    return array;
}

JsonObject problemToJsonObject(const ProblemSPtr prob)
{
    JsonObject jprob;

    jprob["problemName"] = prob->problemProperties().name();

    TVector<IFunctionSPtr> funcs = prob->functionVec();
    JsonArray jarrayI, jarrayO, funcTypes, funcPathes;
    for(IFunctionSPtr func : funcs) {
        funcTypes.push_back(func->typeStr());
        if(func->type() == FunctionType::Internal) {
            /// Using the className() method returns always Tigon::Representation::IFunction
            /// for all functions. Given that the expected values of funcPathes is supposed
            /// to return the specific implementation of the function i.e. DTLZ1 etc the path
            /// method of the function should be used instead. This results in crashes when the
            /// telescope of the network plugin. Also the m_functionPath is supposed to store
            /// the actual path of the function in the cases where Matlab or SharedLib is used
            /// or the actual class name (e.g. Tigon::Representation::DTLZ1) in the case the
            /// function is of the Internal type. This if statement is kept in case something
            /// is changed in the future.
            //funcPathes.push_back(func.get()->className());
            funcPathes.push_back(func->path());
        } else {
            funcPathes.push_back(func->path());
        }
        JsonArray innerArrayI = toJsonArray(func->inputPrpts());
        JsonArray innerArrayO = toJsonArray(func->outputPrpts());
        jarrayI.push_back(innerArrayI);
        jarrayO.push_back(innerArrayO);
    }
    jprob["funcTypes"]  = funcTypes;
    jprob["funcPathes"] = funcPathes;

    jprob["funcPrpts"] = functionsToJsonArray(funcs);

    jprob["iprts"] = jarrayI;
    jprob["oprts"] = jarrayO;

    jprob["dIDs"]  = toIDJsonArray(prob->dPrpts());
    jprob["pIDs"]  = toIDJsonArray(prob->pPrpts());
    jprob["oIDs"]  = toIDJsonArray(prob->oPrpts());
    jprob["cIDs"]  = toIDJsonArray(prob->cPrpts());
    jprob["uIDs"]  = toIDJsonArray(prob->uPrpts());

    if(!prob->boxConstraints()) {
        jprob["lbs"] = JsonValue();
        jprob["ubs"] = JsonValue();
    } else {
        jprob["lbs"] = toJsonArrayDoubleValue(prob->boxConstraints()->lowerBounds());
        jprob["ubs"] = toJsonArrayDoubleValue(prob->boxConstraints()->upperBounds());
    }

    jprob["pVector"]    = toJsonArrayFullInfo(prob->parameterVector());
    jprob["setGoals"]   = toJsonArray(prob->setGoalVector());
    jprob["goals"]      = toJsonArrayDoubleValue(prob->goalVector());
    jprob["priorities"] = toJsonArray(prob->priorityVector());
    jprob["thresholds"] = toJsonArrayDoubleValue(prob->thresholdVector());
    jprob["isExternalParameters"] = toJsonArray(prob->isExternalParameters());

    jprob["dPrpts"]  = toJsonArray(prob->dPrpts());
    jprob["pPrpts"]  = toJsonArray(prob->pPrpts());
    jprob["oPrpts"]  = toJsonArray(prob->oPrpts());
    jprob["cPrpts"]  = toJsonArray(prob->cPrpts());
    jprob["uPrpts"]  = toJsonArray(prob->uPrpts());

    jprob["f2dMap"]  = toJsonArray(prob->f2dMap());
    jprob["f2pMap"]  = toJsonArray(prob->f2pMap());
    jprob["f2oMap"]  = toJsonArray(prob->f2oMap());
    jprob["f2cMap"]  = toJsonArray(prob->f2cMap());
    jprob["f2uMap"]  = toJsonArray(prob->f2uMap());

    jprob["dVecUncertainties"] = toJsonArray(prob->dVecUncertainties());
    jprob["funcOutUncertainties"] = toJsonArray(prob->funcOutUncertainties());

    return jprob;
}

JsonArray toJsonArray(const TVector<ElementProperties> &prts)
{
    JsonArray array;
    for(size_t i=0; i<prts.size(); i++) {
        JsonObject jobj;
        jobj["idx"]  = prts[i].idx();
        jobj["name"] = prts[i].name();
        jobj["description"] = prts[i].description();
        jobj["type"] = elemTypeToString(prts[i].type());
        jobj["optimizationType"] = prts[i].optimizationType();
        jobj["unit"] = prts[i].unit();
        array.push_back(jobj);
    }
    return array;
}

void fromJsonArray(const JsonArray &jarray, TVector<TVector<ElementProperties> > &prts)
{
    std::transform(jarray.constBegin(), jarray.constEnd(), std::back_inserter(prts),
                   [](const JsonValue& val){
        TVector<ElementProperties> prts = fromJsonArray(val.toArray());
        return prts;
    });
}

TVector<ElementProperties> fromJsonArray(JsonArray array)
{
    TVector<ElementProperties> elemPrts;
    std::transform(array.constBegin(), array.constEnd(),
                   std::back_inserter(elemPrts),
                   [](const JsonValue val) {

        JsonObject jobj = val.toObject();
        int idx = jobj["idx"].toInt();
        TString name = jobj["name"].toString();
        TString description = jobj["description"].toString();
        ElementType type =  stringToElemType(jobj["type"].toString());
        OptimizationType optType = static_cast<OptimizationType>(jobj["optimizationType"].toInt());
        ElementProperties prt = ElementPropertiesFactory
                ::instance()->createElementProperties(idx, name, description,
                                                      type, optType);
        return prt;
    });
    return  elemPrts;
}

void fromJsonArrayFullInfo(JsonArray array, TVector<IElementSPtr>& elems)
{
    for(int i=0; i<array.size(); i++) {
        JsonObject jobj = array[i].toObject();
        ElementType type = stringToElemType(jobj["type"].toString());
        IElementSPtr elm = IElementSPtr(new IElement(type));
        elm->defineValue(jobj["value"].toDouble());
        if(jobj.contains("dist")) {
            JsonObject distObj = jobj["dist"].toObject();
            IDistribution* dist = JsonObjectToDistribution(distObj);
            elm->defineDist(dist);
        }
        elems.push_back(elm);
    }
}

TString elemTypeToString(const ElementType &type)
{
    TString content;
    switch(type) {
    case ElementType::RealType:
        content = Tigon::ETReal;
        break;
    case ElementType::IntegerType:
        content = Tigon::ETInteger;
        break;
    case ElementType::OrdinalType:
        content = Tigon::ETOrdinal;
        break;
    case ElementType::NominalType:
        content = Tigon::ETNominal;
        break;
    }
    return content;
}

ElementType stringToElemType(const TString &typeString)
{
    ElementType type = ElementType::RealType;
    if(typeString == Tigon::ETReal) {
        type =  ElementType::RealType;
    } else if(typeString == Tigon::ETInteger) {
        type = ElementType::IntegerType;
    } else if(typeString == Tigon::ETOrdinal) {
        type = ElementType::OrdinalType;
    } else if(typeString == Tigon::ETNominal) {
        type = ElementType::NominalType;
    }
    return type;
}

JsonArray toJsonArray(const TVector<TVector<int> > &vec)
{
    JsonArray array;
    std::transform(vec.cbegin(), vec.cend(), std::back_inserter(array), [](const TVector<int>& innerVec) {
        JsonArray innerarray;
        std::transform(innerVec.cbegin(), innerVec.cend(), std::back_inserter(innerarray),
                       [](int val) { return JsonValue(val); });
        return innerarray;
    });
    return array;
}

JsonArray functionsToJsonArray(const TVector<IFunctionSPtr> &funcs)
{
    JsonArray jarray;
    for(IFunctionSPtr func : funcs) {
        TObject* object = func.get();

        JsonObject jobj;
        TStringList propertyNames = object->propertyNames();
        for(TString name : propertyNames) {
            TString var = object->propertyValue(name);
            jobj[name] = JsonValue(var);
        }

        jarray.push_back(jobj);
    }
    return jarray;
}

void fromJsonArray(const JsonArray &jarray, TVector<double>& vec)
{
    std::transform(jarray.constBegin(), jarray.constEnd(),
                   std::back_inserter(vec),
                   [](const JsonValue jval) {return jval.toDouble();});
}

void fromJsonArray(const JsonArray &jarray, TVector<int>& vec)
{
    std::transform(jarray.constBegin(), jarray.constEnd(),
                   std::back_inserter(vec),
                   [](const JsonValue jval) {return jval.toInt();});
}

void fromJsonArray(const JsonArray &jarray, TVector<bool>& vec)
{
    if(vec.size() > 0) vec.clear();
    std::transform(jarray.constBegin(), jarray.constEnd(),
                   std::back_inserter(vec),
                   [](const JsonValue jval) {return jval.toBool();});
}

void fromJsonArray(const JsonArray &jarray, TVector<TVector<int> > &vec)
{
    int m = jarray.size();
    vec.clear();
    vec.resize(m);
    for(int i=0; i<m; i++) {
        JsonArray innerArray = jarray[i].toArray();
        int n = innerArray.size();
        vec[i].reserve(n);
        std::transform(innerArray.constBegin(), innerArray.constEnd(),
                       std::back_inserter(vec[i]),
                       [](const JsonValue jval) {return jval.toInt();});
    }
}

///\note hacking. Wait until we updated distribution class
/// \todo handle nonparamtric distribution in a generic way
JsonObject distToJsonObject(const IDistribution& dist)
{
    JsonObject obj;
    obj["type"] = distributionTypeToString(dist.type());
    if(dist.parameterNames().size() > 0) {
        // Parametric
        JsonArray paramsjson;
        for(double param : dist.parameters()) {
            paramsjson.push_back(param);
        }
        obj["parameters"] = paramsjson;
    } else {
        // Non-Parametric
        JsonArray samplesjson;
        for(double svec : dist.sampledVec()) {
            samplesjson.push_back(svec);
        }

        JsonArray weightsjson;
        for(double w : dist.weights()) {
            weightsjson.push_back(w);
        }
        obj["samples"] = samplesjson;
        obj["weights"] = weightsjson;
    }
    return obj;
}

IDistribution* JsonObjectToDistribution(const JsonObject &json)
{
    IDistribution* dist = nullptr;
    DistributionType type = stringToDistributionType(json["type"].toString());
    if(json.contains("parameters")) {
        // Parametric
        TVector<double> params;
        JsonArray array = json["parameters"].toArray();
        for(int i=0; i<array.size(); i++) {
            params.push_back(array[i].toDouble());
        }
        dist = DistributionFactory::instance()->createParametricDist(type, params);
    } else {
        // Non-parametric
        JsonArray jarraySamples = json["samples"].toArray();
        JsonArray jarrayWeights = json["weights"].toArray();
        if(jarraySamples.size() == jarrayWeights.size()) {
            TVector<double> samples, weights;
            for(int i=0; i<jarraySamples.size(); i++) {
                samples.push_back(jarraySamples[i].toDouble());
                weights.push_back(jarrayWeights[i].toDouble());
            }
            dist = DistributionFactory::instance()->createNonParametricDist(type,
                                                                            samples,
                                                                            weights);
        }
    }
    return dist;
}

TString distributionTypeToString(DistributionType type)
{
    TString content;
    switch (type) {
    case GenericDistType:
        content = TString("GenericDistType");
        break;
    case NonParametricDistType:
        content = TString("NonParametricDistType");
        break;
    case MergedDistType:
        content = TString("MergedDistType");
        break;
    case SampledDistType:
        content = TString("SampledDistType");
        break;
    case UniformDistType:
        content = TString("UniformDistType");
        break;
    case NormalDistType:
        content = TString("NormalDistType");
        break;
    case LinearDistType:
        content = TString("LinearDistType");
        break;
    case PeakDistType:
        content = TString("PeakDistType");
        break;
    case BetaDistType:
        content = TString("BetaDistType");
        break;
    case GammaDistType:
        content = TString("GammaDistType");
        break;
    case BinomialDistType:
        content = TString("BinomialDistType");
        break;
    case ChiSquaredDistType:
        content = TString("ChiSquaredDistType");
        break;
    case ExponentialDistType:
        content = TString("ExponentialDistType");
        break;
    case PoissonDistType:
        content = TString("PoissonDistType");
        break;
    case StudentTDistType:
        content = TString("StudentTDistType");
        break;
    case CauchyDistType:
        content = TString("CauchyDistType");
        break;
    case BernoulliDistType:
        content = TString("BernoulliDistType");
        break;
    case DiscreteUniformDistType:
        content = TString("DiscreteUniformDistType");
        break;
    }

    return content;
}

DistributionType stringToDistributionType(const TString &typeString)
{
    DistributionType type = DistributionType::GenericDistType;
    if(typeString == "NonParametricDistType") {
        type = DistributionType::NonParametricDistType;
    } else if(typeString == "MergedDistType") {
        type = DistributionType::MergedDistType;
    } else if(typeString == "SampledDistType") {
        type = DistributionType::SampledDistType;
    } else if(typeString == "UniformDistType") {
        type = DistributionType::UniformDistType;
    } else if(typeString == "NormalDistType") {
        type = DistributionType::NormalDistType;
    } else if(typeString == "LinearDistType") {
        type = DistributionType::LinearDistType;
    } else if(typeString == "PeakDistType") {
        type = DistributionType::PeakDistType;
    } else if(typeString == "BetaDistType") {
        type = DistributionType::BetaDistType;
    } else if(typeString == "GammaDistType") {
        type = DistributionType::GammaDistType;
    } else if(typeString == "BinomialDistType") {
        type = DistributionType::BinomialDistType;
    } else if(typeString == "ChiSquaredDistType") {
        type = DistributionType::ChiSquaredDistType;
    } else if(typeString == "ExponentialDistType") {
        type = DistributionType::ExponentialDistType;
    } else if(typeString == "PoissonDistType") {
        type = DistributionType::PoissonDistType;
    } else if(typeString == "StudentTDistType") {
        type = DistributionType::StudentTDistType;
    } else if(typeString == "CauchyDistType") {
        type = DistributionType::CauchyDistType;
    } else if(typeString == "BernoulliDistType") {
        type = DistributionType::BernoulliDistType;
    } else if(typeString == "DiscreteUniformDistType") {
        type = DistributionType::DiscreteUniformDistType;
    }
    return type;
}

JsonArray toJsonArray(const TVector<UncertaintyMapping *>& mapping)
{
    JsonArray jArray;
    int n = mapping.size();
    for(int i=0; i<n; i++) {
        JsonObject jsonObj;
        UncertaintyMapping* currentMap = mapping[i];
        if(currentMap != nullptr) {
            jsonObj["distributionType"] = distributionTypeToString(currentMap->distributionType());
            JsonArray ct;
            TVector<double> ctV = currentMap->constTerms();
            std::transform(ctV.cbegin(), ctV.cend(), std::back_inserter(ct),
                           [](double val){return JsonValue(val);});
            JsonArray ln;
            TVector<double> lnV = currentMap->linearTerms();
            std::transform(lnV.cbegin(), lnV.cend(), std::back_inserter(ln),
                           [](double val){return JsonValue(val);});
            jsonObj["constTerms"]  =  ct;
            jsonObj["linearTerms"] =  ln;
        }
        jArray.push_back(jsonObj);
    }
    return jArray;
}

void fromJsonArray(const JsonArray& jarray, TVector<UncertaintyMapping *> &vec)
{
    int m = jarray.size();
    vec.resize(m);
    for(int i=0; i<m; i++) {
        JsonObject jobj = jarray[i].toObject();
        if(!jobj.empty()) {
            UncertaintyMapping *cmapping = new UncertaintyMapping;

            TString typeString    = jobj["distributionType"].toString();
            DistributionType type = stringToDistributionType(typeString);
            JsonArray jconstTerm = jobj["constTerms"].toArray();
            JsonArray jlinTerm   = jobj["linearTerms"].toArray();

            TVector<double> constTerm;
            for(JsonValue elem : jconstTerm) {
                constTerm.push_back(elem.toDouble());
            }
            TVector<double> linTerm;
            for(JsonValue elem : jlinTerm) {
                linTerm.push_back(elem.toDouble());
            }

            cmapping->defineDistributionType(type);
            cmapping->defineConstTerms(constTerm);
            cmapping->defineLinearTerms(linTerm);
            vec[i] = cmapping;
        }
    }
}

JsonArray toJsonArray(const TVector<TVector<UncertaintyMapping*>> &mapping)
{
    JsonArray jArray;

    for(TVector<UncertaintyMapping*> map : mapping) {
        JsonArray jinnerArray;
        for(UncertaintyMapping* currentMap : map) {
            JsonObject jsonObj;
            if(currentMap!=nullptr) {
                jsonObj["distributionType"] =
                        distributionTypeToString(currentMap->distributionType());
                JsonArray ct;
                for(double c : currentMap->constTerms()) {
                    ct.push_back(JsonValue(c));
                }
                JsonArray ln;
                for(double v : currentMap->linearTerms()) {
                    ln.push_back(JsonValue(v));
                }
                jsonObj["constTerms"]  =  ct;
                jsonObj["linearTerms"] =  ln;
            }
            jinnerArray.push_back(jsonObj);
        }
        jArray.push_back(jinnerArray);
    }
    return jArray;
}

void fromJsonArray(const JsonArray& jarray,
                   TVector<TVector<UncertaintyMapping*>> &vec)
{
    int m = jarray.size();
    vec.resize(m);
    for(int i=0; i<m; i++) {
        JsonArray innerArray = jarray[i].toArray();
        int n = innerArray.size();
        vec[i].resize(n);
        for(int j=0; j<n; j++) {
            JsonObject jobj = innerArray[j].toObject();
            if(!jobj.empty()) {
                UncertaintyMapping* mapping = new UncertaintyMapping;
                TString typeString = jobj["distributionType"].toString();
                DistributionType type = stringToDistributionType(typeString);

                JsonArray jconstTerm = jobj["constTerms"].toArray();
                JsonArray jlinTerm = jobj["linearTerms"].toArray();

                TVector<double> constTerm;
                for(JsonValue elem : jconstTerm) {
                    constTerm.push_back(elem.toDouble());
                }
                TVector<double> linTerm;
                for(JsonValue elem : jlinTerm) {
                    linTerm.push_back(elem.toDouble());
                }
                mapping->defineDistributionType(type);
                mapping->defineConstTerms(constTerm);
                mapping->defineLinearTerms(linTerm);
                vec[i][j] = mapping;
            }
        }
    }
}

void fromJsonArray(const JsonArray &jarray, TStringList &slist)
{
    std::transform(jarray.constBegin(), jarray.constEnd(), std::back_inserter(slist),
                   [](const JsonValue& val) {return val.toString();});
}

TStringList toStringList(const JsonArray &array)
{
    TStringList list;
    std::transform(array.constBegin(), array.constEnd(),
                   std::back_inserter(list),
                   [](const JsonValue& val) {return val.toString();});
    return list;
}

void assignFunctionProperties(IFunctionSPtr func, const JsonObject &prptsObj)
{
    TObject* object = func.get();
    TStringList keys = prptsObj.keys();
    for(TString name : keys) {
        TString valueStr = prptsObj[name].toString();
        object->setProperty(name, valueStr);
    }
}

} // namespace Tigon
