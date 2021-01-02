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
#ifndef SURROGATEBASEDOPTIMIZER_H
#define SURROGATEBASEDOPTIMIZER_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Composites/IComposite.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/tigon_global.h>

using Tigon::ErrorHandlingMethod;

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SurrogateBasedOptimizer : public IComposite
{
    HANDLE_READ_PROPERTIES_BEGIN(IComposite)
    READ(OptimizationSearchQuality, TP_optimizationSearchQuality)
    READ(BudgetPerVariableSS, TP_budgetPerVariableSS)
    READ(InitialPopsizePerVariableSS, TP_initialPopsizePerVariableSS)
    READ(StallIterationsSS, TP_stallIterationsSS)
    READ(BudgetPerVariableTS, TP_budgetPerVariableTS)
    READ(InitialPopsizePerVariableTS, TP_initialPopsizePerVariableTS)
    READ(StallIterationsTS, TP_stallIterationsTS)
    READ(DensityEstimationBandwidth, TP_densityEstimationBandwidth)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IComposite)
    WRITE(OptimizationSearchQuality, int, TP_defineOptimizationSearchQuality)
    WRITE(BudgetPerVariableSS, int, TP_defineBudgetPerVariableSS)
    WRITE(InitialPopsizePerVariableSS, int, TP_defineInitialPopsizePerVariableSS)
    WRITE(StallIterationsSS, int, TP_defineStallIterationsSS)
    WRITE(BudgetPerVariableTS, int, TP_defineBudgetPerVariableTS)
    WRITE(InitialPopsizePerVariableTS, int, TP_defineInitialPopsizePerVariableTS)
    WRITE(StallIterationsTS, int, TP_defineStallIterationsTS)
    WRITE(DensityEstimationBandwidth, double, TP_defineDensityEstimationBandwidth)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::SurrogateBasedOptimizer)

public:
    SurrogateBasedOptimizer();
    SurrogateBasedOptimizer(Representation::IPSet* ipset);
    ~SurrogateBasedOptimizer();

    void evaluateNode();

    // Properties for the optimizer to search the surrogate
    void TP_defineBudgetPerVariableSS(int n);
    int  TP_budgetPerVariableSS()      const;

    void TP_defineInitialPopsizePerVariableSS(int n);
    int  TP_initialPopsizePerVariableSS()      const;

    void TP_defineStallIterationsSS(int iter);
    int  TP_stallIterationsSS()         const;

    // Properties for the optimizer to train the surrogate
    void TP_defineBudgetPerVariableTS(int n);
    int  TP_budgetPerVariableTS()      const;

    void TP_defineInitialPopsizePerVariableTS(int n);
    int  TP_initialPopsizePerVariableTS()      const;

    void TP_defineStallIterationsTS(int iter);
    int  TP_stallIterationsTS()         const;

    // Other properties
    void   TP_defineDensityEstimationBandwidth(double bandwidth);
    double TP_densityEstimationBandwidth()                 const;

    void TP_defineErrorMethod(ErrorHandlingMethod err);
    ErrorHandlingMethod TP_errorMethod()         const;

    void TP_defineOptimizationSearchQuality(int mode);
    int  TP_optimizationSearchQuality()         const;

    // Information about the node
    TString     name();
    TString     description();

protected:
    /*
     * Runs ACROMUSE for a fixed number of function evaluations
     * (budgetPerVariable x decisionVecSize)
     */
    IMappingSPtr runOptimizer_FEBased(ProblemSPtr prob);

    /*
     * Runs ACROMUSE with termination criterion based on the average improvement
     * of the best fitness over a given number of iterations (StallIterations)
     */
    IMappingSPtr runOptimizer_AIBased(ProblemSPtr prob);

private:
    void initialise();

    int m_budgetPerVarSS;
    int m_initialPopsizePerVariableSS;
    int m_stallIterationsSS;

    int m_budgetPerVarTS;
    int m_initialPopsizePerVariableTS;
    int m_stallIterationsTS;

    double m_densityEstimationBandwidth;

    int m_mode;

    Tigon::ErrorHandlingMethod m_errMethod;
};

} // namespace Operators
} // namespace Tigon

#endif // SURROGATEBASEDOPTIMIZER_H
