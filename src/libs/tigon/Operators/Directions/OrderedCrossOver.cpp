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
#include <tigon/Operators/Directions/OrderedCrossOver.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <numeric>

namespace Tigon {
namespace Operators {

OrderedCrossOver::OrderedCrossOver()
{
    initialise();
}

OrderedCrossOver::OrderedCrossOver(Tigon::Representation::IPSet* ipset)
    : IDirection(ipset)
{
    initialise();
}

void OrderedCrossOver::initialise()
{
    TStringList types;
    types << Tigon::ETNominal << Tigon::ETOrdinal;
    defineSupportedElementTypes(types);
}

OrderedCrossOver::~OrderedCrossOver()
{

}

void OrderedCrossOver::evaluateNode()
{
    int n = decisionVecSize();

    while(hasNextOutputSet()) {

        ISet* oSet = nextOutputSet();
        if(oSet->size() < 2) {
            return;
        }

        // probability of crossover for two solutions
        if( doSolutionCrossover() ) {
            IMappingSPtr imapA = oSet->at(0);
            IMappingSPtr imapB = oSet->at(1);

            TVector<IElement> child1(n);
            TVector<IElement> child2(n);

            TVector<int> indices1(n);
            std::iota(std::begin(indices1),std::end(indices1),0);
            TVector<int> indices2(n);
            std::iota(std::begin(indices2),std::end(indices2),0);

            int number1 = TRAND.randInt(n-1,0);
            int number2 = number1;
            while(number1==number2) {
                number2 = TRAND.randInt(n-1,0);
            }

            int start = std::min(number1, number2);
            int end   = std::max(number1, number2);

            // child1: copy segment from aVar
            // child2: copy segment from bVar
            for(int i=start; i<end; i++) {
                IElement aVar = *(imapA->decisionVar(i));
                IElement bVar = *(imapB->decisionVar(i));
                child1[i] = aVar;
                child2[i] = bVar;

                indices1.erase(indices1.begin()+start);
                indices2.erase(indices2.begin()+start);
            }

            for(int i=0; i<n; i++) {
                IElement aVar = *(imapA->decisionVar(i));
                IElement bVar = *(imapB->decisionVar(i));

                int aValue = aVar.value<int>();
                int bValue = bVar.value<int>();

                // child1: use bValue
                bool exists = false;
                for(int j=0; j<n; j++) {
                    int child = child1[j].value<int>();
                    if(bValue==child) {
                        exists = true;
                        break;
                    }
                }
                if(!exists) {
                    child1[indices1[0]] = bVar;
                    indices1.erase(indices1.begin());
                }

                // child2: use aValue
                exists = false;
                for(int j=0; j<n; j++) {
                    int child = child2[j].value<int>();
                    if(aValue==child) {
                        exists = true;
                        break;
                    }
                }
                if(!exists) {
                    child2[indices2[0]] = aVar;
                    indices2.erase(indices2.begin());
                }
            }

            for(int i=0; i<n; i++) {
                imapA->defineDecisionVar(i, child1[i]);
                imapB->defineDecisionVar(i, child2[i]);
            }
        }

    }
}

TString OrderedCrossOver::name()
{
    return TString("Ordered Cross-Over");
}

TString OrderedCrossOver::description()
{
    return TString("Ordered Cross-Over. This crossover builds offspring by choosing a subsequence "
                   "of a tour from one parent and preserving the relative order of cities from "
                   "the other parent.");
}

} // namespace Operators
} // namespace Tigon
