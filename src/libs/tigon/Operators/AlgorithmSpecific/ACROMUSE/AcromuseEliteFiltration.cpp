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
    int sSize = iSet->size();
    int idx = -1;
    double best = Tigon::Highest;

    for(int i=0; i<sSize; i++) {
        double f = iSet->at(i)->doubleCost();
        if(f < best){
            best = f;
            idx = i;
        }
    }
    ISet* oSet = appendOutputSet();
    oSet->append(iSet->at(idx));
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
