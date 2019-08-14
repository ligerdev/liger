/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Filtrations/RouletteWheelSelection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Random/RandomGenerator.h>
#include <numeric>

namespace Tigon {
namespace Operators {

RouletteWheelSelection::RouletteWheelSelection()
{
    initialise();
}

RouletteWheelSelection::RouletteWheelSelection(
        Tigon::Representation::IPSet *ipset): IFiltration(ipset)
{
    initialise();
}

RouletteWheelSelection::~RouletteWheelSelection()
{

}

TVector<double> RouletteWheelSelection::cumulativeProbabilityDistribution(
        const TVector<double> &fitness)
{
    double sumF = std::accumulate(fitness.begin(), fitness.end(), 0.0);

    int N = fitness.size();
    TVector<double> P(N);
    double cummulativeProbability = 0.0;
    for(int i=0; i<N; i++) {
        double pi = fitness[i]/sumF;
        cummulativeProbability += pi;
        P[i] = cummulativeProbability;
    }

    return P;
}

void RouletteWheelSelection::evaluateNode()
{
    clearOutputSets();

    while(hasNextInputSet()) {

        ISet* oSet = appendOutputSet();
        ISet* iSet = nextInputSet();
        int N = iSet->size();
        int outputSetSize = (TP_outputSetSize() < 0) ? N : TP_outputSetSize();

        TVector<double> fitness; fitness.reserve(N);
        for(IMappingSPtr sol : iSet->all()) {
            fitness.push_back(sol->doubleCost());
        }

        TVector<double> P = cumulativeProbabilityDistribution(fitness);

        for(int i=0; i<outputSetSize; i++) {

            int selectedIndex = N-1;
            double randomNumber = TRAND.randUni();

            for(int j=0; j<N; j++) {
                if( randomNumber <= P[j] ) {
                    selectedIndex = j;
                    break;
                }
            }

            oSet->append(iSet->at(selectedIndex));
        }
    }
}

void RouletteWheelSelection::TP_defineOutputSetSize(int n)
{
    if(n > 0) {
        m_outputSetSize = n;
    }
}

int RouletteWheelSelection::TP_outputSetSize() const
{
    return m_outputSetSize;
}

void RouletteWheelSelection::initialise()
{
    addProperty("OutputSetSize"
                , TString("Number of solutions in the output set.\n"
                          "Default is -1, meaning that the size of the output "
                          "set is set to be equal to the size of the input set.")
                , getType(int));

    m_outputSetSize = -1;

    addInputTag(Tigon::TFitness);
    addInputTag(Tigon::TForSelection);
    addOutputTag(Tigon::TMatingPool);
}

TString RouletteWheelSelection::name()
{
    return TString("Roulette Wheel Selection (RWS)");
}

TString RouletteWheelSelection::description()
{
    return TString("Selection operator that uses the roulette-wheel approach.\n"
                   "The operator selects a set of solutions from the input set "
                   "based on their fitness by making use of the roulette-wheel "
                   "concept, where the random numbers are sampled from a "
                   "standard uniform distribution.");
}

} // namespace Operators
} // namespace Tigon
