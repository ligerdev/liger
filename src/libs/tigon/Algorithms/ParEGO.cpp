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
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/ParEGO.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/Directions/SimplexLatticeDirectionIterator.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Operators/Fitness/ConstraintPenalty.h>
#include <tigon/Operators/Fitness/UncertainMonteCarloRobustness.h>
#include <tigon/Operators/Fitness/UncertainConfidenceRobustness.h>
#include <tigon/Operators/AlgorithmSpecific/parego/DirectionFitnessFiltration.h>
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>
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
    /*3*/ DirectionFitnessFiltration* filt  = new DirectionFitnessFiltration(this);
    /*4*/ SurrogateBasedOptimizer*    opt   = new SurrogateBasedOptimizer(this);

    appendOperator(dirs);
    appendOperator(scal);
    appendOperator(cnstr);
    appendOperator(filt);
    appendOperator(opt);

    scal->TP_defineScalarisingFunction(Tigon::WeightedChebyshevAugmented);
    scal->TP_defineWeightScopeApproach(Tigon::GlobalWeight);
    opt->TP_defineErrorMethod(Tigon::ErrDensityBased);

    addProperty("maxSolutions"
                , TString("Maximum number of samples for the surrogate model.\n"
                          "If there are more evaluated solutions than this, "
                          "the sample is constructed according to the direction "
                          "of the objective vector, and the scalarised function "
                          "value.")
                , typeid(int).hash_code());
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

void ParEGO::TP_defineNeighbourhoodRadius(double r)
{
    static_cast<SurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineNeighbourhoodRadius(r);
}

double ParEGO::TP_neighbourhoodRadius() const
{
    return static_cast<SurrogateBasedOptimizer*>(operators().at(4))->
            TP_neighbourhoodRadius();
}

TVector<IMappingSPtr> ParEGO::evaluatedMappings() const
{
    TVector<IMappingSPtr> pop = operators().at(4)->outputSet(0)->all();
    TVector<IMappingSPtr> ret;
    for(int i=0; i<pop.size(); i++) {
        if(pop[i]->isEvaluated()) {
            ret.push_back(pop[i]);
        }
    }
    return ret;
}

TVector<IMappingSPtr> ParEGO::newMappings()  const
{
    TVector<IMappingSPtr> pop = operators().at(4)->outputSet(0)->all();
    TVector<IMappingSPtr> ret;
    for(int i=0; i<pop.size(); i++) {
        if(!pop[i]->isEvaluated()) {
            ret.push_back(pop[i]);
        }
    }
    return ret;
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

void ParEGO::defineBudgetPerVariable(int n)
{
    static_cast<SurrogateBasedOptimizer*>(operators().at(4))->
            TP_defineBudgetPerVariable(n);
}

int ParEGO::budgetPerVariable() const
{
    return static_cast<SurrogateBasedOptimizer*>(operators().at(4))->
            TP_budgetPerVariable();
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
