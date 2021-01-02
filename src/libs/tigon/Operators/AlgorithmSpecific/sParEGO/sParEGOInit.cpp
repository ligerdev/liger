/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/AlgorithmSpecific/sParEGO/sParEGOInit.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Initialisation/LHSInit.h>
#include <tigon/Operators/Filtrations/RandFiltrationForPerturbation.h>
#include <tigon/Operators/Perturbations/BoundedPerturbation.h>
#include <tigon/Operators/Filtrations/FreezeOperatorOutput.h>
#include <tigon/tigonengineregistry.h>

namespace Tigon {
namespace Operators {

sParEGOInit::sParEGOInit()
{
    initialise();
}

sParEGOInit::sParEGOInit(Representation::IPSet* ipset)
    : IInitialisation(ipset)
{
    initialise();
}

sParEGOInit::~sParEGOInit()
{

}

void sParEGOInit::TP_definePerturbedRatio(double ratio)
{
    if(ratio >= 0.0 &&  ratio < 1.0) {
        m_perturbRatio = ratio;
    }
}

double sParEGOInit::TP_perturbedRatio() const
{
    return m_perturbRatio;
}

void sParEGOInit::TP_defineNeighbourhoodRadius(double r)
{
    if(r > 0.0 && r <= 1.0) {
        m_neighbourhoodRadius = r;
    }
}

double sParEGOInit::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

void sParEGOInit::evaluateNode()
{
    if( outputSets().empty() ) {
        ISet* oSet = appendOutputSet();

        LHSInit* LHS   = new LHSInit(this);
        LHS->addOutputTag(Tigon::TForFiltration);

        FreezeOperatorOutput* LHSClone = new FreezeOperatorOutput(this);
        LHSClone->defineInputTags(LHS->outputTags());
        LHSClone->defineOutputTags(TStringList({Tigon::TTempForMutation}));

        BoundedPerturbation*
                pert1 = new BoundedPerturbation(this);
        pert1->defineOutputTags(LHSClone->outputTags());

        RandFiltrationForPerturbation*
                filt = new RandFiltrationForPerturbation(this);
        BoundedPerturbation*
                pert2 = new BoundedPerturbation(this);

        LHS->TP_defineSetSize(std::ceil(TP_setSize()*(1.0-m_perturbRatio)/2.0));
        filt->TP_defineRatioOfNewSolutions(
                    std::ceil(2.0*m_perturbRatio/(1.0-m_perturbRatio)));
        pert1->TP_definePerturbationRadius(m_neighbourhoodRadius *
                                           Tigon::PerturbationRadiusByNeighbourhoodRadius);
        pert2->TP_definePerturbationRadius(m_neighbourhoodRadius *
                                           Tigon::PerturbationRadiusByNeighbourhoodRadius);
        // perturb all variables
        pert1->TP_defineSolutionMutationProbability(1.0);
        pert2->TP_defineSolutionMutationProbability(1.0);

        // use evaluateOnlyThisNode() instead of evaluate() to avoid
        // re-evaluating this node and the previous ones
        LHS->evaluateOnlyThisNode();
        LHSClone->evaluateOnlyThisNode();
        filt->evaluateOnlyThisNode();
        pert1->evaluateOnlyThisNode();
        pert2->evaluateOnlyThisNode();

        oSet->append(LHS->outputSet(0));
        oSet->append(pert1->outputSet(0));
        oSet->append(pert2->outputSet(0));

        while(oSet->size() > TP_setSize()) {
            oSet->remove(oSet->size() - 1);
        }

        delete pert2;
        delete filt;
        delete pert1;
        delete LHSClone;
        delete LHS;

        appendInitialSet();

    } else {
        appendInitialSet();
    }
}

TString sParEGOInit::name()
{
    return TString("sParEGO Initialisation");
}

TString sParEGOInit::description()
{
    return TString("Generate an initial set of candidate solutions. "
                   "A portion of the set is generated using Latin "
                   "hypercube sampling, and the rest of the set are "
                   "perturbations of randomly selected member first "
                   "portion.");
}

void sParEGOInit::initialise()
{
    addProperty("PerturbedRatio"
                , TString("Ratio of perturbed solutions from the entire set size.")
                , getTType(double));

    addProperty("NeighbourhoodRadius"
                , TString("Maximum distance between solutions to be "
                          "cosidered as neighbours.\nTo account for "
                          "different scales, the distance is multiplied "
                          "by sqrt(d), where d is the number of decision "
                          "variables.\nThe default is ") +
                  std::to_string(Tigon::DefaultsParEGOInitPerturbationRatio)
                , getTType(double));

    TP_definePerturbedRatio(Tigon::DefaultsParEGOInitPerturbationRatio);
    TP_defineNeighbourhoodRadius(Tigon::DefaultNeighbourhoodRadius);
}

} // namespace Operators
} // namespace Tigon
REGISTER_IPSET_FACTORY(Operators,sParEGOInit)
