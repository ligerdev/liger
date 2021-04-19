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
#ifndef PROBLEMGENERATOR_H
#define PROBLEMGENERATOR_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Properties/ElementProperties.h>

#include <json/json.h>

namespace Tigon {
namespace Operators {
/*!
 * \brief The ProblemGenerator class. Configurator for IFormulation.
 * This class is meant for helping the Liger Problem Editor to access/edit a
 * tigon problem.
 * \note Json document format is used to parse problem properties.
 * \todo Unify Tigon Error Code
 */
class LIGER_TIGON_EXPORT ProblemGenerator : public IFormulation
{
    HANDLE_READ_PROPERTIES_BEGIN(IFormulation)
    READ(funcTypes, functionTypesJson)
    READ(funcPathes, functionPathes)
    READ(funcPrpts, functionsPropertiesJson)
    READ(iprts, iprts)
    READ(oprts, oprts)
    READ(lbounds, lbounds)
    READ(ubounds, ubounds)
    READ(paramValueVec, paramValueVec)
    READ(externalParam, externalParam)
    READ(externalParamGroups, externalParamGroups)
    READ(externalParamGroupDataPathes, externalParamGroupDataPathes)
    READ(setGoals, setGoals)
    READ(goals, goals)
    READ(priorities, priorities)
    READ(thresholds, thresholds)
    READ(dVecUncertainties, dVecUncertaintiesJson)
    READ(funcOutUncertainties, funcOutUncertaintiesJson)
    READ(f2dMap, f2dMap)
    READ(f2pMap, f2pMap)
    READ(f2oMap, f2oMap)
    READ(f2cMap, f2cMap)
    READ(f2uMap, f2uMap)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFormulation)
    WRITE(funcTypes, TString, defineFunctionTypesFromJson)
    WRITE(funcPathes, TString, defineFunctionPathes)
    WRITE(funcPrpts, TString, defineFunctionsPropertiesFromJson)
    WRITE(iprts, TString, defineIprts)
    WRITE(oprts, TString, defineOprts)
    WRITE(lbounds, TString, defineLbounds)
    WRITE(ubounds, TString, defineUbounds)
    WRITE(paramValueVec, TString, defineParamValues)
    WRITE(externalParam, TString, defineExternalParam)
    WRITE(externalParamGroups, TString, defineExternalParamGroups)
    WRITE(externalParamGroupDataPathes, TString, defineExternalParamGroupDataPathes)
    WRITE(setGoals, TString, defineSetGoals)
    WRITE(goals, TString, defineGoals)
    WRITE(priorities, TString, definePriorities)
    WRITE(thresholds, TString, defineThresholds)
    WRITE(dVecUncertainties, TString, defineDVecUncertaintiesFromJson)
    WRITE(funcOutUncertainties, TString, defineFuncOutVecUncertaintiesFromJson)
    WRITE(f2dMap, TString, definef2dMap)
    WRITE(f2pMap, TString, definef2pMap)
    WRITE(f2oMap, TString, definef2oMap)
    WRITE(f2cMap, TString, definef2cMap)
    WRITE(f2uMap, TString, definef2uMap)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::ProblemGenerator)

public:
    ProblemGenerator();
    ProblemGenerator(Tigon::Representation::IPSet* ipset);
    ~ProblemGenerator(){}

    // Information about the node
    TString name();
    TString description();

    void initialise();
    void evaluateNode();

    /*!
     * \brief Export the problem formulation as a json object.
     * \return Json string.
     */
    TString problemDataJson() const;

    /*!
     * \brief Load the problem from a json object. If the problem defined by the
     * json object cannot be processed, the internal problem remains unchanged.
     * \param Json object that contains the problem information
     * \return Problem definition status. FullyDefined is the problem is full
     * processed.
     */
    Tigon::ProblemDefinitionStatus setProblemDataJson(const TString &str);

    /*!
     * \brief processFormualtion must be called after the problem is configured
     * \return Problem definition status.
     */
    Tigon::ProblemDefinitionStatus processFormulation();

    /*!
     * \brief Create a function for given the type and path.
     * \param Name of the test function or path to the shared library/matlab script
     * \param Type of the function, "Internal", "SharedLib", or "Matlab"
     * \return shared pointer of the newly created function.
     * \note It returns shared pointer not function pointer
     * \todo shared library
     */
    static IFunctionSPtr createFunction(const TString &funcTypeStr,
                                        const TString &funcPath);

    ///\note This function will replace createFunction in future release
    static IFunctionSPtr createFunctionByName(const TString &className,
                                              const TString& funcPath=TString(""));

    /// Properties needed for GUI to construct a problem
    TString problemDefinitionPath()                 const;  // place holder
    void    defineProblemDefinitionPath(TString fileName);  // place holder

