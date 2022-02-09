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
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/ParEGO.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Directions/SimplexLatticeDirectionIterator.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Operators/Fitness/ConstraintPenalty.h>
#include <tigon/Operators/Fitness/UncertainMonteCarloRobustness.h>
#include <tigon/Operators/Fitness/UncertainConfidenceRobustness.h>
#include <tigon/Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.h>
#include <tigon/Operators/AlgorithmSpecific/ParEGO/ConstraintDirectionFitnessFiltration.h>
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>
#include <tigon/Operators/Composites/ConstrainedSurrogateBasedOptimizer.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

ParEGO::ParEGO()
{
    initialise();
}

ParEGO::ParEGO(Representation::IPSet* ipset)
    : IAlgorithm(ipset)
{
    initialise();
}

ParEGO::~ParEGO()
{

}

void ParEGO::initialise()
{
    /*0*/ SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(this);
    /*1*/ GeneralizedDecomposition*   scal  = new GeneralizedDecomposition(this);
    /*2*/ ConstraintPenalty*          cnstr = new ConstraintPenalty(this);
    /*3*/ //DirectionFitnessFiltration* filt  = new DirectionFitnessFiltration(this);
    /*3*/ ConstraintDirectionFitnessFiltration* filt =
            new ConstraintDirectionFitnessFiltration(this);
    /*4*/ ConstrainedSurrogateBasedOptimizer* opt =
            new ConstrainedSurrogateBasedOptimizer(this);

    appendOperator(dirs);
    appendOperator(scal);
    appendOperator(cnstr);
    appendOperator(filt);
    appendOperator(opt);

    TP_defineConstraintHandlingMethod(Tigon::SurrogateBased);
    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    filt->TP_defineMaxSolutions(Tigon::DefaultMaxSolutionsSurrogateModelling);
    opt->TP_defineErrorMethod(Tigon::ErrConfidenceIntervalBased);
    //opt->TP_defineErrorMethod(Tigon::ErrDensityBased);

    // Changing the following parameter to 1 delivers better results for the
    // tasks of training and searching the surrogate, but it will take longer
    // for the optimization task to complete
    opt->TP_defineOptimizationSearchQuality(0);

    addProperty("maxSolutions"
                , TString("Maximum number of samples for the surrogate model.\n"
                          "If there are more evaluated solutions than this, "
                          "the sample is constructed according to the direction "
                          "of the objective vector, and the scalarised function "
                          "value.")
                , getTType(int));
}

void ParEGO::defineOptimizationSearchQuality(int mode)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineOptimizationSearchQuality(mode);
}

int ParEGO::optimizationSearchQuality() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_optimizationSearchQuality();
}

void ParEGO::TP_defineErrorMethod(Tigon::ErrorHandlingMethod err)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineErrorMethod(err);
}

Tigon::ErrorHandlingMethod ParEGO::TP_errorMethod() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_errorMethod();
}

void ParEGO::TP_defineConstraintHandlingMethod(Tigon::ConstraintHandlingMethod chm)
{
    m_constraintHandlingMethod = chm;
    switch(chm) {
    case Tigon::NoConstraintHandling:
        static_cast<ConstraintPenalty*>(operators().at(2))->
                defineOutputTags(TStringList({TString("DO_NO_OPERATE")}));
        static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
                TP_defineDisableConstraintHandling(true);
        break;

    case Tigon::PenaltyBased:
        static_cast<ConstraintPenalty*>(operators().at(2))->
                defineOutputTags(TStringList({Tigon::TFitness}));
        static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
                TP_defineDisableConstraintHandling(true);
        break;

    case Tigon::SurrogateBased:
    default:
        static_cast<ConstraintPenalty*>(operators().at(2))->
                defineOutputTags(TStringList({TString("DO_NO_OPERATE")}));
        static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
                TP_defineDisableConstraintHandling(false);
        break;
    }
}

Tigon::ConstraintHandlingMethod ParEGO::TP_constraintHandlingMethod()
{
    return m_constraintHandlingMethod;
}

