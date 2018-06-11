#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Operators/Filtrations/SMSEMOAReduce.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Utils/HypervolumeUtils.h>

namespace Tigon {
namespace Operators {

SMSEMOAReduce::SMSEMOAReduce()
{
    //SMSEMOAReduce::initialise();
    initialise();
}

SMSEMOAReduce::SMSEMOAReduce(Tigon::Representation::IPSet *ipset)
    :IFiltration(ipset)
{
    //SMSEMOAReduce::initialise();
    initialise();
}

SMSEMOAReduce::~SMSEMOAReduce()
{

}

void SMSEMOAReduce::initialise()
{
    addInputTag(Tigon::TReducePopulation);

    addOutputTag(Tigon::TForNextIteration);
    addOutputTag(Tigon::TForSelection);
}

void SMSEMOAReduce::evaluateNode()
{
    clearOutputSets();
    ISet* oSet= appendOutputSet();

    TVector<IMappingSPtr> solutionVector;
    TVector<double> rankVector;

    while(hasNextInputSet()) {
        ISet* rank = nextInputSet();
        int rankSize = rank->size();
        for(int i=0; i<rankSize; i++) {
            double r = rank->at(i)->cost()->value<double>();
            solutionVector.push_back(rank->at(i));
            rankVector.push_back(r);
        }
        
    }

    // In first iteration just get the MainOptimizationSet and pass it over
    if (this->currentIteration() < 1) {
        oSet->define(solutionVector);
        return;
    }

    //Compute the worst rank set
    double maxRank = *std::max_element(rankVector.begin(),
                                       rankVector.end());
    ISet* worstRankedSet = new ISet;
    if (maxRank < 1) {
        worstRankedSet->define(solutionVector);
    } else {
        for (int i=0; i<rankVector.size(); i++) {
            if(areDoublesEqual(rankVector[i], maxRank)) {
                worstRankedSet->append(solutionVector.at(i));
            }
        }
    }

    // Calculate the ideal point for the ref point
    double maxvalue =0.0;
    for(IMappingSPtr imap : worstRankedSet->all()) {
        TVector<double> vec = imap->doubleObjectiveVec();
        double tempmaxvalue= *std::max_element(vec.begin(), vec.end());
        if (tempmaxvalue>maxvalue){
            maxvalue = tempmaxvalue;
        }

    }

    int nobj = static_cast<int>(this->objectiveVecSize());
    TVector<double> refPoint(nobj, maxvalue*2);

    // Calculate Hypervolume
    double hvOrg = hypervolume(worstRankedSet, refPoint);
    TVector<double> wrankVector(worstRankedSet->size());

    //Remove a solution from the set and calculate Hypervolume to find out
    //which contributes the least
    for (int i=0; i<worstRankedSet->size(); i++) {
        ISet newSet(worstRankedSet->all());
        newSet.remove(i);
        double r = hypervolume(&newSet, refPoint);
        wrankVector[i] = hvOrg - r;
    }
    
    //Find out which individual contributes the least
    double minvalue = *std::min_element(wrankVector.begin(),
                                        wrankVector.end());
    IMappingSPtr cmap;
    for (int i=0; i<wrankVector.size(); i++){
        if (areDoublesEqual(wrankVector[i], minvalue)){
            cmap = worstRankedSet->at(i);
            break;
        }
    }

    // Remove that individual
    for (size_t i=0;i<solutionVector.size();i++){
        if (solutionVector[i] != cmap){
            oSet->append(solutionVector[i]);
        }
    }
    delete worstRankedSet;
}


TString SMSEMOAReduce::name()
{
    return TString("SMS-EMOA Reduce algoritm");
}
TString SMSEMOAReduce::description()
{
    return TString("Get the nondominated sets and based on Hypervolume"
                   "remove the least parent from the worst-ranked front.");
}

} // namespace Operators
} // namespace Tigon
