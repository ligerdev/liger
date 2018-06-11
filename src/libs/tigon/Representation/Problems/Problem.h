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
#ifndef PROBLEM_H
#define PROBLEM_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <tigon/Representation/Properties/ProblemProperties.h>
#include <tigon/Representation/Properties/ElementProperties.h>

namespace Tigon {
namespace Representation {
class UncertaintyMapping;
class IDistribution;

/*!
 * \brief The Problem class defines the optimization problem.
 */
class LIGER_TIGON_EXPORT Problem
{
public:
    Problem();
    Problem(const Problem& prob);
    ~Problem();

    ProblemSPtr clone()   const;

    ProblemDefinitionStatus isProblemDefined()     const;

    ProblemProperties problemProperties()          const;

    /// Variable/parameter/objective/constraint/unused Properties
    /// \note The fastest way to get ids, names, unites, descriptions,
    /// and types of the elements is to get their properties first and
    /// then access the corresspoding memeber function.
    TVector<ElementProperties> dPrpts()            const;
    TVector<ElementProperties> oPrpts()            const;
    TVector<ElementProperties> pPrpts()            const;
    TVector<ElementProperties> cPrpts()            const;
    TVector<ElementProperties> uPrpts()            const;

    TStringList                dVecIDs()           const;
    TStringList                oVecIDs()           const;
    TStringList                pVecIDs()           const;
    TStringList                cVecIDs()           const;
    TStringList                uVecIDs()           const;

    TStringList                dVecNames()         const;
    TStringList                oVecNames()         const;
    TStringList                pVecNames()         const;
    TStringList                cVecNames()         const;
    TStringList                uVecNames()         const;

    TStringList                dVecUnits()         const;
    TStringList                oVecUnits()         const;
    TStringList                pVecUnits()         const;
    TStringList                cVecUnits()         const;
    TStringList                uVecUnits()         const;

    TStringList                dVecDescriptions()  const;
    TStringList                oVecDescriptions()  const;
    TStringList                pVecDescriptions()  const;
    TStringList                cVecDescriptions()  const;
    TStringList                uVecDescriptions()  const;

    TVector<ElementType>       dVecTypes()         const;
    TVector<ElementType>       oVecTypes()         const;
    TVector<ElementType>       pVecTypes()         const;
    TVector<ElementType>       cVecTypes()         const;
    TVector<ElementType>       uVecTypes()         const;

    TVector<OptimizationType>  oVecOptTypes()      const;
    TVector<OptimizationType>  cVecOptTypes()      const;

    TVector<IElementSPtr>      goalVector()        const;
    TVector<IElementSPtr>      thresholdVector()   const;
    TVector<IElementSPtr>      idealVector()       const;
    TVector<IElementSPtr>      antiIdealVector()   const;
    TVector<IElementSPtr>      nadirVector()       const;

    TVector<IFunctionSPtr>     functionVec()       const;
    TVector<TVector<int> >     f2dMap()            const;
    TVector<TVector<int> >     f2oMap()            const;
    TVector<TVector<int> >     f2pMap()            const;
    TVector<TVector<int> >     f2cMap()            const;
    TVector<TVector<int> >     f2uMap()            const;

    BoxConstraintsDataSPtr     boxConstraints()    const;

    TVector<IElementSPtr>      parameterVector()   const;

    TVector<UncertaintyMapping*>           dVecUncertainties()    const;
    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties() const;

    TVector<bool> isExternalParameters() const;

    void defineProbProperties(ProblemProperties        probprp);
    void defineDVecPrpts(const TVector<ElementProperties>  &dvecPrpts);
    void defineOVecPrpts(const TVector<ElementProperties>  &ovecPrpts);
    void definePVecPrpts(const TVector<ElementProperties>  &pvecPrpts);
    void defineCVecPrpts(const TVector<ElementProperties>  &cvecPrpts);
    void defineUVecPrpts(const TVector<ElementProperties>  &uvecPrpts);

    void defineDVecTypes(const TVector<ElementType>        &dvecTypes);
    void defineOVecTypes(const TVector<ElementType>        &ovecTypes);
    void definePVecTypes(const TVector<ElementType>        &pvecTypes);
    void defineCVecTypes(const TVector<ElementType>        &cvecTypes);
    void defineUVecTypes(const TVector<ElementType>        &uvecTypes);
    void redefineDVarType (int idx, ElementType dvarType);
    void redefineParamType(int idx, ElementType paramType);

    void defineOVecOptimizationTypes(const TVector<OptimizationType> &ovecOptTypes);
    void defineCVecOptimizationTypes(const TVector<OptimizationType> &cvecOptTypes);
    void redefineObjOptimizationType (int idx, OptimizationType      optType);
    void redefineCnstOptimizationType(int idx, OptimizationType      optType);

