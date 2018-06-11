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
#include <tigon/Operators/Filtrations/MultiMemberTournamentForDirection.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Random/RandomGenerator.h>



#include <cstdlib>
#include <ctime>


namespace Tigon {
namespace Operators {

MultiMemberTournamentForDirection::MultiMemberTournamentForDirection()
{
    addProperty("RatioOfNewSolutions"
                , TString("Number of new solution created by the "
                          "operator divided by the number of solutions "
                          "in the input set. Default is 0.5")
                , getType(double));
    addProperty("OutputSetSize"
                , TString("Number of solutions to select for each "
                          "sub-set. Default is 2.")
                , getType(int));
    addProperty("TournamentSize"
                , TString("Number of participants in each tournament.\n"
                          "Default is 2.")
                , getType(int));

    m_name = TString("Multi-member tournament selection for direction");
    m_description = TString("Randomly chooses n solutions from the input set, "
                            "and choose the best k solutions.\n"
                            "Creates sets of k solutions for direction operators.");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineRatioOfNewSolutions(0.5);
    TP_defineOutputSetSize(2);
    TP_defineTournamentSize(2);

    addOutputTag(Tigon::TForDirection);
}

MultiMemberTournamentForDirection::
MultiMemberTournamentForDirection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{

    addProperty("RatioOfNewSolutions"
                , TString("Number of new solution created by the "
                          "operator divided by the number of solutions "
                          "in the input set. Default is 0.5")
                , getType(double));
    addProperty("OutputSetSize"
                , TString("Number of solutions to select for each "
                          "sub-set. Default is 2.")
                , getType(int));
    addProperty("TournamentSize"
                , TString("Number of participants in each tournament.\n"
                          "Default is 2.")
                , getType(int));

    m_name = TString("Multi-member tournament selection for direction");
    m_description = TString("Randomly chooses n solutions from the input set, "
                            "and choose the best k solutions.\n"
                            "Creates sets of k solutions for direction operators.");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineRatioOfNewSolutions(0.5);
    TP_defineOutputSetSize(2);
    TP_defineTournamentSize(2);

    addOutputTag(Tigon::TForDirection);
}

MultiMemberTournamentForDirection::~MultiMemberTournamentForDirection()
{

}

void MultiMemberTournamentForDirection::evaluateNode()
{
    // Init
    clearOutputSets();

    ISet* iSet = setsUnion(inputSets());
    int sSize = iSet->size();

    // Define the number of new solutions
    int nNewSolutions = std::ceil(sSize*m_ratioOfNewSolutions);
    int nSets = nNewSolutions / m_outputSetSize;

    for(int i=0; i<nSets; i++) {
        TVector<int> t;
        TVector<double> fitnessVals;
        for(int j=0; j<m_tournamentSize; j++) {
            int idx = TRAND.randInt(sSize);
            t.push_back(idx);
            fitnessVals.push_back(iSet->at(idx)->doubleCost());
        }
        TVector<int> sortedInd = indSort(fitnessVals);

        ISet* oSet = appendOutputSet();
        for(int j=0; j<m_outputSetSize; j++) {
            oSet->append(iSet->at(t[sortedInd[j]]));
        }
    }
    delete iSet;
}

void MultiMemberTournamentForDirection::TP_defineRatioOfNewSolutions(double r)
{
    if(r > 0.0) {
        m_ratioOfNewSolutions = r;
    }
}

double MultiMemberTournamentForDirection::TP_ratioOfNewSolutions() const
{
    return m_ratioOfNewSolutions;
}

void MultiMemberTournamentForDirection::TP_defineOutputSetSize(int n) {
    if(n>0) {
        m_outputSetSize = n;
    }
}

int  MultiMemberTournamentForDirection::TP_outputSetSize() const {
    return m_outputSetSize;
}

void MultiMemberTournamentForDirection::TP_defineTournamentSize(int s)
{
    if(s>=0) {
        m_tournamentSize = s;
    }
}

int MultiMemberTournamentForDirection::TP_tournamentSize() const
{
    return m_tournamentSize;
}


TString MultiMemberTournamentForDirection::name()
{
    return m_name;
}

TString MultiMemberTournamentForDirection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
