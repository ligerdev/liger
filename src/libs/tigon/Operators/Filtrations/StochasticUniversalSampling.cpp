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
#include <tigon/Operators/Filtrations/StochasticUniversalSampling.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Operators {


StochasticUniversalSampling::StochasticUniversalSampling()
{
    initialise();
}

StochasticUniversalSampling::StochasticUniversalSampling(
        Tigon::Representation::IPSet *ipset) : RouletteWheelSelection(ipset)
{
    initialise();
}

StochasticUniversalSampling::~StochasticUniversalSampling()
{

}

void StochasticUniversalSampling::evaluateNode()
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

        double singleRandomNumber = TRAND.randUni();
        TVector<double> R(outputSetSize);
        R[0] = singleRandomNumber;
        for(int i=1; i<outputSetSize; i++) {
            double frac = (double)i/(double)outputSetSize;
            R[i] = std::fmod(singleRandomNumber + frac, 1.0);
        }

        for(int i=0; i<outputSetSize; i++) {

            int selectedIndex = N-1;
            double randomNumber = R[i];

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

void StochasticUniversalSampling::initialise()
{

}

TString StochasticUniversalSampling::name()
{
    return TString("Stochastic Universal Sampling (SUS)");
}

TString StochasticUniversalSampling::description()
{
    return TString("Selection operator that uses the Stochastic Universal "
                   "Sampling (SUS) approach (Baker, 1985).\n"
                   "The operator selects a set of solutions from the input set "
                   "based on their fitness by making use of the roulette-wheel "
                   "concept, where the random numbers are chosen by the SUS"
                   "approach.");
}

} // namespace Operators
} // namespace Tigon