    /*!
     * \brief defineFunctionVec
     * Every function has a vector of inputs and a vector of outputs.
     * The first index of the map relates to the function in m_fvec.
     * The second index of the map relates to the variable in decision/
     * objective/parameter/constraint vectors. The integer in each vector is
     * the corresponding index in the input/output vectors of the function.
     * A value of -1 means that the variable does not map to the function.
    */
    void defineFunctionVec(const TVector<IFunctionSPtr> &fvec);
    void defineF2dMap(const TVector<TVector<int> > &map);
    void defineF2oMap(const TVector<TVector<int> > &map);
    void defineF2pMap(const TVector<TVector<int> > &map);
    void defineF2cMap(const TVector<TVector<int> > &map);
    void defineF2uMap(const TVector<TVector<int> > &map);

    void defineGoalVector(const TVector<IElementSPtr> &goals);
    void redefineGoal(int idx, IElementSPtr goal);
    void defineThresholdVector(const TVector<IElementSPtr> &thold);
    void redefineThreshold(int idx, IElementSPtr  thold);
    void defineIdealVector(const TVector<IElementSPtr>     &ideal);
    void defineAntiIdealVector(const TVector<IElementSPtr> &antiIdeal);
    void defineNadirVector(const TVector<IElementSPtr>     &nadir);

    void defineParameterVector(const TVector<IElementSPtr> &pvec);
    void defineExternalParameters(const TVector<bool> &extpar);

    void defineDVecUncertainties(const TVector<UncertaintyMapping*>  &umaps);
    void redefineDVarUncertainty(int idx, UncertaintyMapping*  umap);
    void defineFuncOutUncertainties(const TVector<TVector<UncertaintyMapping*> > &umaps);
    void redefineFuncOutputUncertainty(int funcIdx,     int outputIdx,
                                     UncertaintyMapping*        umap);

    /*!
     * \brief updateIdeal, updateAntiIdeal, updateNadir
     * \param newVec
     * \return true if the vector is changed
     */
    bool updateIdeal(const TVector<IElementSPtr> &newVec);
    bool updateAntiIdeal(const TVector<IElementSPtr> &newVec);
    bool updateNadir(const TVector<IElementSPtr>     &newVec);

    void defineBoxConstraints(BoxConstraintsDataSPtr  box);

    /*!
     * \brief appendFunction
     * \param func - a pointer to a function with defined inputs and outputs.
     * \param inParamIdx - a list (vector) of indices of function inputs that
     * considered as parameters in the problem rather than decision variables.
     * Default is empty, i.e., all inputs are decision variables.
     * \param outConstrIdx - a list of indices of function outputs that are
     * considered as constraints in the problem. Default is empty, i.e., no
     * constraints.
     * \param outObjIdx - a list of indices of function outputs that are
     * considerd as objectives. IMPORTANT: the default of empty vector means ALL
     * outputs are objectives.
     */
    void appendFunction(IFunctionSPtr func,
                        const TVector<int>  &inParamIdx   = TVector<int>(),
                        const TVector<int>  &outConstrIdx = TVector<int>(),
                        const TVector<int>  &outObjIdx    = TVector<int>(),
                        const TVector<int>  &outUnuIdx    = TVector<int>());

    ProblemDefinitionStatus processProblemDefinition();

    /// *******************
    /// External parameters
    /// *******************

    int numberExternalParameters() const;
    TVector<int> externalParametersIndices() const;
    TVector<bool> usePriorForExternalParameters() const;

    /// external parameters with shared variables (Kriging functions)
    void allocateParameterKrigings(int size);
    TVector<KrigingSPtr> externalParameterKrigings() const;
    void defineExternalParameterKriging(int idx, KrigingSPtr externalParameterKriging);

    void allocateDVec2KrigingsMap(int size);
    TVector<TVector<int>> dVec2KrigingMap() const;
    void defineDVec2KrigingMap(int idx, const TVector<int> &d2kMap);

    /// external parameters without shared variables
    void allocateExtIndividualParams(int size);
    TVector<IDistribution*> externalParameterList(int idx) const;
    void addExtParamData(int idx, IDistribution* dist);

    /// **************************
    /// External parameters Groups
    /// **************************

    TVector<bool> isExternalParameterInGroup();
    void allocateParameterGroups(int size);

    // external parameter groups
    TVector<TVector<int>> externalParameterGroups() const;
    TVector<int> extGroupDependentVars(size_t idx) const;
    void defineParameterGroup(const TVector<TVector<int>> &groups);
    void defineParameterGroup(int idx, const TVector<int> &group);
    void defineGroupDependentVars(int idx, const TVector<int> & vars);

