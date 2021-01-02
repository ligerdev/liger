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
#include <tigon/Operators/Filtrations/RandFiltrationForPerturbation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Operators {

RandFiltrationForPerturbation::RandFiltrationForPerturbation()
{
    addProperty("RatioOfNewSolutions"
                ,TString("Number of new solution created by the operator "
                         "divided by the number of solutions in the input set. "
                         "Default is 1")
                , getTType(double));
    m_name = TString("Random filtration for Perturbation");
    m_description = TString("Creates a set of solutions selected randomly "
                            "from the set tagged \"for modification\".\n"
                            "The set is tagged \"For Perturbation\".\n");

    addOutputTag(Tigon::TForPerturbation);

    addInputTag(Tigon::TForFiltration);

    TP_defineRatioOfNewSolutions(1);
}

RandFiltrationForPerturbation::RandFiltrationForPerturbation(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    addProperty("RatioOfNewSolutions"
                , TString("Number of new solution created by the operator "
                         "divided by the number of solutions in the input set. "
                         "Default is 1")
                , getTType(double));
    m_name = TString("Random filtration for Perturbation");
    m_description = TString("Creates a set of solutions selected randomly from the set tagged \"for modification\".\n");
    m_description += TString("The set is tagged \"For Perturbation\".\n");

    addOutputTag(Tigon::TForPerturbation);

    addInputTag(Tigon::TForFiltration);

    TP_defineRatioOfNewSolutions(1);
}

RandFiltrationForPerturbation::~RandFiltrationForPerturbation()
{

}

void RandFiltrationForPerturbation::evaluateNode()
{
    // Init
    clearOutputSets();
    ISet* iSet = inputSet(0);

    int sSize = iSet->size();
    // TODO: Thing of a clever way to maintain the population size
    //       when several modification operators are used.
    //       Currently, all operators use ceil, and the merge operator
    //       doesn't use the extra solutions (produced by the last operator
    //       in its list)
    int  nNew = std::ceil(sSize * m_ratioOfSolutions);

    appendOutputSet();
    for(int j=0; j<nNew; j++) {
        int idx = TRAND.randInt(sSize);
        cloneMapping(iSet->at(idx));
    }
}

void RandFiltrationForPerturbation::TP_defineRatioOfNewSolutions(double r)
{
    if(r > 0.0) {
        m_ratioOfSolutions = r;
    }
}

double RandFiltrationForPerturbation::TP_ratioOfNewSolutions() const
{
    return m_ratioOfSolutions;
}

TString RandFiltrationForPerturbation::name()
{
    return m_name;
}

TString RandFiltrationForPerturbation::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