void ParEGO::TP_defineMaxSolutions(int n)
{
    static_cast<DirectionFitnessFiltration*>(operators().at(3))->
            TP_defineMaxSolutions(n);
}

int ParEGO::TP_maxSolutions() const
{
    return static_cast<DirectionFitnessFiltration*>(operators().at(3))->
            TP_maxSolutions();
}

TVector<IMappingSPtr> ParEGO::evaluatedMappings() const
{
    TVector<IMappingSPtr> pop = operators().at(4)->outputSet(0)->all();
    TVector<IMappingSPtr> ret;
    for(auto sol : pop) {
        if(sol->isEvaluated()) {
            ret.push_back(sol);
        }
    }

    return ret;
}

TVector<IMappingSPtr> ParEGO::newMappings() const
{
    TVector<IMappingSPtr> pop = operators().at(4)->outputSet(0)->all();
    TVector<IMappingSPtr> ret;
    for(auto sol : pop) {
        if(!sol->isEvaluated()) {
            ret.push_back(sol);
        }
    }

    return ret;
}

TVector<IMappingSPtr> ParEGO::surrogateMappings() const
{
    TVector<IMappingSPtr> pop = operators().at(3)->outputSet(0)->all();
    return pop;
}

void ParEGO::defineReferenceSetSize(int s)
{
    static_cast<SimplexLatticeDirectionIterator*>(operators().at(0))->
            TP_defineReferenceSetSize(s);
}

int  ParEGO::referenceSetSize()  const
{
    return static_cast<SimplexLatticeDirectionIterator*>(operators().at(0))->
            TP_referenceSetSize();
}

void ParEGO::defineBudgetPerVariableSS(int n)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineBudgetPerVariableSS(n);
}

int ParEGO::budgetPerVariableSS() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_budgetPerVariableSS();
}

void ParEGO::defineInitialPopsizePerVariableSS(int popsize)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineInitialPopsizePerVariableSS(popsize);
}

int ParEGO::initialPopsizePerVariableSS() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_initialPopsizePerVariableSS();
}

void ParEGO::defineStallIterationsSS(int iter)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineStallIterationsSS(iter);
}

int ParEGO::stallIterationsSS() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_stallIterationsSS();
}

void ParEGO::defineBudgetPerVariableTS(int n)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineBudgetPerVariableTS(n);
}

int ParEGO::budgetPerVariableTS() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_budgetPerVariableTS();
}

void ParEGO::defineInitialPopsizePerVariableTS(int popsize)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineInitialPopsizePerVariableTS(popsize);
}

int ParEGO::initialPopsizePerVariableTS() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_initialPopsizePerVariableTS();
}

void ParEGO::defineStallIterationsTS(int iter)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineStallIterationsTS(iter);
}

int ParEGO::stallIterationsTS() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_stallIterationsTS();
}

void ParEGO::defineNumberIterations(int iter)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineNumberIterations(iter);
}

int ParEGO::numberIterations() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_numberIterations();
}

void ParEGO::defineMultiObjectivization(bool isMultiObjectivizationUsedOrNot)
{
    static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineMultiObjectivization(isMultiObjectivizationUsedOrNot);
}

bool ParEGO::multiObjectivization() const
{
    return static_cast<ConstrainedSurrogateBasedOptimizer*>(operators().at(4))->
            TP_multiObjectivization();
}

void ParEGO::defineScalarisingFunction(Tigon::ScalarisationType t, double p)
{
    static_cast<GeneralizedDecomposition*>(operators().at(1))->
            TP_defineScalarisingFunction(t, p);
}

Tigon::ScalarisationType ParEGO::scalarisingFunction() const
{
    return static_cast<GeneralizedDecomposition*>(operators().at(1))->
            TP_scalarisingFunction();
}

TString ParEGO::name()
{
    return TString("ParEGO");
}

TString ParEGO::description()
{
    return TString("Efficient global optimization algorithm for expensive "
                   "multiobjective problems.");
}

} // namespace Algorithms
} // namespace Tigon


REGISTER_IPSET_FACTORY(Algorithms, ParEGO)
