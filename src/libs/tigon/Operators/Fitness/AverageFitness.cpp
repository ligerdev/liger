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
#include <tigon/Operators/Fitness/AverageFitness.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Elements/IElementOperations.h>

namespace Tigon {
namespace Operators {

AverageFitness::AverageFitness()
{
   initialise();
}

AverageFitness::AverageFitness(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    initialise();
}

AverageFitness::~AverageFitness()
{

}

void AverageFitness::initialise()
{
    clearOutputTags();
    clearAdditionalOutputTags();
    addOutputTag(Tigon::TFitness);
}

void AverageFitness::evaluateNode()
{
    while(hasNextOutputSet()) {

        ISet* oSet = nextOutputSet();

        // Get the ranks of the solutions into a vector
        // Assumes that the fitness of the solutions corresponds to their ranks
        TVector<double> ranks; ranks.reserve(oSet->size());
        for(IMappingSPtr imap : oSet->all()) {
            ranks.push_back(imap->doubleCost());
        }

        // Sort the solutions by rank and get their indices
        TVector<int> rsInd = Tigon::indSort(ranks);

        // Determine raw fitness
        int N = oSet->size(); // number of solutions
        TVector<int> rawFitness(N);
        for(int i=0; i<N; i++) {
            rawFitness[rsInd[i]] = N - i;
        }


        // Determine average fitness
        IElement currentRank = *oSet->at(rsInd[0])->cost();
        double sumRawFitness = rawFitness[rsInd[0]];
        int    solsPerRank = 1;
        TVector<double> averageFitness;
        for(int i=1; i<N; i++) {

            IElement nextRank = *oSet->at(rsInd[i])->cost();
            if(currentRank==nextRank) { // same rank
                sumRawFitness += rawFitness[rsInd[i]];
                solsPerRank++;
            } else { // next rank
                averageFitness.push_back(sumRawFitness/(double)solsPerRank);

                currentRank = nextRank;
                sumRawFitness = rawFitness[rsInd[i]];
                solsPerRank = 1;
            }
        }
        // For last rank (or in case there is just one rank)
        averageFitness.push_back(sumRawFitness/(double)solsPerRank);


        // Assign average fitness to solutions
        int fitnessCounter = 0;
        currentRank = *oSet->at(rsInd[0])->cost();
        oSet->at(rsInd[0])->defineCost(averageFitness[fitnessCounter]);
        for(int i=1; i<N; i++) {
            IElement nextRank = *oSet->at(rsInd[i])->cost();
            if(currentRank==nextRank) { // same rank
                oSet->at(rsInd[i])->defineCost(averageFitness[fitnessCounter]);
            } else { // next rank
                currentRank = nextRank;
                fitnessCounter++;
                oSet->at(rsInd[i])->defineCost(averageFitness[fitnessCounter]);
            }
        }
    }
}

TString AverageFitness::name()
{
    return TString("Average Fitness");
}


TString AverageFitness::description()
{
    TString desc("Determines the average fitness of each solution in the output "
                 "set. This is part of the fitness assignment procedure used by "
                 "the MOGA algorithm.\n"
                 "Pre-condition: the operator assumes that the fitness score of "
                 "the solutions corresponds to their own rank (e.g. as "
                 "determined by the MOGADominanceRanking operator).");
    return desc;
}

} // namespace Operators
} // namespace Tigon
