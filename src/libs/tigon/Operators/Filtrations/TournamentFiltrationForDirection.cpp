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
#include <tigon/Operators/Filtrations/TournamentFiltrationForDirection.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

TournamentFiltrationForDirection::TournamentFiltrationForDirection()
{
    initialise();
}

TournamentFiltrationForDirection::TournamentFiltrationForDirection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

TournamentFiltrationForDirection::~TournamentFiltrationForDirection()
{

}

void TournamentFiltrationForDirection::initialise()
{
    m_name = TString("Tournament filtration for direction");
    m_description = TString("Creates a set of solutions from a set tagged \"for selection\".\n");
    m_description += TString("The created set is tagged \"for direction\".\n");
    m_description += TString("Every set has the same number of solutions. ");
    m_description += TString("This should match the target direction operator.\n");
    m_description += TString("Until the required number of sets is populated, two solutions from the input set are selected, ");
    m_description += TString("and the solution with the lower cost function is appended to the set");
    m_description += TString("A solution may be selected more than once.\n");
    m_description += TString("The IMappings in the output set are clones of the selected IMappings.");


    addProperty("MappingsPerSet", TString("This is the number of mappings of each Set that "
                                          "is to be sent \"for direction\".\n"
                                          "The default is 2 mappings per set")
                , getTType(int));

    addProperty("NumberOfMappings", TString("This is the total number of mappings that can be "
                                            "added to the output set"
                                            "The default is -1 which means that the entire "
                                            "input set is selected into the output set")
                , getTType(int));

//    addProperty("SelectionMethod", TString("This property allows the user to select the method, "
//                                           " utilised by this operator, for picking the "
//                                           " solutions from the input set for tournament.\n"
//                                           "Note that only two solutions are chosen at the time "
//                                           "from the input set for tournament.\n"
//                                           "The available methods are:\n"
//                                           "1. RandomSelection: the solutions are selected randomly "
//                                           "from the input set until the output set size has reached "
//                                           "the desirable size.\n"
//                                           "2. ShuffledSelection: the input set is shuffled and two "
//                                           "consecutive solutions are selected until the end of the "
//                                           "input set is reached. If the end of the input set is "
//                                           "reached and the number of solutions in the output set is "
//                                           "not equal to the desirable size, then the input set is "
//                                           "shuffled again and two consecutive solution are again "
//                                           "selected from beginning of the input set.\n"
//                                           "3. OrderedSelection: two consecutive solutions are selected "
//                                           "from the input set, until the end of the set is reached. "
//                                           "If the end of the set is reached and the desirable number "
//                                           "of solutions (mappings) in the output set has not been "
//                                           "achieved yet, then start again from the beginning of the "
//                                           "input set.")
//                , typeid(Tigon::SelectionType).hash_code());

    TP_defineMappingsPerSet(2);    // mappings per set
    TP_defineNumberOfMappings(-1); // number of mappings
    TP_defineSelectionMethod(Tigon::OrderedSelection);

    addInputTag(Tigon::TForSelection);
    addOutputTag(Tigon::TForDirection);
}

void TournamentFiltrationForDirection::evaluateNode()
{
    // Init
    int p,i;

    clearOutputSets();  // overrides the data from previous iteration

    ISet* iSet = setsUnion(inputSets());
    int popSize = iSet->size();

    int NMappings = (TP_numberOfMappings()<0) ? popSize : TP_numberOfMappings();

    p=0; i=0;
    switch(TP_selectionMethod()) {
    case Tigon::RandomSelection:
    {
        while(p<NMappings) {
            appendOutputSet();
            for(int j=0; j<TP_mappingsPerSet(); j++) {

                int idxA = TRAND.randInt(popSize);
                int idxB = TRAND.randInt(popSize);
                double cost1 = iSet->at(idxA)->doubleCost();
                double cost2 = iSet->at(idxB)->doubleCost();
                int idxT = (cost1 < cost2) ? idxA : idxB;

                cloneMapping(iSet->at(idxT));
                p++;
                if(p >= NMappings) break;
            }
        }
        break;
    }
    case Tigon::ShuffledSelection:
    {
        TVector<int> permIdx = randomPermutationSampling(popSize);

        while(p<NMappings) {
            appendOutputSet();
            for(int j=0; j<TP_mappingsPerSet(); j++) {

                int idxA = permIdx[i];
                int idxB = (i==(popSize-1)) ? idxA : permIdx[i+1];

                double cost1 = iSet->at(idxA)->doubleCost();
                double cost2 = iSet->at(idxB)->doubleCost();
                int idxT = (cost1 < cost2) ? idxA : idxB;

                cloneMapping(iSet->at(idxT));

                i+=2;
                p++;

                if(p >= NMappings) break;
                if(i >= popSize) {
                    permIdx = randomPermutationSampling(popSize); // permutation again
                    i=0;
                }
            }
        }

        break;
    }
    case Tigon::OrderedSelection:
    default:
    {

        while(p<NMappings) {
            appendOutputSet();
            for(int j=0; j<TP_mappingsPerSet(); j++) {
                int idxA = i;
                int idxB = (i==(popSize-1)) ? idxA : i+1;
                double cost1 = iSet->at(idxA)->doubleCost();
                double cost2 = iSet->at(idxB)->doubleCost();
                int idxT = (cost1 < cost2) ? idxA : idxB;

                cloneMapping(iSet->at(idxT));

                i=i+2;
                p=p+1;

                if(p >= NMappings) break;
                if(i >= NMappings) {
                    i=0;
                }
            }
        }

        break;
    }
    }


    delete iSet;
}


void TournamentFiltrationForDirection::TP_defineMappingsPerSet(int n)
{
    if(n>0) {
        m_mappingsPerSet = n;
    }
}

int  TournamentFiltrationForDirection::TP_mappingsPerSet() const
{
    return m_mappingsPerSet;
}

void TournamentFiltrationForDirection::TP_defineNumberOfMappings(int n)
{
    m_numberOfMappings = n;
}

int TournamentFiltrationForDirection::TP_numberOfMappings() const
{
    return m_numberOfMappings;
}

void TournamentFiltrationForDirection::TP_defineSelectionMethod(SelectionType q)
{
    m_selectionMethod = q;
}

SelectionType TournamentFiltrationForDirection::TP_selectionMethod() const
{
    return m_selectionMethod;
}



TString TournamentFiltrationForDirection::name()
{
    return m_name;
}

TString TournamentFiltrationForDirection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
