/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Fitness/PSACrowding.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Operators/Filtrations/PSAClustering.h>



namespace Tigon {
namespace Operators {

PSACrowding::PSACrowding()
{
    initialise();
}

PSACrowding::PSACrowding(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    initialise();
}

PSACrowding::~PSACrowding()
{
    delete m_cluster;
}

void PSACrowding::initialise()
{
    m_cluster = new PSAClustering(this);
    m_cluster->defineInputTags(TStringList({Tigon::TTempForClustering}));

    clearOutputTags();
    clearAdditionalOutputTags();
    addOutputTag(Tigon::TFitness);
}

void PSACrowding::evaluateNode()
{
    while(hasNextOutputSet()) {
        ISet* rank = nextOutputSet();
        int nClusters = std::ceil(rank->size()/2.0);

        tagSet(rank, Tigon::TTempForClustering);
        m_cluster->TP_defineNumberOfClusters(nClusters);
        m_cluster->evaluateOnlyThisNode();
        removeTagFromSet(rank, Tigon::TTempForClustering);

        //increment at the end of the loop, to avoid processing the representative set
        ISet* niche = m_cluster->outputSet(0);
        while(m_cluster->hasNextOutputSet())
        {
            int nicheSize = niche->size();
            //crowding panelty is within the interval [0 1)
            double nichePenalty = 1 - std::pow(2,1-nicheSize);
            for(int i=0; i < nicheSize; i++)
            {
                double newCost = niche->at(i)->doubleCost() + nichePenalty;
                niche->at(i)->defineCost(newCost);
            }
            niche = m_cluster->nextOutputSet();
        }
    }

    m_cluster->clearOutputSets();
}

TString PSACrowding::name()
{
    return TString("PSA Crowding");
}

TString PSACrowding::description()
{
    TString desc("Assign crowding measure according to NSGAII-PSA.\n");
        desc += TString("Solutions in each rank are partitioned to n/2 subsets "
                        "using 'Part and Select Algorithm' (PSA), where n is "
                        "the number of solutions in the rank.\n"
                        "The cost of each solution is incremented in a number "
                        "proportional to the size of its subset.");
        return desc;
}

} // namespace Operators
} // namespace Tigon
