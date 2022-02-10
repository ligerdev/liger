/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Filtrations/NSGAIIEliteSelection.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {


NSGAIIEliteSelection::NSGAIIEliteSelection()
{
    initialise();
}

NSGAIIEliteSelection::NSGAIIEliteSelection(
        Tigon::Representation::IPSet* ipset): IFiltration(ipset)
{
    initialise();
}

NSGAIIEliteSelection::~NSGAIIEliteSelection()
{

}

void NSGAIIEliteSelection::initialise()
{
    addInputTag(Tigon::TFitness);
    addInputTag(Tigon::TForSelection);
    addOutputTag(Tigon::TForNextIteration);

    addProperty("EliteRatio"
                , TString("The ratio of solutions from the main population to be "
                          "used for the elite population.\n"
                          "Default is 0.5 (meaning half of the main population).")
                , getTType(double));

    TP_defineEliteRatio(0.5);
}

double NSGAIIEliteSelection::TP_eliteRatio() const
{
    return m_eliteRatio;
}


void NSGAIIEliteSelection::TP_defineEliteRatio(double r)
{
    if(isInInterval(r,0,1)) {
        m_eliteRatio = r;
    }
}

void NSGAIIEliteSelection::evaluateNode()
{
    // Init
    clearOutputSets();  //overrides the data from previous iteration
    ISet* oSet = appendOutputSet();

    // Define the elite size
    TVector<ISet*> allRanks = inputSets();
    int popSize = 0;
    int rankSize = allRanks.size();
    for(int r=0; r<rankSize; r++) {
        popSize += allRanks.at(r)->size();
    }
    int eliteSize = static_cast<int>(std::ceil(popSize*m_eliteRatio));

    // Copy the first j ranks
    ISet* iSet;
    while( (oSet->size() < eliteSize) && hasNextInputSet()) {
        if( peepNextInputSet()->size() <= (eliteSize - oSet->size()) ) {
            iSet = nextInputSet();
            oSet->append(iSet);  // adds a set to oSet
        } else {
            break;
        }
    }

    // copy the remaining solutions from the next rank
    int missingSolutions = eliteSize - oSet->size();
    if( (missingSolutions > 0) && hasNextInputSet()) {

        // definitions
        TVector<double> costVector; // vector of the cost of every solution
        TVector<int> rsInd;         // vector of the sorted indices
        IMappingSPtr iSolution;    // pointer to a solution
        double cost;
        int subsetSize;

        iSet = nextInputSet();  // last rank

        subsetSize = iSet->size();

        // make a vector of the cost of every solution
        for(int i=0; i<subsetSize; i++) {
            cost = iSet->at(i)->doubleCost();
            costVector.push_back(cost);
        }

        // use indSort to sort this vector and get the indices
        rsInd = Tigon::indSort(costVector);

        // append only the missing solutions
        for(int i=0; i<missingSolutions; i++) {
            iSolution = iSet->at(rsInd[i]);
            oSet->append(iSolution);
        }
    }
}

TString NSGAIIEliteSelection::name()
{
    return TString("NSGA-II Elite Selection");
}

TString NSGAIIEliteSelection::description()
{
    return TString("Creates a set with the best solutions selected from sets of non-dominace ranks.\n"
                   "The first j ranks that are smaller than the elite size are selected.\n"
                   "The remaining p solutions from the next rank are selected based on the cost function\n");
}
} // namespace Operators
} // namespace Tigon
