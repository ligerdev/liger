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
#include <tigon/Operators/AlgorithmSpecific/ACROMUSE/AcromuseEliteFiltration.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

AcromuseEliteFiltration::AcromuseEliteFiltration()
{
    initialise();
}

AcromuseEliteFiltration::
AcromuseEliteFiltration(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

AcromuseEliteFiltration::~AcromuseEliteFiltration()
{

}

void AcromuseEliteFiltration::evaluateNode()
{
    // Init
    clearOutputSets();

    ISet* iSet = setsUnion(inputSets());

    TVector<double> cost;
    cost.reserve(iSet->size());
    for(auto sol : iSet->all()) {
        cost.push_back(sol->doubleCost());
    }
    int bestIdx = std::min_element(cost.begin(), cost.end()) - cost.begin();

    ISet* oSet = appendOutputSet();
    oSet->append(iSet->at(bestIdx));

    delete iSet;
}

TString AcromuseEliteFiltration::name()
{
    return TString("ACROMUSE elite filtration");
}

TString AcromuseEliteFiltration::description()
{
    return TString("Chooses the solution with the best fitness in the input set.");
}

void AcromuseEliteFiltration::initialise()
{
    addInputTag(Tigon::TMainOptimizationSet);
    addOutputTag(Tigon::TForNextIteration);
}

} // namespace Operators
} // namespace Tigon
