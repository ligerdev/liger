/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Filtrations/FreezeOperatorOutput.h>
#include <tigon/Representation/Sets/ISet.h>


namespace Tigon {
namespace Operators {

FreezeOperatorOutput::FreezeOperatorOutput()
{

}

FreezeOperatorOutput::FreezeOperatorOutput(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{

}

FreezeOperatorOutput::~FreezeOperatorOutput()
{

}

void FreezeOperatorOutput::evaluateNode()
{
    // Init
    clearOutputSets();  //overrides the data from previous iteration

    while(hasNextInputSet()) {
        ISet* iSet  = nextInputSet();
        appendOutputSet();
        for(int i=0; i<iSet->size(); i++) {
            cloneMapping(iSet->at(i));
        }
    }
}

TString FreezeOperatorOutput::name()
{
    return TString("Freeze Operator Output");
}

TString FreezeOperatorOutput::description()
{
    return TString("Duplicates the output sets of an operator.\n"
                   "Useful when several operators share the same output "
                   "sets, and the state of the IMappings along the "
                   "process is required.");
}

} // namespace Operators
} // namespace Tigon
