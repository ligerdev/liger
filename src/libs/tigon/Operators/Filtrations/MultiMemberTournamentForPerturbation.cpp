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
#include <tigon/Operators/Filtrations/MultiMemberTournamentForPerturbation.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Operators {

MultiMemberTournamentForPerturbation::MultiMemberTournamentForPerturbation()
{
    initialise();
}

MultiMemberTournamentForPerturbation::
MultiMemberTournamentForPerturbation(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

MultiMemberTournamentForPerturbation::~MultiMemberTournamentForPerturbation()
{

}

void MultiMemberTournamentForPerturbation::evaluateNode()
{
    // Init
    clearOutputSets();

    ISet* iSet = setsUnion(inputSets());
    int sSize = iSet->size();

    // Define the number of new solutions
    int nNewSolutions = std::ceil(sSize*m_ratioOfNewSolutions);

    ISet* oSet = appendOutputSet();
    for(int i=0; i<nNewSolutions; i++) {
        TVector<int> t;
        TVector<double> fitnessVals;
        for(int j=0; j<m_tournamentSize; j++) {
            int idx = TRAND.randInt(sSize);
            t.push_back(idx);
            fitnessVals.push_back(iSet->at(idx)->doubleCost());
        }
        TVector<int> sortedInd = indSort(fitnessVals);

        oSet->append(iSet->at(t[sortedInd[0]]));
    }
    delete iSet;
}

void MultiMemberTournamentForPerturbation::TP_defineRatioOfNewSolutions(double r)
{
    if(r > 0.0) {
        m_ratioOfNewSolutions = r;
    }
}

double MultiMemberTournamentForPerturbation::TP_ratioOfNewSolutions() const
{
    return m_ratioOfNewSolutions;
}

void MultiMemberTournamentForPerturbation::TP_defineTournamentSize(int s)
{
    if(s>=0) {
        m_tournamentSize = s;
    }
}

int MultiMemberTournamentForPerturbation::TP_tournamentSize() const
{
    return m_tournamentSize;
}

void MultiMemberTournamentForPerturbation::initialise()
{
    addProperty("RatioOfNewSolutions"
                , TString("Number of new solution created by the "
                          "operator divided by the number of solutions "
                          "in the input set. Default is 0.5")
                , getTType(double));

    addProperty("TournamentSize"
                , TString("Number of participants in each tournament. Default is 2.")
                , getTType(int));

    m_name = TString("Multi-member tournament selection for perturbation");
    m_description = TString("Randomly chooses n solutions from the input set, "
                            "and add best solutions to the output set.\n"
                            "The procedure is repeated according to the size "
                            "of the required set for perturbation.");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineRatioOfNewSolutions(0.5);
    TP_defineTournamentSize(2);

    addOutputTag(Tigon::TForPerturbation);
}


TString MultiMemberTournamentForPerturbation::name()
{
    return m_name;
}

TString MultiMemberTournamentForPerturbation::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
