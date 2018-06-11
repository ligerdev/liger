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

#pragma once

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Core/TObject.h>

namespace Tigon {
namespace Representation {
class IPSetPrivate;
class IElement;
class IMapping;
class ISet;
class ElementProperties;
class ProblemProperties;
class UncertaintyMapping;

class LIGER_TIGON_EXPORT IPSet : public TObject
{
    DECLARE_CLASS(Tigon::IPSet)

public:
    IPSet();
    IPSet(const IPSet& set);
    virtual ~IPSet();
    virtual IPSet* clone();

    /// Log keeping
    // Log manager
    void defineLogManager(Log::LogManagerSPtr log);
    Log::LogManagerSPtr log()                const;
    void                                clearLog();

    /// Sets tags
    TStringList    allTags()                              const;
    bool           tagExists(const TString &tag)          const;
    TVector<ISet*> setsWithTags(const TStringList &tags)  const;
    TVector<ISet*> setsWithTag(const TString &tag)        const;
    ISet* setWithTags(const TStringList &tags, int idx=0) const;
    ISet* setWithTag(const TString &tag, int idx=0)       const;

    void  tagSet(ISet* sSet,           const TString &tag);
    bool  removeTagFromSet(ISet* sSet, const TString &tag);

    /// Sets
    TVector<ISet*>   allSets() const;
    int         numberOfSets() const;
    void        clearSets();

    ISet* set(int         idx) const; //returns null if out of bounds
    int indexOfSet(ISet* sSet) const; //returns the index or -1 if no match

    bool  appendSet(ISet* sSet);
    void  appendSet(ISet* sSet, const TString &tag);
    void  appendSet(ISet* sSet, const TStringList &tags);
    ISet* appendSet(const TString &tag); // Creates an empty set with a tag
    ISet* appendSet(const TStringList &tags); // Creates an empty set with tags

    bool removeSet(int    idx); //returns true if the set was found and removed
    bool removeSet(ISet* sSet);

    // Returns true if targetSet belongs to IPSet. Otherwise, targetSet is
    // appended and the function returns false.
    bool appendToSet(ISet* targetSet, IMappingSPtr imap);

    // Returns true if the archive is updated (imap is non-dominated)
    // TODO: At the moment the archive includes all nondominated solutions.
    //       We will need to come up with update strategies.
    bool isKeepArchive()     const;
    void defineKeepArchive(bool b);
    Tigon::ArchiveUpdateStatus updateNonDominatedArchive(IMappingSPtr imap,
                                                         bool weakOrEpsilon=true);

    /// Problem
    TString            problemID()          const;
    TString            problemName()        const;
    TString            problemDescription() const;
    ProblemType        problemType()        const;
    ProblemProperties  problemProperties()  const;

    /// Evaluation Functions and Links
    TVector<IFunctionSPtr> functionVec()             const;
    TVector<TVector<int> > function2VariablesMap()   const;
    TVector<TVector<int> > function2ObjectivesMap()  const;
    TVector<TVector<int> > function2ParametersMap()  const;
    TVector<TVector<int> > function2ConstraintsMap() const;
    TVector<TVector<int> > function2UnusedMap() const;

    /// Decision Variables Vector
    int                          decisionVecSize()                const;
    TStringList                  decisionVecIDs()                 const;
    TStringList                  decisionVecNames()               const;
    TStringList                  decisionVecDescriptions()        const;
    TStringList                  decisionVecUnits()               const;
    TVector<Tigon::ElementType>  decisionVecTypes()               const;
    TVector<ElementProperties>   decisionVecProperties()          const;
    TVector<UncertaintyMapping*> dVecUncertainties()              const;

    /// Parameters Vector
    TVector<IElementSPtr>        parameterVector()                const;
    int                          parameterVecSize()               const;
    TStringList                  parameterVecIDs()                const;
    TStringList                  parameterVecNames()              const;
    TStringList                  parameterVecDescriptions()       const;
    TStringList                  parameterVecUnits()              const;
    TVector<Tigon::ElementType>  parameterVecTypes()              const;
    TVector<ElementProperties>   parameterVecProperties()         const;

    /// Objectives Vector
    int                          objectiveVecSize()               const;
    TStringList                  objectiveVecIDs()                const;
    TStringList                  objectiveVecNames()              const;
    TStringList                  objectiveVecDescriptions()       const;
    TStringList                  objectiveVecUnits()              const;
    TVector<Tigon::ElementType>  objectiveVecTypes()              const;
    TVector<OptimizationType>    objectiveVecOptimizationTypes()  const;
    TVector<ElementProperties>   objectiveVecProperties()         const;
    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties() const;