    // add data to groups
    /*!
     * \brief Add group data for grouped linking variables
     * \param idx The index of the group
     * \param data shared pointer of a SampleVector container
     * \param id For now, specifically for TSP. Ignore it.
     */
    void addGroupData(int idx, SampleVectorsSPtr data, int id=0);
    // get the time-series data for group with index idx
    TimeSeriesContainerSPtr groupData(int idx, int id=0);
    // maximum number of runs so far
    int nMaxRuns() const;

    /// Objective reduction
    TVector<bool> essentialObjectives() const;
    void defineEssentialObjectives(const TVector<bool> &essentialObjectives);

private:
    /// Helper functions
    TStringList getIDs(const TVector<ElementProperties> &vec) const;
    TStringList getNames(const TVector<ElementProperties> &vec) const;
    TStringList getUnits(const TVector<ElementProperties> &vec) const;
    TStringList getDescriptions(const TVector<ElementProperties> &vec) const;
    TVector<ElementType> getTypes(const TVector<ElementProperties>  &vec) const;
    TVector<OptimizationType> getOptTypes(const TVector<ElementProperties> &vec) const;

    ProblemProperties          m_problemPrpts;

    TVector<ElementProperties> m_dvecPrpts;
    TVector<ElementProperties> m_ovecPrpts;
    TVector<ElementProperties> m_pvecPrpts;
    TVector<ElementProperties> m_cvecPrpts;
    TVector<ElementProperties> m_uvecPrpts;

    TVector<IFunctionSPtr>     m_fvec;
    TVector<TVector<bool> >    m_fInPMap;
    TVector<TVector<bool> >    m_fOutOMap;
    TVector<TVector<bool> >    m_fOutCMap;
    TVector<TVector<int> >     m_f2dMap;
    TVector<TVector<int> >     m_f2oMap;
    TVector<TVector<int> >     m_f2pMap;
    TVector<TVector<int> >     m_f2cMap;
    TVector<TVector<int> >     m_f2uMap;

    TVector<IElementSPtr>      m_parameterVec;

    TVector<UncertaintyMapping*>           m_dvecUncertainties;
    TVector<TVector<UncertaintyMapping*> > m_fOutUncertainties;

    TVector<IElementSPtr>      m_ideal;
    TVector<IElementSPtr>      m_nadir;
    TVector<IElementSPtr>      m_antiIdeal;

    /// \todo create box constraints separately
    /// for parameters
    BoxConstraintsDataSPtr     m_boxConstraints;

    TVector<IElementSPtr>      m_goalVec;
    TVector<IElementSPtr>      m_thresholdVec;

    ProblemDefinitionStatus    m_definitionStatus;
    bool                       m_problemPrptsDefined;
    bool                       m_dvecPrptsDefined;
    bool                       m_ovecPrptsDefined;
    bool                       m_pvecPrptsDefined;
    bool                       m_cvecPrptsDefined;
    bool                       m_uvecPrptsDefined;
    bool                       m_fvecDefined;
    bool                       m_mapsDefined;
    bool                       m_pvecDefined;
    bool                       m_dvecUncertaintiesDefined;
    bool                       m_fOutUncertaintiesDefined;
    bool                       m_idealDefined;
    bool                       m_nadirDefined;
    bool                       m_antiIdealDefined;
    bool                       m_boxConstraintsDefined;
    bool                       m_goalVecDefined;
    bool                       m_thresholdVecDefined;

    /// Objective reduction
    TVector<bool>              m_essentialObjectives;

    /// *******************
    /// External parameters
    /// *******************

    // indicates if a parameter is external or not
    TVector<bool>             m_pvecExternal;

    /// Individual external parameters with shared varariables
    TVector<TVector<int>>     m_d2KrigingMap;
    TVector<KrigingSPtr>      m_pvecExternalKrigings;

    /// Individual external parameters without shared variables
    // stores the data for individual external parameters without
    // shared variables. The data is stored as a distribution
    TVector<TVector<IDistribution*>> m_pvecExternalIndividualData;

    /// External parameters that belong to a group
    // keeps the indices of the parameters that belong to a group
    TVector<TVector<int>>     m_pvecExternalGroups;
    // keeps the indices of the decision variables that each group depends on
    TVector<TVector<int>>     m_pvecExternalGroupsDependentVars;

    // stores the data for each group of external parameters
    /// \todo consider changing TMap<int,...> to TMap<String,...>
    /// Deals with problems with shared input linking variables
    /// with different format (at the moment only works for the TTP)
    TVector<TMap<int,TimeSeriesContainerSPtr>> m_pvecExternalGroupData;
};

} // namespace Representation
} // namespace Tigon

#endif // PROBLEM_H
