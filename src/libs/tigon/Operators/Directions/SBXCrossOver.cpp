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
#include <tigon/Operators/Directions/SBXCrossOver.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>

namespace Tigon {
namespace Operators {

SBXCrossOver::SBXCrossOver()
{
    initialise();
}

SBXCrossOver::SBXCrossOver(Tigon::Representation::IPSet* ipset)
    : IDirection(ipset)
{
    initialise();
}

void SBXCrossOver::initialise()
{
    addProperty("CrossoverDistributionIndex"
                , TString("Controls the remoteness of the children "
                          "from the parents (see reference). "
                          "Default is 15.0")
                , getTType(double));

    TP_defineCrossoverDistributionIndex(Tigon::DefaultCrossoverDistributionIndex);

    TStringList types({ Tigon::ETReal, Tigon::ETInteger});
    defineSupportedElementTypes(types);
}

SBXCrossOver::~SBXCrossOver()
{

}

void SBXCrossOver::evaluateNode()
{
    int dvecSize = decisionVecSize();
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

            TVector<ElementType> dVecTypes = decisionVecTypes();
            for(int var=0; var<dvecSize; var++) {

                if( isSupportedElementType(dVecTypes.at(var)) ) {
                    // probability of crossover two variables
                    if( doVariableCrossover() ) {

                        double u = TRAND.randUni();
                        if(u <= 0.5) {
                            beta = std::pow( 2.0*u, 1.0/(m_eta+1.0) );
                        } else {
                            beta = std::pow( 1.0/( 2.0*(1.0-(u-Tigon::Epsilon)) ), 1.0/(m_eta+1.0) );
                        }
                        double aVar = imapA->decisionVar(var)->value();
                        double bVar = imapB->decisionVar(var)->value();
                        double xVar = 0.5*( (1.0+beta)*aVar + (1.0-beta)*bVar );
                        double yVar = 0.5*( (1.0-beta)*aVar + (1.0+beta)*bVar );

                        // probability of swapping two variables
                        if ( doVariableSwap() ) {
                            imapA->defineDecisionVar(var, xVar);
                            imapB->defineDecisionVar(var, yVar);
                        } else {
                            imapA->defineDecisionVar(var, yVar);
                            imapB->defineDecisionVar(var, xVar);
                        }
                    }
                }
            }
            // reflect the solution variables back to feasible decision space if necessary
            for(int i=0; i<oSet->size(); i++) {
                reflectIntoFeasibleDomain(oSet->at(i), boxConstraints());
            }
        }
    }
}

// Distribution index
void SBXCrossOver::TP_defineCrossoverDistributionIndex(double eta)
{
    if(eta >= 0) {
        m_eta = eta;
    }
}

double SBXCrossOver::TP_crossoverDistributionIndex() const
{
    return m_eta;
}

TString SBXCrossOver::name()
{
    return TString("Simulated Binary Cross-Over (SBX)");
}

TString SBXCrossOver::description()
{
    return TString("Based on \"Deb, K. and Agrawal, R.B. (1995). "
                   "Simulated binary crossover for continuous search space. "
                   "Complex Systems 9(2), 115-148.\"");
}

} // namespace Operators
} // namespace Tigon
