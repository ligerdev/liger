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
#include <tigon/Operators/Filtrations/TournamentFiltrationForModification.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

TournamentFiltrationForModification::TournamentFiltrationForModification()
{
    addProperty("ChildrenRatio"
                , TString("Proportion of the Children solutions of the total population. Default is half.")
                , getTType(double));

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineChildrenRatio(0.5);
    addOutputTag(Tigon::TForFiltration);
}

TournamentFiltrationForModification::TournamentFiltrationForModification(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    addProperty("ChildrenRatio"
                , TString("Proportion of the Children solutions of the total population. Default is half.")
                , getTType(double));

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineChildrenRatio(0.5);
    addOutputTag(Tigon::TForFiltration);
}

TournamentFiltrationForModification::~TournamentFiltrationForModification()
{

}

void TournamentFiltrationForModification::evaluateNode()
{
    // Init
    clearOutputSets();  //overrides the data from previous iteration
    appendOutputSet();

    ISet* iSet = setsUnion(inputSets());

    // Define the children set size
    int popSize = iSet->size();
    int childrenSize = std::floor(popSize*m_childrenRatio); //The elite uses std::ceil()

    for(int i=0; i<childrenSize; i++) {
        int idxA = TRAND.randInt(popSize);
        int idxB = TRAND.randInt(popSize);
        if(iSet->at(idxA)->cost() < iSet->at(idxB)->cost()) {
            cloneMapping(iSet->at(idxA));
        } else {
            cloneMapping(iSet->at(idxB));
        }
    }
    delete iSet;
}

void TournamentFiltrationForModification::TP_defineChildrenRatio (double r)
{
    m_childrenRatio = r;
}

double TournamentFiltrationForModification::TP_childrenRatio() const
{
    return m_childrenRatio;
}

TString TournamentFiltrationForModification::name()
{
    return "Tournament selection for modification";
}

TString TournamentFiltrationForModification::description()
{
    TString description = TString("Creates a set of solutions selected from a set tagged \"for selection\".\n");
    description += TString("The created set is tagged \"for modification\".\n");
    description += TString("Until the set is populated, two solutions from the input set are randomly selected,");
    description += TString("and the solution with the lower cost function is appended to the set.");
    description += TString("A solution may be selected more than once.\n");
    description += TString("The IMappings in the set are clones of the selected IMappings.");

    return description;
}

} // namespace Operators
} // namespace Tigon
