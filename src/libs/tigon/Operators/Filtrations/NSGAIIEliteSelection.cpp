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
    int i;
    int r;
    int popSize;
    int rankSize;
    int eliteSize;
    int missingSolutions;

    // Init
    clearOutputSets();  //overrides the data from previous iteration
    ISet* oSet = appendOutputSet();

    // Define the elite size
    TVector<ISet*> allRanks = inputSets();
    popSize = 0;
    rankSize = allRanks.size();
    for(r=0; r<rankSize; r++) {
        popSize += allRanks.at(r)->size();
    }
    eliteSize = static_cast<int>(std::ceil(popSize*m_eliteRatio));

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
    missingSolutions = eliteSize - oSet->size();
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
        for(i=0; i<subsetSize; i++) {
            cost = iSet->at(i)->doubleCost();
            costVector.push_back(cost);
        }

        // use indSort to sort this vector and get the indices
        rsInd = Tigon::indSort(costVector);

        // append only the missing solutions
        for(i=0; i<missingSolutions; i++) {
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
