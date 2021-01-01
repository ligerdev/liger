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
#include <tigon/Operators/Filtrations/FitnessEliteSelection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Operators/Filtrations/PSAClustering.h>
#include <tigon/Utils/TigonUtils.h>



namespace Tigon {
namespace Operators {

FitnessEliteSelection::FitnessEliteSelection()
{
    addProperty("EliteRatio"
                , TString("Proportion of the Elite solutions of the "
                          "total population. Default is half.")
                , getTType(double));

    m_name = TString("Fitness-Based Elite Selection");
    m_description = TString("Creates a set with the best solutions selected "
                            "from the input set.\n");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineEliteRatio(0.5);
    addOutputTag(Tigon::TForNextIteration);
}

FitnessEliteSelection::FitnessEliteSelection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    addProperty("EliteRatio",
                TString("Proportion of the Elite solutions of the total population. Default is half.")
                , getTType(double));

    m_name = TString("Fitness-Based Elite Selection");
    m_description = TString("Creates a set with the best solutions selected "
                            "from the input set.\n");

    addInputTag(Tigon::TForSelection);
    addInputTag(Tigon::TFitness);

    TP_defineEliteRatio(0.5);
    addOutputTag(Tigon::TForNextIteration);
}

FitnessEliteSelection::~FitnessEliteSelection()
{

}

void FitnessEliteSelection::evaluateNode()
{
    // Init
    clearOutputSets();  //overrides the data from previous iteration
    ISet* oSet = appendOutputSet();
    ISet* iSet = setsUnion(inputSets());

    // Define the elite size
    int popSize = iSet->size();
    int eliteSize = std::ceil(popSize*m_eliteRatio);

    // Sort inputs by fitness
    TVector<double> costVec(popSize);
    for(int i=0; i<popSize; i++) {
        costVec[i] = iSet->at(i)->doubleCost();
    }
    TVector<int> idx = indSort(costVec);

    // Select the IMappings with smaller cost
    for(int i=0; i<eliteSize; i++) {
        oSet->append(iSet->at(idx[i]));
    }
    delete iSet;
}

double FitnessEliteSelection::TP_eliteRatio() const
{
    return m_eliteRatio;
}


void FitnessEliteSelection::TP_defineEliteRatio(double r)
{
    if(isInInterval(r, 0.0, 1.0)) {
        m_eliteRatio = r;
    }
}
TString FitnessEliteSelection::name()
{
    return m_name;
}

TString FitnessEliteSelection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
