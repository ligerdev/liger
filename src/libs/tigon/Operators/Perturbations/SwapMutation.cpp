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
#include <tigon/Operators/Perturbations/SwapMutation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>

namespace Tigon {
namespace Operators {

SwapMutation::SwapMutation()
{
    initialise();
}

SwapMutation::SwapMutation(Tigon::Representation::IPSet* ipset)
    : IPerturbation(ipset)
{
    initialise();
}

void SwapMutation::initialise()
{
    TStringList types;
    types << Tigon::ETNominal << Tigon::ETOrdinal << Tigon::ETReal << Tigon::ETInteger;
    defineSupportedElementTypes(types);
}

SwapMutation::~SwapMutation()
{

}

void SwapMutation::evaluateNode()
{
    int n = decisionVecSize();

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        for(int i=0; i<oSet->size(); i++) {
            IMappingSPtr imap = oSet->at(i);

            // probability of mutating a solution
            if( doSolutionMutation() ) {
                for(int j=0; j<n; j++) {
                    if( isSupportedElementType(decisionVecTypes().at(j)) ) {

                        // probability of mutating a variable
                        if( doVariableMutation() ) {

                            int swap = TRAND.randInt(n-1,0);

                            IElement var1 = *(imap->decisionVar(j));
                            IElement var2 = *(imap->decisionVar(swap));

                            imap->defineDecisionVar(j,var2);
                            imap->defineDecisionVar(swap,var1);
                        }
                    }
                }
            }
        }
    }
}

TString SwapMutation::name()
{
    return TString("Swap Mutation");
}

TString SwapMutation::description()
{
    return TString("Swaps the values of two decision variables. For each "
                   "decision variable to be swapped, another decision variable "
                   "is chosen randomly.");
}


} // namespace Operators
} // namespace Tigon
