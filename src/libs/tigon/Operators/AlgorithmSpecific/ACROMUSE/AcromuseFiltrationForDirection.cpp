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
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForDirection.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Random/RandomGenerator.h>

#include <cstdlib>
#include <ctime>

namespace Tigon {
namespace Operators {

AcromuseFiltrationForDirection::AcromuseFiltrationForDirection()
{
    m_name = TString("ACROMUSE filtration for direction");
    m_description = TString("A multi-member tournament for direction, with the "
                            "tournament size and cross-over ratio determined "
                            "according to the diversity measures SPD and HPD.\n"
                            "Randomly chooses n solutions from the input set, "
                            "and choose the best k solutions.\n"
                            "Creates sets of k solutions for direction operators.");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    defineRatioOfNewSolutions(0.5);
    defineOutputSetSize(2);
    defineTournamentSize(2);

    addOutputTag(Tigon::TForDirection);
    addOutputTag(Tigon::TForNextIteration);
}

AcromuseFiltrationForDirection::
AcromuseFiltrationForDirection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    m_name = TString("ACROMUSE filtration for direction");
    m_description = TString("A multi-member tournament for direction, with the "
                            "tournament size and cross-over ratio determined "
                            "according to the diversity measures SPD and HPD.\n"
                            "Randomly chooses n solutions from the input set, "
                            "and choose the best k solutions.\n"
                            "Creates sets of k solutions for direction operators.");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    defineRatioOfNewSolutions(0.5);
    defineOutputSetSize(2);
    defineTournamentSize(2);

    addOutputTag(Tigon::TForDirection);
    addOutputTag(Tigon::TForNextIteration);
}

AcromuseFiltrationForDirection::~AcromuseFiltrationForDirection()
{

}

void AcromuseFiltrationForDirection::evaluateNode()
{
    // Init
    clearOutputSets();

    ISet* optSet = inputSet(0);
    ISet* genSet = inputSet(1);
    int sSize = genSet->size();

    // Adaptive parameter setting
    if(multiPackExists(Tigon::AcSpd)) {
        double SPD = multiPack(Tigon::AcSpd)[0];
        double pc = Tigon::AcK1 + SPD / Tigon::AcMaxSpd * (Tigon::AcK2 - Tigon::AcK1);
        // define
        defineRatioOfNewSolutions(pc);
    }

    if(multiPackExists(Tigon::AcHpd)) {
        double maxTSize = sSize / 6.0;
        double HPD = multiPack(Tigon::AcHpd)[0];
        int tSize = std::ceil( std::min(HPD / Tigon::AcMaxHpd, 1.0) * maxTSize);
        defineTournamentSize(tSize);
    }
    // Define the number of new solutions
    // One solution is selected for elite, so the ratio from sSize - 1 is used
    int nNewSolutions = std::ceil((sSize-1)*m_ratioOfNewSolutions);
    int nSets = nNewSolutions / m_outputSetSize;

    for(int i=0; i<nSets; i++) {
        appendOutputSet();
        for(int j=0; j<m_outputSetSize; j++) {
            int idx = tournament(genSet);
            cloneMapping(optSet->at(idx));
        }
    }

    int lastSetSize = nNewSolutions % m_outputSetSize;
    if(lastSetSize) {
        appendOutputSet();
        for(int j=0; j<lastSetSize; j++) {
            int idx = tournament(genSet);
            cloneMapping(optSet->at(idx));
        }
    }
}

int AcromuseFiltrationForDirection::tournament(ISet* set) const
{
    int sSize = set->size();
    int n = std::min(m_tournamentSize, sSize);
    //TODO - the tournament size shouldn't be zero. It happens because the HPD
    //       value is 0. This is caused by normailsation of fitness values of
    //       different scales, such that one solution is scaled to 1.0, and the
    //       others to 0.0
    //       After fixing the kriging error estimate, check if the following
    //       line can be removed.
    n = std::max(n, 1);

    TVector<int> t;
    TVector<double> fitnessVals;
    for(int j=0; j<n; j++) {
        int idx = TRAND.randInt(sSize);
        t.push_back(idx);
        fitnessVals.push_back(set->at(idx)->doubleCost());
    }
    TVector<int> sortedInd = indSort(fitnessVals);

    int winner = t[sortedInd[0]];
    return winner;
}

void AcromuseFiltrationForDirection::defineRatioOfNewSolutions(double r)
{
    if(r > 0.0) {
        m_ratioOfNewSolutions = r;
    }
}

double AcromuseFiltrationForDirection::ratioOfNewSolutions() const
{
    return m_ratioOfNewSolutions;
}

void AcromuseFiltrationForDirection::defineOutputSetSize(int n) {
    if(n>0) {
        m_outputSetSize = n;
    }
}

int  AcromuseFiltrationForDirection::outputSetSize() const {
    return m_outputSetSize;
}

void AcromuseFiltrationForDirection::defineTournamentSize(int s)
{
    if(s>=0) {
        m_tournamentSize = s;
    }
}

int AcromuseFiltrationForDirection::tournamentSize() const
{
    return m_tournamentSize;
}


TString AcromuseFiltrationForDirection::name()
{
    return m_name;
}

TString AcromuseFiltrationForDirection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
