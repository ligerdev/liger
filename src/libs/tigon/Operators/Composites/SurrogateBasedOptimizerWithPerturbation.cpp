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
#include <tigon/Operators/Composites/SurrogateBasedOptimizerWithPerturbation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>

#include <tigon/Operators/EmptyOperator.h>
#include <tigon/Operators/Perturbations/BoundedPerturbation.h>

namespace Tigon {
namespace Operators {

SurrogateBasedOptimizerWithPerturbation::SurrogateBasedOptimizerWithPerturbation()
{
    initialise();
}

SurrogateBasedOptimizerWithPerturbation::SurrogateBasedOptimizerWithPerturbation(IPSet *ipset)
    : SurrogateBasedOptimizer(ipset)
{
    initialise();
}

SurrogateBasedOptimizerWithPerturbation::~SurrogateBasedOptimizerWithPerturbation()
{

}

void SurrogateBasedOptimizerWithPerturbation::initialise()
{
    addProperty("NeighbourhoodRadius"
                , TString("Neighbourhood Radius")
                , getTType(double));

    // an invalid radius that keeps the RandExpectedImprovement default value:
    TP_defineNeighbourhoodRadius(Tigon::UseDefaultNeighbourhoodRadius);
}

void SurrogateBasedOptimizerWithPerturbation::evaluateNode()
{
    SurrogateBasedOptimizer::evaluateNode();

    ISet* oSet = outputSet(0);
    IMappingSPtr imap = oSet->at(oSet->size()-1);

    // create an additional IMapping which is a perturbation of this one.
    EmptyOperator* toPerturb = new EmptyOperator(this);
    toPerturb->defineOutputTags(TStringList({Tigon::TTempForMutation}));
    ISet* tempSet = toPerturb->appendOutputSet();
    IPSet::cloneMapping(tempSet, imap);
    removeOutputSet(1); //remove tempSet
    toPerturb->evaluateOnlyThisNode();

    BoundedPerturbation* mutat = new BoundedPerturbation(toPerturb);
    mutat->defineOutputTags(toPerturb->outputTags());
    mutat->TP_defineSolutionMutationProbability(1.0);
    double pertRadius = TP_neighbourhoodRadius() *
                        Tigon::PerturbationRadiusByNeighbourhoodRadius;
    mutat->TP_definePerturbationRadius(pertRadius);
    mutat->evaluateOnlyThisNode();

    for(auto sol : mutat->outputSet(0)->all()) {
        sol->defineWeightingVec(dirVec());
    }

    oSet->append(mutat->outputSet(0));

    delete mutat;
    delete toPerturb;
}

void SurrogateBasedOptimizerWithPerturbation::
TP_defineNeighbourhoodRadius(double r)
{
    m_neighbourhoodRadius = r;
}

double SurrogateBasedOptimizerWithPerturbation::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

TString SurrogateBasedOptimizerWithPerturbation::name()
{
    return TString("Surrogate-Based Optimization with Pertubation");
}

TString SurrogateBasedOptimizerWithPerturbation::description()
{
    return TString("Creates a surrogate model (Kriging) from the "
                   "existing solutions and optimizes over this model.\n"
                   "The optimal solution is appended to the current "
                   "set, together with a perturbation of it.\n");
}

} // namespace Operators
} // namespace Tigon
