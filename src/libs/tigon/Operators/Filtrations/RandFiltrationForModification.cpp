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
#include <tigon/Operators/Filtrations/RandFiltrationForModification.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Random/RandomGenerator.h>
#include <cstdlib>
#include <ctime>


namespace Tigon {
namespace Operators {

RandFiltrationForModification::RandFiltrationForModification()
{

}

RandFiltrationForModification::RandFiltrationForModification(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

RandFiltrationForModification::~RandFiltrationForModification()
{

}

void RandFiltrationForModification::evaluateNode()
{
    // Init
    clearOutputSets();  //overrides the data from previous iteration
    ISet* iSet  = inputSet(0);
    appendOutputSet();

    int sSize = iSet->size();

    for(int i=0; i<m_poolSize; i++) {
        int idx = TRAND.randInt(sSize);
        cloneMapping(iSet->at(idx));
    }
}

void RandFiltrationForModification::TP_definePoolSize(int n) {
    m_poolSize = n;
}

int RandFiltrationForModification::TP_poolSize() const {
    return m_poolSize;
}

void RandFiltrationForModification::initialise()
{
    addProperty("PoolSize", "Number of solutions to select. Default is 1"
                , getTType(int));

    m_name = TString("Random selection for modification");
    m_description = TString("Creates a set of solutions selected randomly from a set tagged \"for selection\".\n");
    m_description += TString("The created set is tagged \"for modification\".\n");
    m_description += TString("A solution may be selected more than once.\n");
    m_description += TString("The IMappings in the set are clones of the selected IMappings.");

    addInputTag(Tigon::TForSelection);

    TP_definePoolSize(1);
    addOutputTag(Tigon::TForModification);
}

TString RandFiltrationForModification::name()
{
    return m_name;
}

TString RandFiltrationForModification::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
