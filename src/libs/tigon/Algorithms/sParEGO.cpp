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
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/sParEGO.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Directions/SimplexLatticeDirectionIterator.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Operators/Fitness/ConstraintPenalty.h>
#include <tigon/Operators/AlgorithmSpecific/sParEGO/SparegoValidation.h>
#include <tigon/Operators/Fitness/RobustnessAssignment.h>
#include <tigon/Operators/Fitness/UncertainMonteCarloRobustness.h>
#include <tigon/Operators/Fitness/UncertainConfidenceRobustness.h>
#include <tigon/Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.h>
#include <tigon/Operators/Composites/SurrogateBasedOptimizerWithPerturbation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

sParEGO::sParEGO()
{
    initialise();
}

sParEGO::sParEGO(Representation::IPSet* ipset)
    : IAlgorithm(ipset)
{
    initialise();
}

sParEGO::~sParEGO()
{

}

void sParEGO::initialise()
{
    /*0*/ SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(this);
    /*1*/ GeneralizedDecomposition*   scal  = new GeneralizedDecomposition(this);
    /*2*/ ConstraintPenalty*          cnstr = new ConstraintPenalty(this);
    /*3*/ SparegoValidation*          valid = new SparegoValidation(this);
    /*4*/ RobustnessAssignment*       iVal  = new RobustnessAssignment(this);
    /*5*/ DirectionFitnessFiltration* filt  = new DirectionFitnessFiltration(this);
    /*6*/ SurrogateBasedOptimizerWithPerturbation*    opt   =
            new SurrogateBasedOptimizerWithPerturbation(this);

    appendOperator(dirs);
    appendOperator(scal);
    appendOperator(cnstr);
    appendOperator(valid);
    appendOperator(iVal);
    appendOperator(filt);
    appendOperator(opt);

    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshev);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    filt->TP_defineMaxSolutions(Tigon::DefaultMaxSolutionsSurrogateModelling);
    opt->TP_defineErrorMethod(Tigon::ErrConfidenceIntervalBased);
    //opt->TP_defineErrorMethod(Tigon::ErrDensityBased);
    opt->TP_defineDensityEstimationBandwidth(Tigon::KDEBandwidthRatio *
                                             TP_neighbourhoodRadius());

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

    addProperty("neighbourhoodRadius"
                , TString("Maximum Euclidean distance in normalised decision "
                          "space for two solutions to be considered as neighbours.")
                , getTType(double));
}

void sParEGO::defineOptimizationSearchQuality(int mode)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineOptimizationSearchQuality(mode);
}

int sParEGO::optimizationSearchQuality() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_optimizationSearchQuality();
}

void sParEGO::TP_defineErrorMethod(ErrorHandlingMethod err)
{
    switch(err) {
    case Tigon::ErrConfidenceIntervalBased:
        static_cast<SparegoValidation*>(operators().at(3))->
                TP_defineConfidenceInSample(ConfidenceDecrease);
        break;

    case Tigon::ErrDensityBased:
    default:
        static_cast<SparegoValidation*>(operators().at(3))->
                TP_defineConfidenceInSample(UnchangedDistribution);
        break;
    }

    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineErrorMethod(err);
}

Tigon::ErrorHandlingMethod sParEGO::TP_errorMethod() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_errorMethod();
}

void sParEGO::TP_defineMaxSolutions(int n)
{
    static_cast<DirectionFitnessFiltration*>(operators().at(5))->
            TP_defineMaxSolutions(n);
}

int sParEGO::TP_maxSolutions() const
{
    return static_cast<DirectionFitnessFiltration*>(operators().at(5))->
            TP_maxSolutions();
}

void sParEGO::TP_defineNeighbourhoodRadius(double r)
{
    static_cast<SparegoValidation*>(operators().at(3))->
            TP_defineNeighbourhoodRadius(r);
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineNeighbourhoodRadius(r);
}

