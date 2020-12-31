/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Properties/ProblemProperties.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Properties/ProblemPropertiesFactory.h>
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Representation/Container/TimeSeriesContainer.h>
#include <tigon/Representation/Distributions/IDistribution.h>

#include <algorithm>

namespace Tigon {
namespace Representation {

Problem::Problem()
    : m_definitionStatus         (Undefined)
    , m_problemPrptsDefined      (false)
    , m_dvecPrptsDefined         (false)
    , m_ovecPrptsDefined         (false)
    , m_pvecPrptsDefined         (false)
    , m_cvecPrptsDefined         (false)
    , m_uvecPrptsDefined         (false)
    , m_fvecDefined              (false)
    , m_mapsDefined              (false)
    , m_pvecDefined              (false)
    , m_dvecUncertaintiesDefined (false)
    , m_fOutUncertaintiesDefined (false)
    , m_idealDefined             (false)
    , m_nadirDefined             (false)
    , m_antiIdealDefined         (false)
    , m_boxConstraintsDefined    (false)
    , m_setGoalVecDefined        (false)
    , m_goalVecDefined           (false)
    , m_priorityVecDefined       (false)
    , m_thresholdVecDefined      (false)
{

}

Problem::Problem(const Problem &prob)
    : m_problemPrpts  (prob.m_problemPrpts)
    , m_dvecPrpts     (prob.m_dvecPrpts)
    , m_ovecPrpts     (prob.m_ovecPrpts)
    , m_pvecPrpts     (prob.m_pvecPrpts)
    , m_cvecPrpts     (prob.m_cvecPrpts)
    , m_uvecPrpts     (prob.m_uvecPrpts)
    , m_fvec          (prob.m_fvec)
    , m_fInPMap       (prob.m_fInPMap)
    , m_fOutOMap      (prob.m_fOutOMap)
    , m_fOutCMap      (prob.m_fOutCMap)
    , m_f2dMap        (prob.m_f2dMap)
    , m_f2oMap        (prob.m_f2oMap)
    , m_f2pMap        (prob.m_f2pMap)
    , m_f2cMap        (prob.m_f2cMap)
    , m_f2uMap        (prob.m_f2uMap)
    , m_definitionStatus         (prob.m_definitionStatus)
    , m_problemPrptsDefined      (prob.m_problemPrptsDefined)
    , m_dvecPrptsDefined         (prob.m_dvecPrptsDefined)
    , m_ovecPrptsDefined         (prob.m_ovecPrptsDefined)
    , m_pvecPrptsDefined         (prob.m_pvecPrptsDefined)
    , m_cvecPrptsDefined         (prob.m_cvecPrptsDefined)
    , m_uvecPrptsDefined         (prob.m_uvecPrptsDefined)
    , m_fvecDefined              (prob.m_fvecDefined)
    , m_mapsDefined              (prob.m_mapsDefined)
    , m_pvecDefined              (prob.m_pvecDefined)
    , m_dvecUncertaintiesDefined (prob.m_dvecUncertaintiesDefined)
    , m_fOutUncertaintiesDefined (prob.m_fOutUncertaintiesDefined)
    , m_idealDefined             (prob.m_idealDefined)
    , m_nadirDefined             (prob.m_nadirDefined)
    , m_antiIdealDefined         (prob.m_antiIdealDefined)
    , m_boxConstraintsDefined    (prob.m_boxConstraintsDefined)
    , m_setGoalVecDefined        (prob.m_setGoalVecDefined)
    , m_goalVecDefined           (prob.m_goalVecDefined)
    , m_priorityVecDefined       (prob.m_priorityVecDefined)
    , m_thresholdVecDefined      (prob.m_thresholdVecDefined)
    , m_d2KrigingMap             (prob.m_d2KrigingMap)
    , m_pvecExternal             (prob.m_pvecExternal)
    , m_pvecExternalGroups       (prob.m_pvecExternalGroups)
    , m_pvecExternalGroupsDependentVars (prob.m_pvecExternalGroupsDependentVars)
    , m_essentialObjectives      (prob.m_essentialObjectives)
{
    //TODO: implement IFunction::clone()
    //    m_fvec.clear();
    //    for(int i=0; i<prob.m_fvec.size(); i++) {
    //        m_fVec.push_back(prob.m_fvec.at(i)->clone());
    //    }

    m_parameterVec.clear();
    for(size_t i=0; i<prob.m_parameterVec.size(); i++) {
        m_parameterVec.push_back(prob.m_parameterVec[i]->clone());
    }

    clearPointerVector(m_dvecUncertainties);
    for(size_t i=0; i<prob.m_dvecUncertainties.size(); i++) {
        UncertaintyMapping* u = 0;
        if(prob.m_dvecUncertainties[i] != nullptr) {
            u = prob.m_dvecUncertainties[i]->clone();
        }
        m_dvecUncertainties.push_back(u);
    }

    for(size_t i=0; i<m_fOutUncertainties.size(); i++) {
        clearPointerVector(m_fOutUncertainties[i]);
    }

    m_fOutUncertainties.resize(prob.m_fOutUncertainties.size());
    for(size_t i=0; i<prob.m_fOutUncertainties.size(); i++) {
        for(size_t j=0; j<prob.m_fOutUncertainties[i].size(); j++) {
            UncertaintyMapping* u = 0;
            if(prob.m_fOutUncertainties[i][j] != nullptr) {
                u = prob.m_fOutUncertainties[i][j]->clone();
            }
            m_fOutUncertainties[i].push_back(u);
        }
    }

    m_ideal.clear();
    m_antiIdeal.clear();
    m_nadir.clear();
    for(size_t i=0; i<prob.m_ideal.size(); i++) {
        m_ideal.push_back(prob.m_ideal[i]->clone());
        m_nadir.push_back(prob.m_nadir[i]->clone());
        m_antiIdeal.push_back(prob.m_antiIdeal[i]->clone());
    }

    if(prob.m_boxConstraints) {
        m_boxConstraints = prob.m_boxConstraints->clone();
    }

    m_setGoalVec.clear();
    for(size_t i=0; i<prob.m_setGoalVec.size(); i++) {
        m_setGoalVec.push_back(prob.m_setGoalVec[i]);
    }

    m_goalVec.clear();
    for(size_t i=0; i<prob.m_goalVec.size(); i++) {
        m_goalVec.push_back(prob.m_goalVec[i]->clone());
    }

    m_priorityVec.clear();
    for(auto elem : prob.m_priorityVec) {
        m_priorityVec.push_back(elem);
    }

    m_thresholdVec.clear();
    for(size_t i=0; i<prob.m_thresholdVec.size(); i++) {
        m_thresholdVec.push_back(prob.m_thresholdVec[i]->clone());
    }

    m_pvecExternalKrigings.clear();
    for(size_t i=0; i<prob.m_pvecExternalKrigings.size(); i++) {
        KrigingSPtr u;
        if(prob.m_pvecExternalKrigings[i]) {
            u = prob.m_pvecExternalKrigings[i];
        }
        m_pvecExternalKrigings.push_back(u);
    }


    for(size_t i=0; i<m_pvecExternalIndividualData.size(); i++) {
        clearPointerVector(m_pvecExternalIndividualData[i]);
    }

    m_pvecExternalIndividualData.resize(prob.m_pvecExternalIndividualData.size());
    for(size_t i=0; i<m_pvecExternalIndividualData.size(); i++) {
        for(size_t j=0; j<m_pvecExternalIndividualData[i].size(); j++) {
            IDistribution* u = 0;
            if(prob.m_pvecExternalIndividualData[i][j] != nullptr) {
                u = prob.m_pvecExternalIndividualData[i][j]->clone();
            }
            m_pvecExternalIndividualData[i].push_back(u);
        }
    }

    m_pvecExternalGroupData.clear();
    // for each group
    for(size_t i=0; i<prob.m_pvecExternalGroupData.size(); i++) {

        TMap<int,TimeSeriesContainerSPtr> u;
        u.insert(prob.m_pvecExternalGroupData[i].begin(),prob.m_pvecExternalGroupData[i].end());
        m_pvecExternalGroupData.push_back(u);
    }
}

Problem::~Problem()
{
    m_dvecPrpts.clear();
    m_ovecPrpts.clear();
    m_pvecPrpts.clear();
    m_cvecPrpts.clear();
    m_uvecPrpts.clear();
    m_fInPMap.clear();
    m_fOutOMap.clear();
    m_fOutCMap.clear();
    m_f2dMap.clear();
    m_f2oMap.clear();
    m_f2pMap.clear();
    m_f2cMap.clear();
    m_f2uMap.clear();
    m_setGoalVec.clear();
    m_goalVec.clear();
    m_priorityVec.clear();
    m_thresholdVec.clear();

    if(!m_fvec.empty()) {
        for(size_t i=0; i<m_fvec.size(); i++) {
            m_fvec[i].reset();
        }
        m_fvec.clear();
    }

    clearPointerVector(m_dvecUncertainties);

    if(!(m_fOutUncertainties.empty())) {
        for(size_t i=0; i<m_fOutUncertainties.size(); i++) {
            clearPointerVector(m_fOutUncertainties[i]);
        }
        m_fOutUncertainties.clear();
    }

    if(!(m_parameterVec.empty())) {
        for(size_t i=0; i<m_parameterVec.size(); i++) {
            m_parameterVec[i].reset();
        }
        m_parameterVec.clear();
    }

    if(!(m_ideal.empty())) {
        for(size_t i=0; i<m_ideal.size(); i++) {
            m_ideal[i].reset();
        }
        m_ideal.clear();
    }

    if(!(m_antiIdeal.empty())) {
        for(size_t i=0; i<m_antiIdeal.size(); i++) {
            m_antiIdeal[i].reset();
        }
        m_antiIdeal.clear();
    }

    if(!(m_nadir.empty())) {
        for(size_t i=0; i<m_nadir.size(); i++) {
            m_nadir[i].reset();
        }
        m_nadir.clear();
    }

    if(m_boxConstraints) {
        m_boxConstraints.reset();
    }

    if(!m_pvecExternalIndividualData.empty()) {
        for(size_t i=0; i<m_pvecExternalIndividualData.size(); i++) {
            clearPointerVector(m_pvecExternalIndividualData[i]);
        }
        m_pvecExternalIndividualData.clear();
    }
}

ProblemSPtr Problem::clone() const
{
    return ProblemSPtr(new Problem(*this));
}

ProblemDefinitionStatus Problem::isProblemDefined() const
{
    return m_definitionStatus;
}

ProblemProperties Problem::problemProperties() const
{
    return m_problemPrpts;
}

TVector<ElementProperties> Problem::dPrpts() const
{
    return m_dvecPrpts;
}

TVector<ElementProperties> Problem::oPrpts() const
{
    return m_ovecPrpts;
}

TVector<ElementProperties> Problem::pPrpts() const
{
    return m_pvecPrpts;
}

TVector<ElementProperties> Problem::cPrpts() const
{
    return m_cvecPrpts;
}

TVector<ElementProperties> Problem::uPrpts() const
{
    return m_uvecPrpts;
}

TStringList Problem::dVecIDs() const
{
    return getIDs(m_dvecPrpts);
}

TStringList Problem::oVecIDs() const
{
    return getIDs(m_ovecPrpts);
}

TStringList Problem::pVecIDs() const
{
    return getIDs(m_pvecPrpts);
}

TStringList Problem::cVecIDs() const
{
    return getIDs(m_cvecPrpts);
}

TStringList Problem::uVecIDs() const
{
    return getIDs(m_uvecPrpts);
}

TStringList Problem::dVecNames() const
{
    return getNames(m_dvecPrpts);
}

TStringList Problem::oVecNames() const
{
    return getNames(m_ovecPrpts);
}

TStringList Problem::pVecNames() const
{
    return getNames(m_pvecPrpts);
}

TStringList Problem::cVecNames() const
{
    return getNames(m_cvecPrpts);
}

TStringList Problem::uVecNames() const
{
    return getNames(m_uvecPrpts);
}

TStringList Problem::dVecUnits() const
{
    return getUnits(m_dvecPrpts);
}

TStringList Problem::oVecUnits() const
{
    return getUnits(m_ovecPrpts);
}

TStringList Problem::pVecUnits() const
{
    return getUnits(m_pvecPrpts);
}

TStringList Problem::cVecUnits() const
{
    return getUnits(m_cvecPrpts);
}

TStringList Problem::uVecUnits() const
{
    return getUnits(m_uvecPrpts);
}

TStringList Problem::dVecDescriptions() const
{
    return getDescriptions(m_dvecPrpts);
}

TStringList Problem::oVecDescriptions() const
{
    return getDescriptions(m_ovecPrpts);
}

TStringList Problem::pVecDescriptions() const
{
    return getDescriptions(m_pvecPrpts);
}

TStringList Problem::cVecDescriptions() const
{
    return getDescriptions(m_cvecPrpts);
}

TStringList Problem::uVecDescriptions() const
{
    return getDescriptions(m_uvecPrpts);
}

TVector<ElementType> Problem::dVecTypes() const
{
    return getTypes(m_dvecPrpts);
}

TVector<ElementType> Problem::oVecTypes() const
{
    return getTypes(m_ovecPrpts);
}

TVector<ElementType> Problem::pVecTypes() const
{
    return getTypes(m_pvecPrpts);
}

TVector<ElementType> Problem::cVecTypes() const
{
    return getTypes(m_cvecPrpts);
}

TVector<ElementType> Problem::uVecTypes() const
{
    return getTypes(m_uvecPrpts);
}

TVector<OptimizationType> Problem::oVecOptTypes() const
{
    return getOptTypes(m_ovecPrpts);
}

TVector<OptimizationType> Problem::cVecOptTypes() const
{
    return getOptTypes(m_cvecPrpts);
}

TVector<bool> Problem::setGoalVector() const
{
    return m_setGoalVec;
}

TVector<IElementSPtr> Problem::goalVector() const
{
    return m_goalVec;
}

TVector<int> Problem::priorityVector() const
{
    return m_priorityVec;
}

TVector<IElementSPtr> Problem::thresholdVector() const
{
    return m_thresholdVec;
}

TVector<IElementSPtr> Problem::idealVector() const
{
    return m_ideal;
}

TVector<IElementSPtr> Problem::antiIdealVector() const
{
    return m_antiIdeal;
}

TVector<IElementSPtr> Problem::nadirVector() const
{
    return m_nadir;
}

TVector<IFunctionSPtr> Problem::functionVec() const
{
    return m_fvec;
}

TVector<TVector<int> > Problem::f2dMap() const
{
    return m_f2dMap;
}

TVector<TVector<int> > Problem::f2oMap() const
{
    return m_f2oMap;
}

TVector<TVector<int> > Problem::f2pMap() const
{
    return m_f2pMap;
}

TVector<TVector<int> > Problem::f2cMap() const
{
    return m_f2cMap;
}

TVector<TVector<int> > Problem::f2uMap() const
{
    return m_f2uMap;
}

BoxConstraintsDataSPtr Problem::boxConstraints() const
{
    return m_boxConstraints;
}

TVector<IElementSPtr> Problem::parameterVector() const
{
    return m_parameterVec;
}

TVector<UncertaintyMapping*> Problem::dVecUncertainties() const
{
    return m_dvecUncertainties;
}

TVector<TVector<UncertaintyMapping*> > Problem::funcOutUncertainties() const
{
    return m_fOutUncertainties;
}

void Problem::defineProbProperties(ProblemProperties probprp)
{
    m_problemPrpts        = probprp;
    m_problemPrptsDefined = true;
}

void Problem::defineDVecPrpts(const TVector<ElementProperties> &dvecPrpts)
{
    m_dvecPrpts = dvecPrpts;
    int n = m_dvecPrpts.size();
    for(int i=0; i<m_f2dMap.size(); i++) {
        if(m_f2dMap[i].size() != n) {
            fill(m_f2dMap[i], -1, n);
        }
    }
    m_dvecPrptsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineOVecPrpts(const TVector<ElementProperties> &ovecPrpts)
{
    m_ovecPrpts = ovecPrpts;
    int n = m_ovecPrpts.size();
    for(int i = 0; i < n; i++) {
        if(m_ovecPrpts[i].optimizationType() == NonOptimization) {
            m_ovecPrpts[i].setOptimizationType(Minimization);
        }
    }
    for(int i=0; i<m_f2oMap.size(); i++) {
        if(m_f2oMap[i].size() != n) {
            fill(m_f2oMap[i], -1, n);
        }
    }
    m_ovecPrptsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::definePVecPrpts(const TVector<ElementProperties> &pvecPrpts)
{
    m_pvecPrpts = pvecPrpts;
    int n = m_pvecPrpts.size();
    for(int i=0; i<m_f2pMap.size(); i++) {
        if(m_f2pMap[i].size() != n) {
            fill(m_f2pMap[i], -1, n);
        }
    }
    m_pvecPrptsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineCVecPrpts(const TVector<ElementProperties> &cvecPrpts)
{
    m_cvecPrpts = cvecPrpts;
    int n = m_cvecPrpts.size();
    for(int i = 0; i < n; i++) {
        if(m_cvecPrpts[i].optimizationType() == NonOptimization) {
            m_cvecPrpts[i].setOptimizationType(Minimization);
        }
    }
    for(int i=0; i<m_f2cMap.size(); i++) {
        if(m_f2cMap[i].size() != n) {
            fill(m_f2cMap[i], -1, n);
        }
    }
    m_cvecPrptsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineUVecPrpts(const TVector<ElementProperties> &uvecPrpts)
{
    m_uvecPrpts = uvecPrpts;
    int n = m_uvecPrpts.size();

    for(int i=0; i<m_f2uMap.size(); i++) {
        if(m_f2uMap[i].size() != n) {
            fill(m_f2uMap[i], -1, n);
        }
    }
    m_uvecPrptsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineDVecTypes(const TVector<ElementType> &dvecTypes)
{
    if(m_dvecPrptsDefined) {
        for(int i=0; i<m_dvecPrpts.size(); i++) {
            m_dvecPrpts[i].setType(dvecTypes[i]);
        }
    } else {
        TVector<ElementProperties> dVecPrpts =
                createElemPropVec(dvecTypes, ProbNamespace::DVec);
        defineDVecPrpts(dVecPrpts);
    }
}

void Problem::defineOVecTypes(const TVector<ElementType> &ovecTypes)
{
    if(m_ovecPrptsDefined) {
        for(int i=0; i<m_ovecPrpts.size(); i++) {
            m_ovecPrpts[i].setType(ovecTypes[i]);
        }
    } else {
        TVector<ElementProperties> oVecPrpts =
                createElemPropVec(ovecTypes, ProbNamespace::OVec);
        defineOVecPrpts(oVecPrpts);
    }
}

void Problem::definePVecTypes(const TVector<ElementType> &pvecTypes)
{
    if(m_pvecPrptsDefined) {
        for(int i=0; i<m_pvecPrpts.size(); i++) {
            m_pvecPrpts[i].setType(pvecTypes[i]);
        }
    } else {
        TVector<ElementProperties> pVecPrpts =
                createElemPropVec(pvecTypes, ProbNamespace::PVec);
        definePVecPrpts(pVecPrpts);
    }
}

void Problem::defineCVecTypes(const TVector<ElementType> &cvecTypes)
{
    if(m_cvecPrptsDefined) {
        for(int i=0; i<m_cvecPrpts.size(); i++) {
            m_cvecPrpts[i].setType(cvecTypes[i]);
        }
    } else {
        TVector<ElementProperties> cVecPrpts =
                createElemPropVec(cvecTypes, ProbNamespace::CVec);
        defineCVecPrpts(cVecPrpts);
    }
}

void Problem::defineUVecTypes(const TVector<ElementType> &uvecTypes)
{
    if(m_uvecPrptsDefined) {
        for(int i=0; i<m_uvecPrpts.size(); i++) {
            m_uvecPrpts[i].setType(uvecTypes[i]);
        }
    } else {
        TVector<ElementProperties> uVecPrpts =
                createElemPropVec(uvecTypes, ProbNamespace::UVec);
        defineOVecPrpts(uVecPrpts);
    }
}

void Problem::redefineDVarType(int idx, ElementType dvarType)
{
    if(isInRange(idx, m_dvecPrpts.size())) {
        m_dvecPrpts[idx].setType(dvarType);
        if(m_boxConstraintsDefined) {
            if(isInRange(idx, m_boxConstraints->size())) {
                IElement bound = m_boxConstraints->lowerBound(idx);
                bound.defineType(dvarType);
                m_boxConstraints->defineLowerBound(idx, bound);
                bound = m_boxConstraints->upperBound(idx);
                bound.defineType(dvarType);
                m_boxConstraints->defineUpperBound(idx, bound);
            } else {
                // ERROR - index out of bounds of box constraints
            }
        }

    } else {
        // ERROR - index out of bounds of decision variables vector
    }
}

void Problem::redefineParamType(int idx, ElementType paramType)
{
    if(isInRange(idx, m_pvecPrpts.size())) {
        m_pvecPrpts[idx].setType(paramType);
        if(m_pvecDefined) {
            if(isInRange(idx, m_parameterVec.size())) {
                m_parameterVec[idx]->defineType(paramType);
            } else {
                // ERROR - index out of bounds of parameters vector
            }
        }
    } else {
        // ERROR - index out of bounds of parameters vector
    }
}

void Problem::defineOVecOptimizationTypes(const TVector<OptimizationType> &ovecOptTypes)
{
    if(m_ovecPrptsDefined) {
        for(int i=0; i<m_ovecPrpts.size(); i++) {
            if(ovecOptTypes[i] != NonOptimization) {
                m_ovecPrpts[i].setOptimizationType(ovecOptTypes[i]);
            }
        }
    } else {
        // Don't do anything
    }
}

void Problem::defineCVecOptimizationTypes(const TVector<OptimizationType> &cvecOptTypes)
{
    if(m_cvecPrptsDefined) {
        for(int i=0; i<m_cvecPrpts.size(); i++) {
            if(cvecOptTypes[i] != NonOptimization) {
                m_cvecPrpts[i].setOptimizationType(cvecOptTypes[i]);
            }
        }
    } else {
        // Don't do anything
    }

}

void Problem::redefineObjOptimizationType(int idx, OptimizationType optType)
{
    if(isInRange(idx, m_ovecPrpts.size()) && (optType != NonOptimization)) {
        m_ovecPrpts[idx].setOptimizationType(optType);
    } else {
        // ERROR - index out of bounds of objectives vector
    }
}

void Problem::redefineCnstOptimizationType(int idx, OptimizationType optType)
{
    if(isInRange(idx, m_cvecPrpts.size()) && (optType != NonOptimization)) {
        m_cvecPrpts[idx].setOptimizationType(optType);
    } else {
        // ERROR - index out of bounds of constraints vector
    }
}

void Problem::defineFunctionVec(const TVector<IFunctionSPtr> &fvec)
{
    for(int i=0; i<fvec.size(); i++) {
        appendFunction(fvec[i]);
    }

    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineF2dMap(const TVector<TVector<int> >& map)
{
    m_f2dMap = map;
    m_mapsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineF2oMap(const TVector<TVector<int> >& map)
{
    m_f2oMap = map;
    m_mapsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineF2pMap(const TVector<TVector<int> >& map)
{
    m_f2pMap = map;
    m_mapsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineF2cMap(const TVector<TVector<int> >& map)
{
    m_f2cMap = map;
    m_mapsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineF2uMap(const TVector<TVector<int> > &map)
{
    m_f2uMap = map;
    m_mapsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineSetGoalVector(const TVector<bool> &setGoals)
{
    m_setGoalVec = setGoals;
    m_setGoalVecDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineGoalVector(const TVector<IElementSPtr> &goals)
{
    m_goalVec = goals;
    m_goalVecDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::redefineSetGoal(int idx, bool setGoal)
{
    if(isInRange(idx, m_setGoalVec.size())) {
        m_setGoalVec[idx] = setGoal;
    }
}

void Problem::redefineGoal(int idx, IElementSPtr goal)
{
    if(isInRange(idx, m_goalVec.size())) {
        m_goalVec[idx]->defineValue(goal->value());
    }
}

void Problem::definePriorityVector(const TVector<int> &priorities)
{
    m_priorityVec = priorities;
    m_priorityVecDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::redefinePriority(int idx, int priority)
{
    if(isInRange(idx, m_priorityVec.size())) {
        m_priorityVec[idx] = priority;
    }
}

void Problem::defineThresholdVector(const TVector<IElementSPtr> &thold)
{
    m_thresholdVec = thold;
    m_thresholdVecDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::redefineThreshold(int idx, IElementSPtr thold)
{
    if(isInRange(idx, m_thresholdVec.size())) {
        m_thresholdVec[idx]->defineValue(thold->value());
    }
}

void Problem::defineIdealVector(const TVector<IElementSPtr> &ideal)
{
    m_ideal = ideal;
    m_idealDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineAntiIdealVector(const TVector<IElementSPtr> &antiIdeal)
{
    m_antiIdeal = antiIdeal;
    m_antiIdealDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineNadirVector(const TVector<IElementSPtr> &nadir)
{
    m_nadir = nadir;
    m_nadirDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineParameterVector(const TVector<IElementSPtr> &pvec)
{
    if(!(m_parameterVec.empty())) {
        for(int i=0; i<m_parameterVec.size(); i++) {
            m_parameterVec[i].reset();
        }
    }
    m_parameterVec = pvec;
    m_pvecDefined  = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineDVecUncertainties(const TVector<UncertaintyMapping*> &umaps)
{
    clearPointerVector(m_dvecUncertainties);
    m_dvecUncertainties = umaps;
    m_dvecUncertaintiesDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::redefineDVarUncertainty(int idx, UncertaintyMapping* umap)
{
    if(isInRange(idx, m_dvecUncertainties.size())) {
        if(m_dvecUncertainties[idx] != nullptr) {
            delete m_dvecUncertainties[idx];
        }
        m_dvecUncertainties[idx] = umap;
    }
}

void Problem::defineFuncOutUncertainties(const TVector<TVector<UncertaintyMapping*> > &umaps)
{
    if(!(m_fOutUncertainties.empty())) {
        for(int i=0; i<m_fOutUncertainties.size(); i++) {
            clearPointerVector(m_fOutUncertainties[i]);
        }
    }
    m_fOutUncertainties = umaps;
    m_fOutUncertaintiesDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::redefineFuncOutputUncertainty(int funcIdx, int outputIdx,
                                            UncertaintyMapping* umap)
{
    if(isInRange(funcIdx, m_fOutUncertainties.size())) {
        if(isInRange(outputIdx, m_fOutUncertainties[funcIdx].size())) {
            if(m_fOutUncertainties[funcIdx][outputIdx] != nullptr) {
                delete m_fOutUncertainties[funcIdx][outputIdx];
            }
            m_fOutUncertainties[funcIdx][outputIdx] = umap;
        }
    }
}

void Problem::defineExternalParameters(const TVector<bool> &extpar)
{
    if(m_parameterVec.empty()) {
        return; // Error - there is no parameters defined
    }

    if(m_parameterVec.size() == extpar.size()) {
        m_pvecExternal = extpar;
        m_definitionStatus = UnprocessedChanges;
    }
}

bool Problem::updateIdeal(const TVector<IElementSPtr> &newVec)
{
    bool ret = false;
    if(newVec.size() == m_ideal.size()) {
        for(int i=0; i<newVec.size(); i++) {
            if(*(newVec[i]) < *(m_ideal[i])) {
                m_ideal[i] = newVec[i]->clone();
                ret = true;
            }
        }
    }
    return ret;
}

bool Problem::updateAntiIdeal(const TVector<IElementSPtr> &newVec)
{
    bool ret = false;
    if(newVec.size() == m_antiIdeal.size()) {
        for(int i=0; i<newVec.size(); i++) {
            if(*(newVec[i]) > *(m_antiIdeal[i])) {
                m_antiIdeal[i] = newVec[i]->clone();
                ret = true;
            }
        }
    }
    return ret;
}

bool Problem::updateNadir(const TVector<IElementSPtr> &newVec)
{
    bool ret = false;
    if(newVec.size() == m_nadir.size()) {
        for(int i=0; i<newVec.size(); i++) {
            if(*(newVec[i]) > *(m_nadir[i])) {
                m_nadir[i] = newVec[i]->clone();
                ret = true;
            }
        }
    }
    return ret;
}

void Problem::defineBoxConstraints(BoxConstraintsDataSPtr box)
{
    m_boxConstraints = box;
    m_boxConstraintsDefined = true;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::appendFunction(IFunctionSPtr func,
                             const TVector<int> &inParamIdx,
                             const TVector<int> &outConstrIdx,
                             const TVector<int> &outObjIdx,
                             const TVector<int> &outUnuIdx)
{
    int nInputs  = func->TP_nInputs();
    int nOutputs = func->TP_nOutputs();

    TVector<bool> inParamBool(nInputs, false);
    for(int i=0; i<inParamIdx.size(); i++) {
        if(!isInRange(inParamIdx[i], nInputs)) {
            return;
        } else {
            inParamBool[inParamIdx[i]] = true;
        }
    }

    TVector<bool> outObjBool(nOutputs, false);
    if(outObjIdx.empty()) {
        fill(outObjBool, true);
    } else {
        for(int i=0; i<outObjIdx.size(); i++) {
            if(!isInRange(outObjIdx[i], nOutputs)) {
                return;
            } else {
                outObjBool[outObjIdx[i]] = true;
            }
        }
    }

    TVector<bool> outConstrBool(nOutputs, false);
    for(int i=0; i<outConstrIdx.size(); i++) {
        if(!isInRange(outConstrIdx[i], nOutputs)) {
            return;
        } else {
            outConstrBool[outConstrIdx[i]] = true;
        }
    }

    if(outUnuIdx.size() !=0) {
        TVector<bool> outUnuBool(nOutputs, false);
        for(int i=0; i<outUnuIdx.size(); i++) {
            if(!isInRange(outUnuIdx[i], nOutputs)) {
                return;
            } else {
                outObjBool[outUnuIdx[i]] = false;
                outConstrBool[outUnuIdx[i]] = false;
                outUnuBool[outUnuIdx[i]] = true;
            }
        }
    }

    m_fvec.push_back(func);
    m_fInPMap.push_back(inParamBool);
    m_fOutOMap.push_back(outObjBool);
    m_fOutCMap.push_back(outConstrBool);

    m_fvecDefined = true;
    m_mapsDefined = false;
    m_definitionStatus = UnprocessedChanges;
}

ProblemDefinitionStatus Problem::processProblemDefinition()
{
    if(m_definitionStatus == FullyDefined) {
        return m_definitionStatus;
    }

    // a problem must include a function to be defined
    if(!m_fvecDefined) {
        m_definitionStatus = IllDefinedFunctionVec;
        return m_definitionStatus;
    }

    int nFuncs = static_cast<int>(m_fvec.size());

    if(!m_mapsDefined) {
        /// construct the maps and set all the definition flags to false in
        /// order to reconstruct them if necessary.
        // Find the dimension of every vector, and create lists of IDs
        TStringList dVars, oVars, params, constrs, uVars;
        for(int i=0; i<nFuncs; i++) {
            TVector<ElementProperties> inputs  = m_fvec[i]->inputPrpts();
            TVector<ElementProperties> outputs = m_fvec[i]->outputPrpts();

            for(int j=0; j<inputs.size(); j++) {
                if(m_fInPMap[i][j]) { // the input is a parameter
                    // check if the parameter already exists
                    if(indexOf(params, inputs[j].ID()) == -1) {
                        params.push_back(inputs[j].ID());
                    }
                }
                else { // the input is a decision variable
                    // check if the decision variable already exists
                    if(indexOf(dVars, inputs[j].ID()) == -1) {
                        dVars.push_back(inputs[j].ID());
                    }
                }
            }

            for(int j=0; j<outputs.size(); j++) {
                if( m_fOutOMap[i][j] || m_fOutCMap[i][j] ) {
                    if(m_fOutOMap[i][j]) { // the output is an objective
                        // check if the objective already exists
                        if(indexOf(oVars, outputs[j].ID()) == -1) {
                            oVars.push_back(outputs[j].ID());
                        }
                    }
                    if(m_fOutCMap[i][j]) { // the output is a constraint
                        // check if the constraint already exists
                        if(indexOf(constrs, outputs[j].ID()) == -1) {
                            constrs.push_back(outputs[j].ID());
                        }
                    }
                } else  { // unused output
                    // check if the unused output already exists
                    if(indexOf(uVars, outputs[j].ID()) == -1) {
                        uVars.push_back(outputs[j].ID());
                    }
                }
            }
        }

        // Generate the maps
        fill(m_f2dMap, TVector<int>(dVars.size(),   -1), nFuncs);
        fill(m_f2oMap, TVector<int>(oVars.size(),   -1), nFuncs);
        fill(m_f2pMap, TVector<int>(params.size(),  -1), nFuncs);
        fill(m_f2cMap, TVector<int>(constrs.size(), -1), nFuncs);
        fill(m_f2uMap, TVector<int>(uVars.size(),   -1), nFuncs);
        for(int i=0; i<nFuncs; i++) {
            // inputs
            TVector<ElementProperties> inputs  = m_fvec[i]->inputPrpts();
            for(int j=0; j<inputs.size(); j++) {
                // decision variables
                for(int k=0; k<dVars.size(); k++) {
                    if(dVars[k] == inputs[j].ID()) {
                        m_f2dMap[i][k] = j;
                    }

                }

                // parameters
                for(int k=0; k<params.size(); k++) {
                    if(params[k] == inputs[j].ID()) {
                        m_f2pMap[i][k] = j;
                    }
                }
            }

            // outputs
            TVector<ElementProperties> outputs = m_fvec[i]->outputPrpts();
            for(int j=0; j<outputs.size(); j++) {

                for(int k=0; k<oVars.size(); k++) {
                    if(oVars[k] == outputs[j].ID()) {
                        m_f2oMap[i][k] = j;
                    }

                }

                for(int k=0; k<uVars.size(); k++) {
                    if(uVars[k] == outputs[j].ID()) {
                        m_f2uMap[i][k] = j;
                    }

                }

                for(int k=0; k<constrs.size(); k++) {
                    if(constrs[k] == outputs[j].ID()) {
                        m_f2cMap[i][k] = j;
                    }
                }
            }
        }
        m_mapsDefined              = true;
        m_dvecPrptsDefined         = false;
        m_ovecPrptsDefined         = false;
        m_pvecPrptsDefined         = false;
        m_cvecPrptsDefined         = false;
        m_uvecPrptsDefined         = false;
        m_pvecDefined              = false;
        m_dvecUncertaintiesDefined = false;
        m_fOutUncertaintiesDefined = false;
        m_idealDefined             = false;
        m_nadirDefined             = false;
        m_antiIdealDefined         = false;
        m_boxConstraintsDefined    = false;
        m_setGoalVecDefined        = false;
        m_goalVecDefined           = false;
        m_priorityVecDefined       = false;
        m_thresholdVecDefined      = false;
    }

    /// error checks for function vec and maps sizes
    if(m_f2dMap.size() != nFuncs) {
        m_definitionStatus = IllDefinedDVecMaps;
        return m_definitionStatus;
    }
    if(m_f2oMap.size() != nFuncs) {
        m_definitionStatus = IllDefinedOVecMaps;
        return m_definitionStatus;
    }
    if(m_f2pMap.size() != nFuncs) {
        m_definitionStatus = IllDefinedPVecMaps;
        return m_definitionStatus;
    }
    if(m_f2cMap.size() != nFuncs) {
        m_definitionStatus = IllDefinedCVecMaps;
        return m_definitionStatus;
    }
    if(m_f2uMap.size() != nFuncs) {
        m_definitionStatus = IllDefinedUVecMaps;
        return m_definitionStatus;
    }

    int nDVars   = m_f2dMap[0].size();
    int nOVars   = m_f2oMap[0].size();
    int nParams  = m_f2pMap[0].size();
    int nConstrs = m_f2cMap[0].size();
    int nUVars   = m_f2uMap[0].size();

    // Check for consistency between all the functions' maps
    for(int i=1; i<nFuncs; i++) {
        if(m_f2dMap[i].size() != nDVars) {
            m_definitionStatus = IllDefinedDVecMaps;
            return m_definitionStatus;
        }
        if(m_f2oMap[i].size() != nOVars) {
            m_definitionStatus = IllDefinedOVecMaps;
            return m_definitionStatus;
        }
        if(m_f2pMap[i].size() != nParams) {
            m_definitionStatus = IllDefinedPVecMaps;
            return m_definitionStatus;
        }
        if(m_f2cMap[i].size() != nConstrs) {
            m_definitionStatus = IllDefinedCVecMaps;
            return m_definitionStatus;
        }
        if(m_f2uMap[i].size() != nUVars) {
            m_definitionStatus = IllDefinedUVecMaps;
            return m_definitionStatus;
        }
    }

    // if properties vector defined, error checks for each map and vector sizes
    if(m_dvecPrptsDefined & (m_dvecPrpts.size() != nDVars)) {
        m_definitionStatus = IllDefinedDVecMaps;
        return m_definitionStatus;
    } else if(m_ovecPrptsDefined & (m_ovecPrpts.size() != nOVars)) {
        m_definitionStatus = IllDefinedOVecMaps;
        return m_definitionStatus;
    } else if(m_pvecPrptsDefined & (m_pvecPrpts.size() != nParams)) {
        m_definitionStatus = IllDefinedPVecMaps;
        return m_definitionStatus;
    } else if(m_cvecPrptsDefined & (m_cvecPrpts.size() != nConstrs)) {
        m_definitionStatus = IllDefinedCVecMaps;
        return m_definitionStatus;
    } else if(m_uvecPrptsDefined & (m_uvecPrpts.size() != nUVars)) {
        m_definitionStatus = IllDefinedUVecMaps;
        return m_definitionStatus;
    }

    /// if maps (and vectors properties) are empty, assume all inputs are
    /// decision variables, and all outputs are objectives

    // Check the problem consists of at least a single variable and a single
    // objective. If not, create the maps and properties from the properties
    // defined in the functions

    // inputs
    if(nDVars == 0) {
        if (nParams==0) {

            // Both nDVars and nParams are zero

            for(int i=0; i<nFuncs; i++) {
                TVector<ElementProperties> fInPrprts = m_fvec[i]->inputPrpts();
                BoxConstraintsDataSPtr fBox = m_fvec[i]->boxConstraints();
                for(int j=0; j<fInPrprts.size(); j++) {
                    bool prptyExist = false;
                    for(int k=0; k<m_dvecPrpts.size(); k++) {
                        if(fInPrprts[j].ID() == m_dvecPrpts[k].ID()) {
                            prptyExist = true;
                            if(m_f2dMap[i].size() == k) {
                                m_f2dMap[i].push_back(j);
                            } else if(m_f2dMap[i].size() > k) {
                                m_f2dMap[i][k] = j;
                            } else {
                                // ERROR - every input was supposed to appear
                                // already.
                            }
                            break;
                        }
                    }
                    if(!prptyExist) {
                        // The jth input property is appended to the decision
                        // vector properties, and is mapped to the jth input
                        m_dvecPrpts.push_back(fInPrprts[j]);
                        m_f2dMap[i].push_back(j);
                    }
                }
            }
            nDVars = m_dvecPrpts.size();
            m_dvecPrptsDefined = true;
            m_pvecPrptsDefined = true;


        } else {
            m_definitionStatus = IllDefinedDVecMaps;
            return m_definitionStatus;
        }
    }

    // outputs
    if(nOVars == 0) {
        if( (nConstrs==0) && (nUVars==0) ) {

            // nOVars, nConstrs and nUVars are zero

            for(int i=0; i<nFuncs; i++) {
                TVector<ElementProperties> fOutPrprts = m_fvec[i]->outputPrpts();
                for(int j=0; j<fOutPrprts.size(); j++) {
                    bool prptyExist = false;
                    for(int k=0; k<m_ovecPrpts.size(); k++) {
                        if(fOutPrprts[j].ID() == m_ovecPrpts[k].ID()) {
                            prptyExist = true;
                            if(m_f2oMap[i].size() == k) {
                                m_f2oMap[i].push_back(j);
                            } else if(m_f2oMap[i].size() > k) {
                                m_f2oMap[i][k] = j;
                            } else {
                                // ERROR - every output was supposed to appear
                                // already.
                            }
                            break;
                        }
                    }
                    if(!prptyExist) {
                        // The jth input property is appended to the decision
                        // vector properties, and is mapped to the jth input
                        m_ovecPrpts.push_back(fOutPrprts[j]);
                        m_f2oMap[i].push_back(j);
                    }
                }
            }
            nOVars = m_ovecPrpts.size();
            m_ovecPrptsDefined = true;
            m_cvecPrptsDefined = true;
            m_uvecPrptsDefined = true;

        } else {
            m_definitionStatus = IllDefinedOVecMaps;
            return m_definitionStatus;
        }
    }

    /// verify the maps are correct (all function inputs are connected to the
    /// decision variables and parameters, and all objectives and constraints
    /// are connected to function outputs.)
    // inputs
    for(int i=0; i<nFuncs; i++) {
        int nInputs = m_fvec[i]->TP_nInputs();
        TVector<int> dMap = m_f2dMap[i];
        TVector<int> pMap = m_f2pMap[i];

        for(int j=0; j<nInputs; j++) {
            // number of connections
            int nDVarsCon = vectorCount(dMap, j);
            int nParamCon = vectorCount(pMap, j);

            // the order of checking which input to blame for wrong connections
            // is decision variables and then the parameters

            if( ((nDVarsCon==1) && (nParamCon==0)) ||
                    ((nDVarsCon==0) && (nParamCon==1))   ) {
                // input is properly connected
            } else if(nParamCon==0) {
                m_definitionStatus = IllDefinedDVecMaps;
                return m_definitionStatus;
            } else {
                m_definitionStatus = IllDefinedPVecMaps;
                return m_definitionStatus;
            }
        }
    }

    //outputs
    for(int i=0; i<nOVars; i++) {
        bool exists = false;
        for(int j=0; j<nFuncs; j++) {
            if(m_f2oMap[j][i] >= 0) {
                if(!exists) {
                    exists = true;
                } else {
                    m_definitionStatus = IllDefinedOVecMaps;
                    return m_definitionStatus;
                }
            }
        }
    }
    for(int i=0; i<nConstrs; i++) {
        bool exists = false;
        for(int j=0; j<nFuncs; j++) {
            if(m_f2cMap[j][i] >= 0) {
                if(!exists) {
                    exists = true;
                } else {
                    m_definitionStatus = IllDefinedCVecMaps;
                    return m_definitionStatus;
                }
            }
        }
    }
    for(int i=0; i<nUVars; i++) {
        bool exists = false;
        for(int j=0; j<nFuncs; j++) {
            if(m_f2uMap[j][i] >= 0) {
                if(!exists) {
                    exists = true;
                } else {
                    m_definitionStatus = IllDefinedUVecMaps;
                    return m_definitionStatus;
                }
            }
        }
    }

    // create null property vectors
    TVector<ElementProperties> dPrpts(nDVars);
    TVector<ElementProperties> oPrpts(nOVars);
    TVector<ElementProperties> pPrpts(nParams);
    TVector<ElementProperties> cPrpts(nConstrs);
    TVector<ElementProperties> uPrpts(nUVars);

    // populate the vectors with the properties from the functions according
    // to the maps
    for(int i=0; i<m_fvec.size(); i++) {
        TVector<ElementProperties> inputs  = m_fvec[i]->inputPrpts();
        TVector<ElementProperties> outputs = m_fvec[i]->outputPrpts();

        //inputs
        TVector<int> map = m_f2dMap[i];
        for(int j=0; j<nDVars; j++) {
            if(map[j] >= 0) {
                dPrpts[j] = inputs[map[j]];
            }
        }

        //outputs
        map = m_f2oMap[i];
        for(int j=0; j<nOVars; j++) {
            if(map[j] >= 0) {
                oPrpts[j] = outputs[map[j]];
            }
        }

        //parameters
        map = m_f2pMap[i];
        for(int j=0; j<nParams; j++) {
            if(map[j] >= 0) {
                pPrpts[j] = inputs[map[j]];
            }
        }

        //constraints
        map = m_f2cMap[i];
        for(int j=0; j<nConstrs; j++) {
            if(map[j] >= 0) {
                cPrpts[j] = outputs[map[j]];
            }
        }

        // not used outputs
        map = m_f2uMap[i];
        for(int j=0; j<nUVars; j++) {
            if(map[j] >= 0) {
                uPrpts[j] = outputs[map[j]];
            }
        }
    }

    // varify all the properties are defined
    for(int j=0; j<nDVars; j++) {
        ElementProperties el = dPrpts[j];
        if(el.ID().empty()) {
            m_definitionStatus = IllDefinedDVecMaps;
            return m_definitionStatus;
        }
    }
    for(int j=0; j<nOVars; j++) {
        ElementProperties el = oPrpts[j];
        if(el.ID().empty()) {
            m_definitionStatus = IllDefinedOVecMaps;
            return m_definitionStatus;
        }
    }
    for(int j=0; j<nParams; j++) {
        ElementProperties el = pPrpts[j];
        if(el.ID().empty()) {
            m_definitionStatus = IllDefinedPVecMaps;
            return m_definitionStatus;
        }
    }
    for(int j=0; j<nConstrs; j++) {
        ElementProperties el = cPrpts[j];
        if(el.ID().empty()) {
            m_definitionStatus = IllDefinedCVecMaps;
            return m_definitionStatus;
        }
    }
    for(int j=0; j<nUVars; j++) {
        ElementProperties el = uPrpts[j];
        if(el.ID().empty()) {
            m_definitionStatus = IllDefinedUVecMaps;
            return m_definitionStatus;
        }
    }

    /// If vector properties were not defined, use the ones from the functions
    if(!m_dvecPrptsDefined) {
        m_dvecPrpts.swap(dPrpts);
        m_dvecPrptsDefined = true;
    }
    if(!m_ovecPrptsDefined) {
        m_ovecPrpts.swap(oPrpts);
        m_ovecPrptsDefined = true;
    }
    if(!m_pvecPrptsDefined) {
        m_pvecPrpts.swap(pPrpts);
        m_pvecPrptsDefined = true;
    }
    if(!m_cvecPrptsDefined) {
        m_cvecPrpts.swap(cPrpts);
        m_cvecPrptsDefined = true;
    }
    if(!m_uvecPrptsDefined) {
        m_uvecPrpts.swap(uPrpts);
        m_uvecPrptsDefined = true;
    }

    /// check size / construct boxConstraints for decision variables
    /// according to dvecproperties
    if(m_boxConstraintsDefined) {
        if(m_boxConstraints->size() != nDVars) {
            m_definitionStatus = IllDefinedBoxConstraints;
            return m_definitionStatus;
        } else {
            // enforce types
            TVector<IElement> lbs = createIElementVector(m_dvecPrpts);
            TVector<IElement> ubs = createIElementVector(m_dvecPrpts);
            for(int i=0; i<nDVars; i++) {
                lbs[i].defineValue(m_boxConstraints->lowerBound(i));
                ubs[i].defineValue(m_boxConstraints->upperBound(i));
            }
            m_boxConstraints->defineLowerBounds(lbs);
            m_boxConstraints->defineUpperBounds(ubs);
        }
    } else {
        m_boxConstraints =
                BoxConstraintsDataSPtr(new BoxConstraintsData(m_dvecPrpts));

        // assign limits from the functions
        for(int i=0; i<m_fvec.size(); i++) {
            IFunctionSPtr func = m_fvec[i];
            TVector<int> map = m_f2dMap[i];
            for(int j=0; j<nDVars; j++) {
                if(map[j] >= 0) {
                    BoxConstraintsDataSPtr fbox = func->boxConstraints();
                    if(fbox) {
                        m_boxConstraints->defineLowerBound
                                (j, fbox->lowerBound(map[j]));
                        m_boxConstraints->defineUpperBound
                                (j, fbox->upperBound(map[j]));
                    }
                }
            }
        }
        m_boxConstraintsDefined = true;
    }

    /// check size / construct ideal, anti-ideal, nadir according to ovecproperties
    // ideal
    if(m_idealDefined) {
        if(m_ideal.size() != nOVars) {
            m_definitionStatus = IllDefinedIdealVec;
            return m_definitionStatus;
        }
        for(int i=0; i<nOVars; i++) {
            m_ideal[i]->defineType(m_ovecPrpts[i].type());
        }
    } else {
        m_ideal.clear();
        for(int i=0; i<nOVars; i++) {
            m_ideal.push_back(IElementSPtr(new IElement(m_ovecPrpts[i].type())));
        }
        Tigon::resetToMax(m_ideal);
        m_idealDefined = true;
    }

    // anti-ideal
    if(m_antiIdealDefined) {
        if(m_antiIdeal.size() != nOVars) {
            m_definitionStatus = IllDefinedAntiIdealVec;
            return m_definitionStatus;
        }
        for(int i=0; i<nOVars; i++) {
            m_antiIdeal[i]->defineType(m_ovecPrpts[i].type());
        }
    } else {
        m_antiIdeal.clear();
        for(int i=0; i<nOVars; i++) {
            m_antiIdeal.push_back(IElementSPtr(new IElement(m_ovecPrpts[i].type())));
        }
        Tigon::resetToMin(m_antiIdeal);
        m_antiIdealDefined = true;
    }

    // nadir
    if(m_nadirDefined) {
        if(m_nadir.size() != nOVars) {
            m_definitionStatus = IllDefinedNadirVec;
            return m_definitionStatus;
        }
        for(int i=0; i<nOVars; i++) {
            m_nadir[i]->defineType(m_ovecPrpts[i].type());
        }
    } else {
        m_nadir.clear();
        for(int i=0; i<nOVars; i++) {
            m_nadir.push_back(IElementSPtr(new IElement(m_ovecPrpts[i].type())));
        }
        Tigon::resetToMin(m_nadir);
        m_nadirDefined = true;
    }

    /// check size/construct setGoalvec according to ovecproperties
    // setGoals are set to false by default
    if(m_setGoalVecDefined) {
        if(m_setGoalVec.size() != nOVars) {
            m_definitionStatus = IllDefinedSetGoalVec;
            return m_definitionStatus;
        }
    } else {
        m_setGoalVec.clear();
        for(int i=0; i<nOVars; i++) {
            m_setGoalVec.push_back(false);
        }
        m_setGoalVecDefined = true;
    }

    /// check size/construct goalvec according to ovecproperties
    // goals are set to minimum value by default (as low as possible)
    if(m_goalVecDefined) {
        if(m_goalVec.size() != nOVars) {
            m_definitionStatus = IllDefinedGoalVec;
            return m_definitionStatus;
        }
        for(int i=0; i<nOVars; i++) {
            m_goalVec[i]->defineType(m_ovecPrpts[i].type());
        }
    } else {
        m_goalVec.clear();
        for(int i=0; i<nOVars; i++) {
            m_goalVec.push_back(IElementSPtr(new IElement(m_ovecPrpts[i].type())));
        }
        Tigon::resetToMin(m_goalVec);
        m_goalVecDefined = true;
    }

    /// check size/construct priorityvec according to ovecproperties
    // priorities are set to minimum (1) value by default
    if(m_priorityVecDefined) {
        if(m_priorityVec.size() != nOVars) {
            m_definitionStatus = IllDefinedPriorityVec;
            return m_definitionStatus;
        }
    } else {
        m_priorityVec.clear();
        for(int i=0; i<nOVars; i++) {
            m_priorityVec.push_back(1);
        }
        m_priorityVecDefined = true;
    }

    /// check size/construct threshold vec according to cvecproperties
    // constraints are set to maximum value by default (never active)
    if(m_thresholdVecDefined) {
        if(m_thresholdVec.size() != nConstrs) {
            m_definitionStatus = IllDefinedThresholdVec;
            return m_definitionStatus;
        }
        for(int i=0; i<nConstrs; i++) {
            m_thresholdVec[i]->defineType(m_cvecPrpts[i].type());
        }
    } else {
        m_thresholdVec.clear();
        for(int i=0; i<nConstrs; i++) {
            m_thresholdVec.push_back(IElementSPtr(new IElement(m_cvecPrpts[i].type())));
        }
        Tigon::resetToMax(m_thresholdVec);
        m_thresholdVecDefined = true;
    }

    /// \todo check if it is necessary to define uncertainty mappings for
    /// strategy variables

    /// check size / construct uncertainty mappings vec
    /// according to dvecproperties
    if(m_dvecUncertaintiesDefined) {
        if(m_dvecUncertainties.size() != nDVars) {
            m_definitionStatus = IllDefinedUncertaintyVec;
            return m_definitionStatus;
        }
    } else {
        m_dvecUncertainties.clear();
        m_dvecUncertainties.resize(nDVars);
        m_dvecUncertaintiesDefined = true;
    }

    /// check size / construct uncertainty mappings vec according to function vec
    if(m_fOutUncertaintiesDefined) {
        if(m_fOutUncertainties.size() != nFuncs) {
            m_definitionStatus = IllDefinedFuncOutUncertaintyVec;
            return m_definitionStatus;
        } else {
            for(int i=0; i<nFuncs; i++) {
                if(m_fOutUncertainties[i].size() != m_fvec[i]->TP_nOutputs()) {
                    m_definitionStatus = IllDefinedFuncOutUncertaintyVec;
                    return m_definitionStatus;
                }
            }
        }
    } else {
        m_fOutUncertainties.clear();
        m_fOutUncertainties.resize(nFuncs);
        for(int i=0; i<nFuncs; i++) {
            m_fOutUncertainties[i].resize(m_fvec[i]->TP_nOutputs());
        }
        m_fOutUncertaintiesDefined = true;
    }

    /// check size / construct parameter vector according to pvecproperties
    if(m_pvecDefined) {
        if(m_parameterVec.size() != nParams) {
            m_definitionStatus = IllDefinedParameterVec;
            return m_definitionStatus;
        }
        for(int i=0; i<nParams; i++) {
            m_parameterVec[i]->defineType(m_pvecPrpts[i].type());
        }
    } else {
        m_parameterVec.clear();
        m_parameterVec = createIElementSPtrVector(m_pvecPrpts);
        m_pvecDefined = true;
    }

    /// ****************************************
    /// Deal with individual external parameters
    /// ****************************************

    if(m_pvecExternal.size() != m_parameterVec.size()) {
        // if m_pvecExternal has the wrong size, resize it and initialize to false.
        fill(m_pvecExternal, false, m_parameterVec.size());
    }
    // number of external parameters
    int nExternal = std::count(m_pvecExternal.cbegin(), m_pvecExternal.cend(), true);

    // If the m_pvecExternalKrigings has the wrong size, resize it.
    if(m_pvecExternalKrigings.size() != nExternal) {
        m_pvecExternalKrigings.clear();
        m_pvecExternalKrigings.resize(nExternal);
    } else {
        // If the m_pvecExternalKrigings has the correct size and are defined
        // remove the prior distribution
        int ct = 0;
        for(size_t i=0; i<m_parameterVec.size(); i++) {
            if(m_pvecExternal[i]) {
                if(m_pvecExternalKrigings[ct]) { // is kriging defined
                    m_parameterVec[i]->defineDist(nullptr); // remove prior
                }
                ++ct;
            }
        }
    }

    // If the m_pvecExternalIndividualData has the wrong size, resize it
    if(m_pvecExternalIndividualData.size() != m_parameterVec.size()) {
        m_pvecExternalIndividualData.clear();
        m_pvecExternalIndividualData.resize(m_parameterVec.size());
    }
    // Attach the prior
    for(size_t i=0; i<m_parameterVec.size(); i++) {
        if(m_pvecExternalIndividualData[i].empty() && m_pvecExternal[i]) {
            if(m_parameterVec[i]->dist()) {
                m_pvecExternalIndividualData[i].push_back(m_parameterVec[i]->dist()->clone());
            // m_parameterVec[i]->defineDist(nullptr); // do not delete
            }
        }
    }

    /// ************************************************
    /// Deal with external parameters that are in groups
    /// ************************************************

    // if there are groups defined
    if(!m_pvecExternalGroups.empty()) {

        // checks that the vector that stores the groups has the same size
        // as the vector that stores the dependent variables
        if(m_pvecExternalGroups.size()!=m_pvecExternalGroupsDependentVars.size()) {
            m_definitionStatus = IllDefinedParameterGroups;
            return m_definitionStatus;
        }

        // checks that the vector that stores the groups has the same size
        // as the vector that stores the containers of data
        if(m_pvecExternalGroups.size()!=m_pvecExternalGroupData.size()) {
            m_definitionStatus = IllDefinedParameterGroups;
            return m_definitionStatus;
        }

        // for each group
        for(size_t i=0; i<m_pvecExternalGroups.size(); i++) {
            // for each element in the group
            for(size_t j=0; j<m_pvecExternalGroups[i].size(); j++) {

                // check if the parameter id in the group is valid
                int nparams = m_parameterVec.size();
                if(m_pvecExternalGroups[i][j]>=nparams) {
                    m_definitionStatus = IllDefinedParameterGroups;
                    return m_definitionStatus;
                }

                // the parameter id in the group needs to be flagged as external
                if(!m_pvecExternal[m_pvecExternalGroups[i][j]]) {
                    m_definitionStatus = IllDefinedParameterGroups;
                    return m_definitionStatus;
                }

                // if the external parameter belongs to a group then
                // remove the prior
                m_parameterVec[m_pvecExternalGroups[i][j]]->defineDist(nullptr);
            }
        }
    }

    if(m_essentialObjectives.size()!=nOVars) {
        m_essentialObjectives.resize(nOVars,true);
    }

    m_definitionStatus = FullyDefined;
    return m_definitionStatus;
}

TVector<bool> Problem::isExternalParameters() const
{
    return m_pvecExternal;
}

int Problem::numberExternalParameters() const
{
    return std::count(m_pvecExternal.cbegin(), m_pvecExternal.cend(), true);
}

TVector<int> Problem::externalParametersIndices() const
{
    TVector<int> indices;

    for(size_t i=0; i<m_pvecExternal.size(); i++) {
        if(m_pvecExternal[i]) {
            indices.push_back(i);
        }
    }

    return indices;
}

/// ********************************
/// External parameters (Individual)
/// ********************************

TVector<bool> Problem::usePriorForExternalParameters() const
{
    TVector<bool> res;
    int nExt = m_pvecExternalKrigings.size();
    res.reserve(nExt);
    std::transform(m_pvecExternalKrigings.cbegin(),
                   m_pvecExternalKrigings.cend(),
                   std::back_inserter(res),
                   [](const KrigingSPtr& kpt) {return !bool(kpt);});
    return res;
}

void Problem::allocateParameterKrigings(int size)
{
    m_pvecExternalKrigings.resize(size);
}

TVector<KrigingSPtr> Problem::externalParameterKrigings() const
{
    return m_pvecExternalKrigings;
}

void Problem::defineExternalParameterKriging(int idx, KrigingSPtr externalParameterKriging)
{
    m_pvecExternalKrigings[idx] = externalParameterKriging;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::allocateDVec2KrigingsMap(int size)
{
    m_d2KrigingMap.resize(size);
}

TVector<TVector<int> > Problem::dVec2KrigingMap() const
{
    return m_d2KrigingMap;
}

void Problem::defineDVec2KrigingMap(int idx, const TVector<int> &d2kMap)
{
    m_d2KrigingMap[idx] = d2kMap;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::allocateExtIndividualParams(int size)
{
    m_pvecExternalIndividualData.resize(size);
}

TVector<IDistribution*> Problem::externalParameterList(int idx) const
{
    if(idx>=(int)m_pvecExternalIndividualData.size()) {
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }
    return m_pvecExternalIndividualData[(size_t)idx];
}

void Problem::addExtParamData(int idx, IDistribution* dist)
{
    if(idx>=(int)m_pvecExternalIndividualData.size()) {
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }

    m_pvecExternalIndividualData[(size_t)idx].push_back(dist);
}

/// ****************************
/// External parameters (Groups)
/// ****************************

TVector<bool> Problem::isExternalParameterInGroup()
{
    TVector<bool> res;

    if(!m_parameterVec.empty()) {
        fill(res, false, m_parameterVec.size());

        if(!m_pvecExternalGroups.empty()) {
            for(size_t i=0; i<m_pvecExternalGroups.size(); i++) {
                for(size_t j=0; j<m_pvecExternalGroups[i].size(); j++) {
                    int pIndex = m_pvecExternalGroups[i][j];
                    res[pIndex] = true;
                }
            }
        }
    }

    return res;
}

void Problem::allocateParameterGroups(int size)
{
    m_pvecExternalGroups.resize(size);
    m_pvecExternalGroupsDependentVars.resize(size);
    m_pvecExternalGroupData.resize(size);
}

TVector<TVector<int>> Problem::externalParameterGroups() const
{
    return m_pvecExternalGroups;
}

TVector<int> Problem::extGroupDependentVars(size_t idx) const
{
    if(m_pvecExternalGroupsDependentVars.empty()) {
        throw TException("Tigon::Representation::Problem",
                         NullPointerException);
    }

    if(idx>=m_pvecExternalGroupsDependentVars.size()){
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }

    return m_pvecExternalGroupsDependentVars[idx];
}

void Problem::defineParameterGroup(const TVector<TVector<int> > &groups)
{
    allocateParameterGroups(groups.size());
    m_pvecExternalGroups = groups;
    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineParameterGroup(int idx, const TVector<int>& group)
{
    if(m_pvecExternalGroups.empty()) {
        throw TException("Tigon::Representation::Problem",
                         NullPointerException);
    }

    if(idx>=m_pvecExternalGroups.size()){
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }
    m_pvecExternalGroups[idx] = group;

    m_definitionStatus = UnprocessedChanges;
}

void Problem::defineGroupDependentVars(int idx, const TVector<int> &vars)
{
    if(m_pvecExternalGroupsDependentVars.empty()) {
        throw TException("Tigon::Representation::Problem",
                         NullPointerException);
    }

    if(idx>=m_pvecExternalGroupsDependentVars.size()){
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }

    m_pvecExternalGroupsDependentVars[idx] = vars;
}

void Problem::addGroupData(int idx, SampleVectorsSPtr data, int id)
{
    if(m_pvecExternalGroupData.empty()) {
        throw TException("Tigon::Representation::Problem", NullPointerException);
    }

    if(idx>=m_pvecExternalGroupData.size()){
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }

    // if the id does not exists in the map
    if( m_pvecExternalGroupData[idx].count(id)==0 ) {

        // create a new container object
        TimeSeriesContainerSPtr containerObj =
                TimeSeriesContainerSPtr(new TimeSeriesContainer());

        /// \deprecated for TTP
        /// \todo create a constant for the default id
        // if is not the prior (or default) id
//        if(!(id==0)) {
//            // get the prior
//            SampleVectorsSPtr prior =
//                    m_pvecExternalGroupData[idx].at(0)->sampledVal(0);
//            // add the prior
//            containerObj->addData(prior);
//        }
        // add the data
        containerObj->addData(data);
        // add the container to the problem for the given id
        (m_pvecExternalGroupData[idx])[id] = containerObj;

    } else {

        // gets a pointer to the current container
        TimeSeriesContainerSPtr containerObj =
                (m_pvecExternalGroupData[idx]).at(id);
        // adds more data
        containerObj->addData(data);
    }
}

TimeSeriesContainerSPtr Problem::groupData(int idx, int id)
{
    if(m_pvecExternalGroupData.empty()) {
        throw TException("Tigon::Representation::Problem",
                         NullPointerException);
    }

    if(idx>=m_pvecExternalGroupData.size()){
        throw TException("Tigon::Representation::Problem",
                         DomainException);
    }

    if( m_pvecExternalGroupData[idx].count(id)==0 ) {
        // the id does not exist in the map
        // return the data associated with the default id
        return (m_pvecExternalGroupData[idx]).at(0);
    }
    else {
        return (m_pvecExternalGroupData[idx]).at(id);
    }
}

int Problem::nMaxRuns() const
{
    int nruns = 1;

    // for each group of external parameters
    for(auto groupExtParam : m_pvecExternalGroupData) {
        // get the keys of the group
        TVector <int> keys = getMapKeys(groupExtParam);

        // for each key
        for(auto key : keys) {
            int nr = groupExtParam.at(key)->nRuns();
            nruns = std::max(nruns,nr);
        }
    }

    // for each individual external parameters
    for(auto individualExtParam : m_pvecExternalIndividualData) {
        int nr = individualExtParam.size();
        nruns = std::max(nruns,nr);
    }

    return nruns;
}

/// ****************
/// Helper functions
/// ****************

TStringList Problem::getIDs(const TVector<ElementProperties> &vec) const
{
    TStringList lst;
    std::transform(vec.begin(), vec.end(), std::back_inserter(lst),
                   [](ElementProperties p){return p.ID();});
    return lst;
}

TStringList Problem::getNames(const TVector<ElementProperties> &vec) const
{
    TStringList lst;
    std::transform(vec.begin(), vec.end(), std::back_inserter(lst),
                   [](ElementProperties p){return p.name();});
    return lst;
}

TStringList Problem::getUnits(const TVector<ElementProperties> &vec) const
{
    TStringList lst;
    std::transform(vec.begin(), vec.end(), std::back_inserter(lst),
                   [](ElementProperties p){return p.unit();});
    return lst;
}

TStringList Problem::getDescriptions(const TVector<ElementProperties> &vec) const
{
    TStringList lst;
    std::transform(vec.begin(), vec.end(), std::back_inserter(lst),
                   [](ElementProperties p){return p.description();});
    return lst;
}

TVector<Tigon::ElementType> Problem::getTypes(const TVector<ElementProperties> &vec) const
{
    TVector<Tigon::ElementType> types;
    std::transform(vec.begin(), vec.end(), std::back_inserter(types),
                   [](ElementProperties p){return p.type();});
    return types;
}

TVector<OptimizationType> Problem::getOptTypes(const TVector<ElementProperties> &vec) const
{
    TVector<Tigon::OptimizationType> types;
    std::transform(vec.begin(), vec.end(), std::back_inserter(types),
                   [](ElementProperties p){return p.optimizationType();});
    return types;
}

TVector<bool> Problem::essentialObjectives() const
{
    return m_essentialObjectives;
}

void Problem::defineEssentialObjectives(const TVector<bool> &essentialObjectives)
{
    m_essentialObjectives = essentialObjectives;
    m_definitionStatus = UnprocessedChanges;
}

} // namespace Representation
} // namespace Tigon
