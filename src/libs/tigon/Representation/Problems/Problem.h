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
#ifndef PROBLEM_H
#define PROBLEM_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <tigon/Representation/Problems/ParamConnect.h>

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
    /// then access the corresspoding member function.
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

    TVector<bool>              setGoalVector()     const;
    TVector<IElementSPtr>      goalVector()        const;
    TVector<int>               priorityVector()    const;
    TVector<IElementSPtr>      thresholdVector()   const;
    TVector<IElementSPtr>      idealVector()       const;
    TVector<IElementSPtr>      antiIdealVector()   const;
    TVector<IElementSPtr>      nadirVector()       const;

    TVector<IFunctionSPtr>     functionVec()       const;
    TVector<TVector<int>>      f2dMap()            const;
    TVector<TVector<int>>      f2oMap()            const;
    TVector<TVector<int>>      f2pMap()            const;
    TVector<TVector<int>>      f2cMap()            const;
    TVector<TVector<int>>      f2uMap()            const;

    BoxConstraintsDataSPtr     boxConstraints()    const;

    TVector<IElementSPtr>      parameterVector()   const;

    TVector<UncertaintyMapping*>           dVecUncertainties()    const;
    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties() const;

    /// Properties
    void defineProbProperties(ProblemProperties        probprp);
    void defineDVecPrpts(const TVector<ElementProperties>  &dvecPrpts);
    void defineOVecPrpts(const TVector<ElementProperties>  &ovecPrpts);
    void definePVecPrpts(const TVector<ElementProperties>  &pvecPrpts);
    void defineCVecPrpts(const TVector<ElementProperties>  &cvecPrpts);
    void defineUVecPrpts(const TVector<ElementProperties>  &uvecPrpts);

    /// Types
    void defineDVecTypes(const TVector<ElementType>        &dvecTypes);
    void defineOVecTypes(const TVector<ElementType>        &ovecTypes);
    void definePVecTypes(const TVector<ElementType>        &pvecTypes);
    void defineCVecTypes(const TVector<ElementType>        &cvecTypes);
    void defineUVecTypes(const TVector<ElementType>        &uvecTypes);
    void redefineDVarType (int idx, ElementType dvarType);
    void redefineParamType(int idx, ElementType paramType);

    /// Optimization Types
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
    void defineF2dMap(const TVector<TVector<int>> &map);
    void defineF2oMap(const TVector<TVector<int>> &map);
    void defineF2pMap(const TVector<TVector<int>> &map);
    void defineF2cMap(const TVector<TVector<int>> &map);
    void defineF2uMap(const TVector<TVector<int>> &map);

    void defineSetGoalVector(const TVector<bool> &setGoals);
    void redefineSetGoal(int idx, bool setGoal);
    void defineGoalVector(const TVector<IElementSPtr> &goals);
    void redefineGoal(int idx, IElementSPtr goal);
    void definePriorityVector(const TVector<int> &priorities);
    void redefinePriority(int idx, int priority);
    void defineThresholdVector(const TVector<IElementSPtr> &thold);
    void redefineThreshold(int idx, IElementSPtr  thold);
    void defineIdealVector(const TVector<IElementSPtr>     &ideal);
    void defineAntiIdealVector(const TVector<IElementSPtr> &antiIdeal);
    void defineNadirVector(const TVector<IElementSPtr>     &nadir);

    void defineParameterVector(const TVector<IElementSPtr> &pvec);
    void defineExternalParameters(const TVector<bool> &extpar);

    void defineDVecUncertainties(const TVector<UncertaintyMapping*> &umaps);
    void redefineDVarUncertainty(int idx, UncertaintyMapping* umap);
    void defineFuncOutUncertainties(const TVector<TVector<UncertaintyMapping*>>
                                    &umaps);
    void redefineFuncOutputUncertainty(int funcIdx, int outputIdx,
                                       UncertaintyMapping* umap);

    /*!
     * \brief updateIdeal, updateAntiIdeal, updateNadir
     * \param newVec
     * \return true if the vector is changed
     */
    bool updateIdeal(const TVector<IElementSPtr>     &newVec);
    bool updateAntiIdeal(const TVector<IElementSPtr> &newVec);
    bool updateNadir(const TVector<IElementSPtr>     &newVec);

    void defineBoxConstraints(BoxConstraintsDataSPtr box);

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
     * \param outUnuIdx - a list of indices of outputs that are not used by the
     * optimization process. Default is empty, i.e., no unused outputs.
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

    TVector<bool> isExternalParameters() const;
    int           numberExternalParameters() const;

    TVector<bool> paramConnectIsConnected() const;
    bool          paramConnectIsConnected(size_t idx) const;

    TVector<size_t> paramConnectOutputIdx() const;
    size_t          paramConnectOutputIdx(size_t idx) const;

    TVector<OutputType> paramConnectOutputType() const;
    OutputType          paramConnectOutputType(size_t idx) const;

    TVector<bool> paramConnectIsMaximization() const;
    bool          paramConnectIsMaximization(size_t idx) const;

    TVector<TVector<int>> connectedFunctions() const;

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

    TVector<IFunctionSPtr>    m_fvec;
    TVector<TVector<bool>>    m_fInPMap;
    TVector<TVector<bool>>    m_fOutOMap;
    TVector<TVector<bool>>    m_fOutCMap;
    TVector<TVector<int>>     m_f2dMap;
    TVector<TVector<int>>     m_f2oMap;
    TVector<TVector<int>>     m_f2pMap;
    TVector<TVector<int>>     m_f2cMap;
    TVector<TVector<int>>     m_f2uMap;

    TVector<IElementSPtr>      m_parameterVec;

    TVector<UncertaintyMapping*>           m_dvecUncertainties;
    TVector<TVector<UncertaintyMapping*> > m_fOutUncertainties;

    TVector<IElementSPtr>      m_ideal;
    TVector<IElementSPtr>      m_nadir;
    TVector<IElementSPtr>      m_antiIdeal;

    /// \todo create box constraints separately
    /// for parameters
    BoxConstraintsDataSPtr     m_boxConstraints;

    TVector<bool>              m_setGoalVec;
    TVector<IElementSPtr>      m_goalVec;
    TVector<int>               m_priorityVec;
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
    bool                       m_setGoalVecDefined;
    bool                       m_goalVecDefined;
    bool                       m_priorityVecDefined;
    bool                       m_thresholdVecDefined;

    /// *******************
    /// External parameters
    /// *******************

    TVector<ParamConnect>  m_paramConnect;
    TVector<TVector<int>>  m_connectedFunctions;

    // indicates if a parameter is external or not
    TVector<bool>             m_pvecExternal;
};

} // namespace Representation
} // namespace Tigon

#endif // PROBLEM_H
