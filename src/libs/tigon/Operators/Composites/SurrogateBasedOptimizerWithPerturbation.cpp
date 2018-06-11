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
#include <tigon/Operators/Composites/SurrogateBasedOptimizerWithPerturbation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/Kriging.h>
#include <tigon/Utils/PowerVariogram.h>
#include <tigon/Representation/Functions/ExpectedImprovement.h>
#include <tigon/Representation/Functions/RandExpectedImprovement.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Initialisation/FitnessBasedInit.h>
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/EmptyOperator.h>
#include <tigon/Operators/Perturbations/BoundedPerturbation.h>
#include <tigon/Algorithms/ACROMUSE.h>
#include <tigon/Representation/Properties/ElementProperties.h>

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
    mutat->TP_definePerturbationRadius(TP_neighbourhoodRadius() *
                                       Tigon::PerturbationRadiusByNeighbourhoodRadius);
    mutat->evaluateOnlyThisNode();

    for(auto sol : mutat->outputSet(0)->all()) {
        sol->defineWeightingVec(dirVec());
    }

    oSet->append(mutat->outputSet(0));

    delete mutat;
    delete toPerturb;
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
