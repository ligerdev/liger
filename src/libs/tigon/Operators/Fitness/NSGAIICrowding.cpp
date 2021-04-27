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
#include <tigon/Operators/Fitness/NSGAIICrowding.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

NSGAIICrowding::NSGAIICrowding()
{
    initialise();
}

NSGAIICrowding::NSGAIICrowding(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    initialise();
}

NSGAIICrowding::~NSGAIICrowding()
{

}

void NSGAIICrowding::initialise()
{
    clearOutputTags();
    clearAdditionalOutputTags();
    addOutputTag(Tigon::TFitness);
}

void NSGAIICrowding::evaluateNode()
{
    int nobj = objectiveVecSize();

    while(hasNextOutputSet()) {
        ISet* rank = nextOutputSet();
        int rankSize = rank->size();

        TVector <double> cDist(rankSize, 0.0);
        TVector <double> objVector(rankSize);

        for(int k=0; k<nobj; k++) {    // for each objective

            // copy population objective values to one vector for sorting
            // get maximum and minimum objective values
            for(int i=0; i<rankSize; i++) {
                objVector[i] = rank->at(i)->doubleObjectiveVar(k);
            }
            double fmax = vectorMax(objVector);
            double fmin = vectorMin(objVector);

            // sort the objective values
            TVector<int> rsInd = Tigon::indSort(objVector);

            // set crowding for extreme solutions
            cDist[rsInd[0]]=0.0;
            cDist[rsInd[rankSize-1]]=0.0;

            // set crowding for remaining solutions
            for(int i=1; i<rankSize-1; i++) {
                double currrentCost;
                if( (fmax-fmin)<Tigon::Epsilon ) {
                    currrentCost = 0.0;
                }
                else {
                    double solA = rank->at(rsInd[i+1])->doubleObjectiveVar(k);
                    double solB = rank->at(rsInd[i-1])->doubleObjectiveVar(k);
                    // normalise in the interval [0,1]
                    currrentCost = 1.0 - (solA-solB)/(fmax-fmin);
                }
                cDist[rsInd[i]] += currrentCost;
            }
        }
        // normalise the crowding distance value by dividing by the number of
        // objectives
        std::transform(cDist.begin(), cDist.end(), cDist.begin(),
                       std::bind1st(std::multiplies<double>(), 1.0/nobj));

        for(int i=0; i<rankSize; i++) {
            double prevCost = rank->at(i)->doubleCost();
            rank->at(i)->defineCost(prevCost + cDist[i]);
        }
    }
}

TString NSGAIICrowding::name()
{
    return TString("Crowding Distance");
}


TString NSGAIICrowding::description()
{
    TString desc("Assigns crowding distance according to NSGA-II "
                 "(Deb et al. 2002). This is an estimate of the density of "
                 "solutions surrounding a particular solution. Consists in "
                 "estimating the perimeter of the cuboid formed by using the "
                 "nearest neighbours of one solutions as the vertices.");
    return desc;
}

} // namespace Operators
} // namespace Tigon
