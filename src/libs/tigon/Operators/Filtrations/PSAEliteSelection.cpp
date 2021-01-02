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
#include <tigon/Operators/Filtrations/PSAEliteSelection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Operators/Filtrations/PSAClustering.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Random/RandomGenerator.h>



namespace Tigon {
namespace Operators {

PSAEliteSelection::PSAEliteSelection()
{
    initialise();
}

PSAEliteSelection::PSAEliteSelection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

PSAEliteSelection::~PSAEliteSelection()
{
    delete m_cluster;
}

void PSAEliteSelection::initialise()
{
    addProperty("EliteRatio"
                , TString("Proportion of the Elite solutions of the total"
                          " population. Default is half.")
                , getTType(double));

    m_name = TString("PSA Elite Selection");
    m_description = TString("Creates a set with the best solutions selected from sets of non-dominace ranks.\n");
    m_description += TString("The first j ranks that are smaller then the elite size are selected.\n");
    m_description += TString("The reamining p solutions from the next rank are selected using PSA algorithm:\n");
    m_description += TString("The next rank is clustered to p sets, and the central solution is selected from each.\n");
    m_description += TString("The IMappings in the set are aliases of the selected IMappings.");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);
    addOutputTag(Tigon::TForNextIteration);

    TP_defineEliteRatio(0.5);

    m_cluster = new PSAClustering(this);
    m_cluster->defineInputTags(TStringList({Tigon::TTempForClustering}));
}

void PSAEliteSelection::evaluateNode()
{
    // Init
    clearOutputSets();  //overrides the data from previous iteration
    ISet* oSet = appendOutputSet();

    // Define the elite size
    TVector<ISet*> allRanks = inputSets();
    int popSize = 0;
    for(int r=0; r<allRanks.size(); r++) {
        popSize += allRanks.at(r)->size();
    }
    int eliteSize = std::ceil(popSize*m_eliteRatio);

    // Copy the first j ranks
    ISet* iSet;
    while(oSet->size() < eliteSize  && hasNextInputSet()) {
        if( peepNextInputSet()->size() <= eliteSize - oSet->size() ) {
            iSet = nextInputSet();
            ISet* uSet = uniqueSet(iSet); //calls new
            oSet->append(uSet);
            delete uSet;
        } else {
            break;
        }
    }

    // copy the remaining solutions from the next rank
    int missingSolutions = eliteSize - oSet->size();
    while(missingSolutions > 0 && hasNextInputSet()) {
        ISet* iSet = nextInputSet();

        tagSet(iSet, Tigon::TTempForClustering);
        m_cluster->TP_defineNumberOfClusters(missingSolutions);
        m_cluster->evaluateOnlyThisNode();
        removeTagFromSet(iSet, Tigon::TTempForClustering);

        ISet* lastSolutions = m_cluster->represenativeSet();
        oSet->append(lastSolutions);
        missingSolutions = eliteSize - oSet->size();
    }

    while(missingSolutions > 0) {
        int idx = TRAND.randInt(oSet->size());
        oSet->append(oSet->at(idx));
        missingSolutions--;
    }

    m_cluster->clearOutputSets();
}

double PSAEliteSelection::TP_eliteRatio() const
{
    return m_eliteRatio;
}

void PSAEliteSelection::TP_defineEliteRatio(double r)
{
    if(isInInterval(r,0,1)) {
        m_eliteRatio = r;
    }
}
TString PSAEliteSelection::name()
{
    return m_name;
}

TString PSAEliteSelection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
