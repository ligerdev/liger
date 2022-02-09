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
#include <tigon/Operators/Directions/SinglePointCrossOver.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Random/RandomGenerator.h>



namespace Tigon {
namespace Operators {

SinglePointCrossOver::SinglePointCrossOver()
{
    initialise();
}

SinglePointCrossOver::SinglePointCrossOver(Tigon::Representation::IPSet* ipset)
    : IDirection(ipset)
{
    initialise();
}

SinglePointCrossOver::~SinglePointCrossOver()
{

}

void SinglePointCrossOver::initialise()
{
    TStringList types;
    types << Tigon::ETReal << Tigon::ETInteger;
    defineSupportedElementTypes(types);
}

void SinglePointCrossOver::evaluateNode()
{
    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        if(oSet->size() < 2) {
            return;
        }
        // probability of crossover two solutions
        if( doSolutionCrossover() ) {

            IMappingSPtr imapA = oSet->at(0);
            IMappingSPtr imapB = oSet->at(1);
            double beta;

            for(int var=0; var<decisionVecSize(); var++) {

                if( isSupportedElementType(decisionVecTypes().at(var)) ) {
                    IElement aVar = *(imapA->decisionVar(var));
                    IElement bVar = *(imapB->decisionVar(var));

                    // probability of crossover two variables
                    if( (aVar != bVar) && doVariableCrossover() ) {
                        beta = TRAND.randUni();
                        IElement xVar = beta     * aVar + (1-beta) * bVar;
                        IElement yVar = (1-beta) * aVar + beta     * bVar;

                        imapA->defineDecisionVar(var, xVar);
                        imapB->defineDecisionVar(var, yVar);
                    }
                }
            }
        }
    }
}

TString SinglePointCrossOver::name()
{
    return TString("Single Point Cross-Over");
}

TString SinglePointCrossOver::description()
{
    return TString("Two offspring are generated from every two parents. "
                   "a random number beta ~ U(0, 1) is used for each variable. "
                   "For every pair of parent values a, b "
                   "the offspring values are beta*a+(1-beta)*b and (1-beta)*a+beta*a");
}

} // namespace Operators
} // namespace Tigon
