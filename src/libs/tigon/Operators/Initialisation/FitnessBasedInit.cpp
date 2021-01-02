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
#include <tigon/Operators/Initialisation/FitnessBasedInit.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Operators/Initialisation/LHSInit.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

FitnessBasedInit::FitnessBasedInit()
{
    initialise();
}

FitnessBasedInit::FitnessBasedInit(Representation::IPSet* ipset)
    : IInitialisation(ipset)
{
    initialise();
}

FitnessBasedInit::~FitnessBasedInit()
{

}

void  FitnessBasedInit::defineOriginalSolutionsRatio(double ratio)
{
    if(ratio >= 0.0 &&  ratio <= 1.0) {
        m_existingRatio = ratio;
    }
}

double FitnessBasedInit::originalSolutionsRatio() const
{
    return m_existingRatio;
}

void FitnessBasedInit::evaluateNode()
{
    ISet* iSet = inputSet(0);
    if(iSet == nullptr) {
        // throw an error
        return;
    }

    clearOutputSets();
    ISet* oSet = appendOutputSet();

    // Sort inputs by fintness
    TVector<double> costVec(iSet->size());
    for(int i=0; i<iSet->size(); i++) {
        costVec[i] = iSet->at(i)->doubleCost();
    }
    TVector<int> idx = indSort(costVec);

    // Select the IMappings with smaller cost
    int bestSize = static_cast<int>(std::round(TP_setSize() * m_existingRatio));
    bestSize = std::min(bestSize, iSet->size());
    for(int i=0; i<bestSize; i++) {
        cloneMapping(iSet->at(idx[i]));
    }

    // Mutate the selected IMappings
    tagSet(oSet, Tigon::TTempForMutation);
    PolynomialMutation* mutat = new PolynomialMutation(this);
    mutat->defineOutputTags(TStringList({Tigon::TTempForMutation}));
    mutat->TP_defineSolutionMutationProbability(1.0);
    mutat->TP_defineVariableMutationProbability(1.0);
    mutat->evaluateOnlyThisNode();
    removeTagFromSet(oSet, Tigon::TTempForMutation);

    // Complete the set with latin hypercube sampling
    LHSInit* LHS = new LHSInit(this);
    LHS->clearOutputTags();
    LHS->TP_defineSetSize(TP_setSize() - bestSize);
    LHS->evaluateOnlyThisNode();

    oSet->append(LHS->outputSet(0));

    appendInitialSet();

    delete LHS;
    delete mutat;
}

TString FitnessBasedInit::name()
{
    return TString("Surrogate-Based Initialisation");
}

TString FitnessBasedInit::description()
{
    return TString("Generate decision vectors to search a surrogate "
                   "model constructed from real function evaluations.\n"
                   "Based on the algorithm described in J. Knowles "
                   "ParEGO paper (2005).\n"
                   "The initial population is composed of some "
                   "perturbations of the solutions with the best "
                   "fitness from the original solutions, and the rest "
                   "are generated using Latin hypercube sampling.");
}

void FitnessBasedInit::initialise()
{
    clearOutputTags();

    defineOriginalSolutionsRatio(0.25);
}

} // namespace Operators
} // namespace Tigon
