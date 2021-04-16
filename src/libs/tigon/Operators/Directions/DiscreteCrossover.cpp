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
#include <tigon/Operators/Directions/DiscreteCrossover.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>

namespace Tigon {
namespace Operators {

DiscreteCrossover::DiscreteCrossover()
{
    initialise();
}

DiscreteCrossover::DiscreteCrossover(Tigon::Representation::IPSet* ipset)
    : IDirection(ipset)
{
    initialise();
}

DiscreteCrossover::~DiscreteCrossover()
{

}

void DiscreteCrossover::initialise()
{
    TStringList types;
    types << Tigon::ETOrdinal << Tigon::ETNominal;
    defineSupportedElementTypes(types);
}

void DiscreteCrossover::evaluateNode()
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

            TVector<double> p1 = imapA->doubleDecisionVec();
            TVector<double> p2 = imapB->doubleDecisionVec();

            for(int i=0; i<decisionVecSize(); i++) {

                if( isSupportedElementType(decisionVecTypes().at(i)) ) {

                    // probability of crossover two variables
                    if( doVariableCrossover() ) {

                        double u1 = TRAND.randUni();
                        if(u1 <= 0.5) {
                          imapA->defineDecisionVar(i, p2[i]);
                        } else {
                          imapA->defineDecisionVar(i, p1[i]);
                        }

                        double u2 = TRAND.randUni();
                        if(u2 <= 0.5) {
                          imapB->defineDecisionVar(i, p1[i]);
                        } else {
                          imapB->defineDecisionVar(i, p2[i]);
                        }
                    }
                }
            }
            // reflect the solution variables back to feasible decision space,
            // if necessary
            for(int i=0; i<oSet->size(); i++) {
                reflectIntoFeasibleDomain(oSet->at(i), boxConstraints());
            }
        }
    }
}

TString DiscreteCrossover::name()
{
    return TString("Discrete Crossover (DC)");
}

TString DiscreteCrossover::description()
{
    return TString("Based on Voigt H. M., Muhlenbein H., (1995), Cvetkovic D., "
                   "Fuzzy recombination for the Breeder Genetic Algorithm, in "
                   "Proceedings of the Sixth International Conference on Genetic "
                   "Algorithms, Morgan Kaufman, pp. 104-111");
}

} // namespace Operators
} // namespace Tigon
