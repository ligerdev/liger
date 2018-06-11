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
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseFiltrationForPerturbation.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Random/RandomGenerator.h>

using namespace std;

namespace Tigon {
namespace Operators {

AcromuseFiltrationForPerturbation::AcromuseFiltrationForPerturbation()
{
    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    defineRatioOfNewSolutions(0.5);
    defineTournamentSize(2);

    addOutputTag(Tigon::TForPerturbation);
    addOutputTag(Tigon::TForNextIteration);
}

AcromuseFiltrationForPerturbation::
AcromuseFiltrationForPerturbation(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    defineRatioOfNewSolutions(0.5);
    defineTournamentSize(2);

    addOutputTag(Tigon::TForPerturbation);
}

AcromuseFiltrationForPerturbation::~AcromuseFiltrationForPerturbation()
{

}

void AcromuseFiltrationForPerturbation::evaluateNode()
{
    // Init
    clearOutputSets();

    ISet* iOptSet = inputSet(0);
    ISet* iGenSet = inputSet(1);
    int sSize = iOptSet->size();

    // Adaptive parameter setting
    if(multiPackExists(Tigon::AcSpd)) {
        double SPD = multiPack(Tigon::AcSpd)[0];
        double pc = Tigon::AcK1 + SPD / Tigon::AcMaxSpd * (Tigon::AcK2 - Tigon::AcK1);
        // define
        defineRatioOfNewSolutions(1.0 - pc);
    }

    if(multiPackExists(Tigon::AcHpd)) {
        double maxTSize = sSize / 6.0;
        double HPD = multiPack(Tigon::AcHpd)[0];
        int tSize = static_cast<int>(ceil( min(HPD / Tigon::AcMaxHpd, 1.0) * maxTSize));
        defineTournamentSize(tSize);
    }

    // Define the number of new solutions
    // One solution is selected for elite, so the ratio from sSize - 1 is used
    int nNewSolutions = static_cast<int>(floor((sSize-1)*m_ratioOfNewSolutions));

    ISet* oOptSet = appendOutputSet();
    ISet* oGenSet = appendOutputSet();

    for(int i=0; i<nNewSolutions; i++) {
        int idx = tournament(iGenSet);
        IPSet::cloneMapping(oOptSet, iOptSet->at(idx));
        oGenSet->append(iGenSet->at(idx));
    }
}

int AcromuseFiltrationForPerturbation::tournament(ISet* set) const
{
    int sSize = set->size();
    int n = min(m_tournamentSize, sSize);
    n = max(n, 1);

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

void AcromuseFiltrationForPerturbation::defineRatioOfNewSolutions(double r)
{
    if(r > 0.0) {
        m_ratioOfNewSolutions = r;
    }
}

double AcromuseFiltrationForPerturbation::ratioOfNewSolutions() const
{
    return m_ratioOfNewSolutions;
}

void AcromuseFiltrationForPerturbation::defineTournamentSize(int s)
{
    if(s>=0) {
        m_tournamentSize = s;
    }
}

int AcromuseFiltrationForPerturbation::tournamentSize() const
{
    return m_tournamentSize;
}


TString AcromuseFiltrationForPerturbation::name()
{
    return m_name;
}

TString AcromuseFiltrationForPerturbation::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