    /// Constraints Vector
    int                          constraintVecSize()              const;
    TStringList                  constraintVecIDs()               const;
    TStringList                  constraintVecNames()             const;
    TStringList                  constraintVecDescriptions()      const;
    TStringList                  constraintVecUnits()             const;
    TVector<Tigon::ElementType>  constraintVecTypes()             const;
    TVector<OptimizationType>    constraintVecOptimizationTypes() const;
    TVector<ElementProperties>   constraintVecProperties()        const;

    /// Unused Vector
    int                          unusedVecSize()                  const;
    TStringList                  unusedVecIDs()                   const;
    TStringList                  unusedVecNames()                 const;
    TStringList                  unusedVecDescriptions()          const;
    TStringList                  unusedVecUnits()                 const;
    TVector<Tigon::ElementType>  unusedVecTypes()                 const;
    TVector<OptimizationType>    unusedVecOptimizationTypes()     const;
    TVector<ElementProperties>   unusedVecProperties()            const;

    /// Box constraints
    BoxConstraintsDataSPtr boxConstraints()         const;

    /// Special Vectors in Objective (and constraint) Space
    TVector<IElementSPtr> idealVec()     const;
    TVector<IElementSPtr> nadirVec()     const;
    TVector<IElementSPtr> antiIdealVec() const;
    TVector<IElementSPtr> goalVec()      const;
    TVector<IElementSPtr> thresholdVec() const;
    TVector<double>       dirVec()       const;   

    /// External
    TVector<bool> isExternalParameters() const;
    int numberExternalParameters() const;

    void defineIdealVec(const TVector<IElementSPtr> &ideal);
    void defineNadirVec(const TVector<IElementSPtr> &nadir);
    void defineAntiIdealVec(const TVector<IElementSPtr> &antiIdeal);

    bool updateIdealNadirVec(IMappingSPtr newIMap);
    bool updateIdealVec(IMappingSPtr                 newIMap);
    bool updateAntiIdealVec(IMappingSPtr             newIMap);
    bool updateNadirVec(IMappingSPtr                 newIMap);
    void defineGoalVec(const TVector<IElementSPtr>&    goals);
    void defineGoal(int idx,               IElementSPtr goal);
    void defineThresholdVec(const TVector<IElementSPtr>& thresholds);
    void defineThreshold(int idx,   IElementSPtr threshold);
    void defineDirVec(const TVector<double>           &dir);   

    /// Optimization and Generic Mapping constructors
    IMappingSPtr createOptimizationMapping(ISet* targetSet);
    IMappingSPtr createGenericMapping(ISet* targetSet, ProblemSPtr prob);
    IMappingSPtr cloneMapping(ISet* targetSet, IMappingSPtr imap);

    /// Iteration and evaluation budget
    bool isTerminate()            const;
    int  currentIteration()       const;
    void incrementIteration();
    void setCurrentIteration(int citer);
    int  remainingIteratoins()    const;
    int  maxIteration()           const;
    void defineMaxIteration(int  nIter);
    int  remainingBudget()        const;
    int  budget()                 const;
    void defineBudget(int       budget);
    void decrementBudget(int  cost = 1);
    int  usedBudget()             const;
    void resetBudgetCount(int    c = 0);

    /// Error state
    void        defineErrorState(TErrorState state);
    TErrorState errorState()                  const;
    void        ignoreUnsuccessfulEvals();
    void        removeInvalidIMappings();
    void        deteriorateInvalidIMappings();

    /// MultiPack - Data structure for arbitrary information passing
    void setMultiPack(const TMap<TString, TVector<double>> &mname);
    void insertMultiPack(const TString &name, const TVector<double> &data);
    bool appendMultiPack(const TString &name, const TVector<double> &data);
    bool multiPackExists(const TString &name) const;
    TVector<double> multiPack(const TString &name) const;
    TMap<TString, TVector<double> > multiPack() const;
    void clearMutliPack();

    /// Evaluation
    virtual void evaluate() {}

    /// IPSet chaining
    void setFromIPSet(const IPSet* ipset);

    /// Initialization status
    ProblemDefinitionStatus isProblemDefined() const;

    /// Liger Engine Compatibility Operators
    void    clearData();
    virtual IPSet& operator=(const IPSet& data);

    // public interfance in IFormulation operator
    const ProblemSPtr problem() const;

protected:
    /// Private data access
    TSharedPointer<IPSetPrivate> getInternal() const;
    void setInternal(TSharedPointer<IPSetPrivate> nd);

    void defineProblem(ProblemSPtr    prob);
    void appendFunction(IFunctionSPtr func);
    void updateProblem();

private:
    /// Utility functions
    void updatePSetMappings();

    TSharedPointer<IPSetPrivate> d;
};

} // namespace Representation
} // namespace Tigon
