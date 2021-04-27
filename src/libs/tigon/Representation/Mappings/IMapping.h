/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef IMAPPING_H
#define IMAPPING_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Core/TVector.h>

// STL Includes
#include <mutex>

namespace Tigon {
class BatchSolveRegister;

namespace Representation {
class IElement;
class IMappingPrivate;

struct DirectedElements {
    TVector<IElementSPtr>     elems;
    TVector<OptimizationType> optTypes;
};

}
}

namespace Tigon {
namespace Representation {

/*!
 * \brief The IMapping class represents a single solution
 * \todo Rename this class to Solution
 * \todo The implementation of this class can be improved.
 */
class LIGER_TIGON_EXPORT IMapping
{
public:
    IMapping();
    IMapping(const IMapping& mapping);
    IMapping(ProblemSPtr prob);
    IMapping(ProblemSPtr prob, Tigon::Log::LogManagerSPtr log);

    virtual ~IMapping();
    IMapping& operator=(const IMapping& mapping);

    virtual IMappingSPtr clone();

    /*!
     * \brief problem - used for read-only purposes.
     * \return a copy of the problem defined for the IMapping.
     */
    ProblemSPtr             problem()                 const;
    ProblemDefinitionStatus defineProblem(ProblemSPtr prob);

    /// Decision vector
    //TODO: remove type checks when setting values
    TVector<IElementSPtr>  decisionVec()               const;
    IElementSPtr           decisionVar(int var)        const;
    TVector<double>        doubleDecisionVec()         const;
    double                 doubleDecisionVar(int var)  const;

    bool defineDecisionVar(int idx,       const IElement &val);
    bool defineDecisionVar(int idx,       IElementSPtr    val);
    bool defineDecisionVec(const TVector<IElement>      &dvec);
    bool defineDecisionVec(const TVector<IElementSPtr>  &dvec);

    /// Parameter vector
    TVector<IElementSPtr> parameterVec()              const;
    IElementSPtr          parameterVar(int var)       const;
    TVector<double>       doubleParameterVec()        const;
    double                doubleParameterVar(int var) const;

    bool defineParameterVec(const TVector<IElement>      &pvec);
    bool defineParameterVec(const TVector<IElementSPtr>  &pvec);

    TVector<bool> isExternalParameters() const;
    int numberExternalParameters() const;

    /// Objective vector
    //TODO: remove type checks when setting values
    TVector<IElementSPtr> objectiveVec()              const;
    IElementSPtr          objectiveVar(int var)       const;
    TVector<double>       doubleObjectiveVec()        const;
    double                doubleObjectiveVar(int var) const;

    bool defineObjectiveVar(int idx,      const IElement &val);
    bool defineObjectiveVar(int idx,      IElementSPtr    val);
    bool defineObjectiveVec(const TVector<IElement>      &ovec);
    bool defineObjectiveVec(const TVector<IElementSPtr>  &ovec);

    /// Constraints vector
    //TODO: remove type checks when setting values
    TVector<IElementSPtr> constraintVec()             const;
    IElementSPtr          constraintVar(int idx)         const;
    TVector<double>       doubleConstraintVec()       const;
    double                doubleConstraintVar(int idx)   const;

    bool defineConstraint(int idx,        const IElement &val);
    bool defineConstraint(int idx,        IElementSPtr    val);
    bool defineConstraintVec(const TVector<IElement>     &cvec);
    bool defineConstraintVec(const TVector<IElementSPtr> &cvec);

    /// Unused vector
    TVector<IElementSPtr> unusedVec()             const;
    IElementSPtr          unusedVar(int idx)      const;
    TVector<double>       doubleUnusedVec()       const;
    double                doubleUnusedVar(int idx)   const;

    bool defineUnused(int idx,        const IElement &val);
    bool defineUnused(int idx,        IElementSPtr    val);
    bool defineUnusedVec(const TVector<IElement>     &uvec);
    bool defineUnusedVec(const TVector<IElementSPtr> &uvec);

    /// Weighting vector
    TVector<double> weightingVec() const;
    bool defineWeight(int idx,  double val);
    // If you get a error here READ the compilation message.
    // These functions stop implicit type conversions.
    template <class T> bool defineWeightingVec(int idx, T val) = delete;
    bool defineWeightingVec(const TVector<double> &wvec);

    /// Cost
    void         defineCostType(ElementType t);
    IElementSPtr cost()                  const;
    double       doubleCost()            const;
    void         defineCost(const IElement&    c);
    void         defineCost(const IElementSPtr c);
    void         defineCost(double              c);

    /// Evaluation checks
    bool isEvaluated()                  const;
    bool isObjectiveVecEvaluated()      const;
    bool isObjectiveEvaluated(int idx)  const;
    bool isConstraintVecEvaluated()     const;
    bool isConstraintEvaluated(int idx) const;
    ValidationStatus isValidated()      const;
    bool isSuccessfulEval()             const;
    void defineErrorFlag(bool           flag);

    /// Quality check
    bool isFeasible()  const;
    bool isPertinent() const;

    void defineEvaluated(                   bool status=true);
    void defineObjectiveVecEvaluated(       bool status=true);
    void defineObjectiveEvaluated(int idx,  bool status=true);
    void defineConstraintVecEvaluated(      bool status=true);
    void defineConstraintEvaluated(int idx, bool status=true);
    void defineValidated(ValidationStatus          status);

    /// Scalarization checks
    bool isScalarised()  const;
    void defineScalarised(bool status = true);

    /// Run number
    void defineRunNumber(int runNumber);

    /// Evaluation
    int evaluate();
    int parallelEvaluate();
    void registerForBatchEvaluation(BatchSolveRegister* batchReg);

    /// Log
    void defineLogManager(Log::LogManagerSPtr log);
    Log::LogManagerSPtr log()                const;

    /// Optimization / Generic mapping
    bool isOptimizationMapping()          const;
    void defineOptimizationMapping(bool status);

    /// Distributed
    void setRunNumber(int run);
    int  runNumber() const;

    static TString className();

private:
    void copyMapping(const IMapping& mapping);
    void createDecisionVariables();
    void createParameters();
    void createOutputVectors();
    void processExternalParameters();
    void defineFuncEvaluated(int idx,  bool status);
    void applyEvaluationUncertainty(int funcIdx, TVector<IElementSPtr> funcOutputs);

    TVector<IElementSPtr> fetchInputs(int  funcIdx);
    DirectedElements      fetchOutputs(int funcIdx);

    IMappingPrivate*     d;
    ProblemSPtr          m_problem;
    Log::LogManagerSPtr  m_log;
    static std::mutex    ms_mutex;
};

} // namespace Representation
} // namespace Tigon

#endif // IMAPPING_H