double sParEGO::TP_neighbourhoodRadius() const
{
    return static_cast<SparegoValidation*>(operators().at(3))->
            TP_neighbourhoodRadius();
}

TVector<IMappingSPtr> sParEGO::evaluatedMappings() const
{
    TVector<IMappingSPtr> pop = operators().at(6)->outputSet(0)->all();
    TVector<IMappingSPtr> ret;
    for(size_t i=0; i<pop.size(); i++) {
        if(pop[i]->isEvaluated()) {
            ret.push_back(pop[i]);
        }
    }
    return ret;
}

TVector<IMappingSPtr> sParEGO::newMappings()  const
{
    TVector<IMappingSPtr> pop = operators().at(6)->outputSet(0)->all();
    TVector<IMappingSPtr> ret;
    for(size_t i=0; i<pop.size(); i++) {
        if(!pop[i]->isEvaluated()) {
            ret.push_back(pop[i]);
        }
    }
    return ret;
}

void sParEGO::defineReferenceSetSize(int s)
{
    static_cast<SimplexLatticeDirectionIterator*>(operators().at(0))->
            TP_defineReferenceSetSize(s);
}

int  sParEGO::referenceSetSize()  const
{
    return static_cast<SimplexLatticeDirectionIterator*>(operators().at(0))->
            TP_referenceSetSize();
}

void sParEGO::defineIndicator(TString indicatorType, double indParam)
{
    static_cast<RobustnessAssignment*>(operators().at(4))->
            defineIndicator(indicatorType, indParam);
}

double sParEGO::percentile() const
{
    return static_cast<SparegoValidation*>(operators().at(3))->
            TP_percentile();
}

void sParEGO::definePercentile(double p)
{
    static_cast<SparegoValidation*>(operators().at(3))->
            TP_definePercentile(p);
}

void sParEGO::defineBudgetPerVariableSS(int n)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineBudgetPerVariableSS(n);
}

int sParEGO::budgetPerVariableSS() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_budgetPerVariableSS();
}

void sParEGO::defineInitialPopsizePerVariableSS(int popsize)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineInitialPopsizePerVariableSS(popsize);
}

int sParEGO::initialPopsizePerVariableSS() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_initialPopsizePerVariableSS();
}

void sParEGO::defineStallIterationsSS(int iter)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineStallIterationsSS(iter);
}

int sParEGO::stallIterationsSS() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_stallIterationsSS();
}

void sParEGO::defineBudgetPerVariableTS(int n)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineBudgetPerVariableTS(n);
}

int sParEGO::budgetPerVariableTS() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_budgetPerVariableTS();
}

void sParEGO::defineInitialPopsizePerVariableTS(int popsize)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineInitialPopsizePerVariableTS(popsize);
}

int sParEGO::initialPopsizePerVariableTS() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_initialPopsizePerVariableTS();
}

void sParEGO::defineStallIterationsTS(int iter)
{
    static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_defineStallIterationsTS(iter);
}

int sParEGO::stallIterationsTS() const
{
    return static_cast<SurrogateBasedOptimizerWithPerturbation*>(operators().at(6))->
            TP_stallIterationsTS();
}


void sParEGO::defineScalarisingFunction(Tigon::ScalarisationType t, double p)
{
    static_cast<GeneralizedDecomposition*>(operators().at(1))->
            TP_defineScalarisingFunction(t, p);
}

Tigon::ScalarisationType sParEGO::scalarisingFunction() const
{
    return static_cast<GeneralizedDecomposition*>(operators().at(1))->
            TP_scalarisingFunction();
}

TString sParEGO::name()
{
    return TString("sParEGO");
}

TString sParEGO::description()
{
    return TString("Efficient global optimization algorithm for expensive "
                   "uncertain multiobjective problems.");
}

} // namespace Algorithms
} // namespace Tigon

REGISTER_IPSET_FACTORY(Algorithms, sParEGO)