    TString f2dMap();
    void    definef2dMap(const TString& mapString);
    TString f2pMap();
    void    definef2pMap(const TString& mapString);
    TString f2oMap();
    void    definef2oMap(const TString& mapString);
    TString f2cMap();
    void    definef2cMap(const TString& mapString);
    TString f2uMap();
    void    definef2uMap(const TString& mapString);

    /// Input properties for functions
    TString iprts() const;
    void defineIprts(const TString &iprtsString);

    /// Output properties for functions
    TString oprts() const;
    void defineOprts(const TString &oprtsString);

    TString lbounds() const;
    void defineLbounds(const TString &lbString);
    TString ubounds() const;
    void defineUbounds(const TString &ubString);

    /// Parameters (including external parameters)
    TString paramValueVec() const;
    void defineParamValues(const TString& value);
    TString externalParam() const;
    void defineExternalParam(const TString& value);
    TString externalParamGroupDataPathes() const;
    void defineExternalParamGroupDataPathes(const TString &pathes);
    TString externalParamGroups() const;
    void defineExternalParamGroups(const TString& groupsString);
    /*!
     * \brief Loads group data for external parameters
     */
    void processExternalParamGroupData();

    /// Set Goals
    TString setGoals() const;
    void defineSetGoals(const TString& setGoalString);

    /// Priorities
    TString priorities() const;
    void definePriorities(const TString& priorityString);

    /// Goals
    TString goals() const;
    void defineGoals(const TString& goalString);

    /// Thresholds
    TString thresholds() const;
    void defineThresholds(const TString& thresholdString);

    /// Decision uncertainties
    TString dVecUncertaintiesJson();
    void defineDVecUncertaintiesFromJson(const TString& uncertaintiesString);
    TString funcOutUncertaintiesJson();
    void defineFuncOutVecUncertaintiesFromJson(const TString& uncertaintiesString);

    /// Function properties
    TString functionsPropertiesJson() const;
    void defineFunctionsPropertiesFromJson(const TString& funcString);

    TString functionTypesJson() const;
    void defineFunctionTypesFromJson(const TString& typesStr);
    TString functionPathes() const;
    void defineFunctionPathes(const TString& pathes);

private:
    /// Private data
    TVector<TVector<int> > m_f2dMap;
    TVector<TVector<int> > m_f2pMap;
    TVector<TVector<int> > m_f2oMap;
    TVector<TVector<int> > m_f2cMap;
    TVector<TVector<int> > m_f2uMap;

    TVector<TVector<ElementProperties> > m_iprts;
    TVector<TVector<ElementProperties> > m_oprts;

    TVector<IElement>      m_lowerBounds;
    TVector<IElement>      m_upperBounds;
    TVector<IElementSPtr>  m_paramValues;
    TVector<bool>          m_isExternalParams;
    TVector<TVector<int>>  m_externalParamGroups;
    TStringList            m_groupDataPathes;
    TVector<bool>          m_setGoalVec;
    TVector<IElementSPtr>  m_goalVec;
    TVector<int>           m_priorityVec;
    TVector<IElementSPtr>  m_threholdVec;

    TVector<UncertaintyMapping*>           m_dVecUncertainties;
    TVector<TVector<UncertaintyMapping*> > m_funcOutUncertainties;

    Json::JsonArray m_funcPrpts;
    TStringList m_funcTypes;
    TStringList m_funcPathes;

    /// Private Helper functions
    TString mapToString(const TVector<TVector<int> >& map) const;
    TVector<TVector<int> > stringToMap(const TString& mapString);

    TString elementVecToJsonString(const TVector<Representation::IElement> &vec) const;
    TString boolVecToJsonString(const TVector<bool> &vec) const;
    TString intVecToJsonString(const TVector<int> &vec) const;
    TString elementVecToJsonString(const TVector<Representation::IElementSPtr> &vec) const;
    TVector<IElement> JsonStringToElementVec(const TString &JsonString);
    TVector<IElementSPtr> JsonStringToElementSPtrVec(const TString &JsonString);
    TVector<bool> JsonStringToBoolVec(const TString &JsonString);
    TVector<int> JsonStringToIntVec(const TString &JsonString);

    TString uncertainMappingsToJsonString(TVector<UncertaintyMapping*> mapping);
    TString uncertainMappingsToJsonString2D(TVector<TVector<UncertaintyMapping*> > mapping);
    TVector<UncertaintyMapping*> JsonStringToUncertaintyMapping(const TString& mappingString);
    TVector<TVector<UncertaintyMapping*> > JsonStringToUncertaintyMapping2D(const TString& mappingString);

    TVector<IElementSPtr> realVecToElemVec(const TVector<double>& vec);

    bool m_groupedDataProcessed;
};

} // namespace Operators
} // namespace Tigon

#endif // PROBLEMGENERATOR_H
