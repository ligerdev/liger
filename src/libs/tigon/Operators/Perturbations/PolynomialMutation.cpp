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
#include <tigon/Operators/Perturbations/PolynomialMutation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>

namespace Tigon {
namespace Operators {

PolynomialMutation::PolynomialMutation()
{
    initialise();
}

PolynomialMutation::PolynomialMutation(Tigon::Representation::IPSet* ipset)
    : IPerturbation(ipset)
{
    initialise();
}

void PolynomialMutation::initialise()
{
    addProperty("MutationDistributionIndex",
                TString("Controls the remoteness of the children from "
                        "the parent (see reference). "
                        "Default is 20.0"),
                typeid(double).hash_code());

    TP_defineMutationDistributionIndex(Tigon::DefaultMutationDistributionIndex);

    TStringList types;
    types << Tigon::ETReal << Tigon::ETInteger;
    defineSupportedElementTypes(types);
}


PolynomialMutation::~PolynomialMutation()
{

}

void PolynomialMutation::evaluateNode()
{
    TVector<IElement> lb = boxConstraints()->lowerBounds();
    TVector<IElement> ub = boxConstraints()->upperBounds();

    int dVecSize = decisionVecSize();

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        for(IMappingSPtr imap : oSet->all()) {
            //IMappingSPtr imap = oSet->at(i);

            // probability of mutating a solution
            if( doSolutionMutation() ) {

                for(int j=0; j<dVecSize; j++) {

                    // probability of mutating a variable
                    if( doVariableMutation() ) {

                        if( isSupportedElementType(decisionVecTypes().at(j)) ) {
                            double var = imap->decisionVar(j)->value<double>();
                            double mutatedVar;

                            double u = TRAND.randUni();
                            if(u <= 0.5) {
                                double delta = std::pow( 2.0*u, 1.0/(m_eta+1.0) ) - 1.0;
                                mutatedVar = var + delta * (var - lb[j].value());
                            } else {
                                double delta = 1.0 - std::pow( ( 2.0*(1.0-u) ), 1.0/(m_eta+1.0) );
                                mutatedVar = var + delta * (ub[j].value() - var);
                            }
                            imap->defineDecisionVar(j, mutatedVar);
                        }
                    }
                }
                // reflect the solution variables back to feasible decision space if necessary
                reflectIntoFeasibleDomain(imap, boxConstraints());
            }

        }
    }
}

// Distribution index
void PolynomialMutation::TP_defineMutationDistributionIndex(double eta)
{
    if(eta >= 0) {
        m_eta = eta;
    }
}

double PolynomialMutation::TP_mutationDistributionIndex() const
{
    return m_eta;
}


TString PolynomialMutation::name()
{   
    return TString("Polynomial Mutation");
}

TString PolynomialMutation::description()
{
    return TString("Based on Deb, K. and Deb, D. \"Analysing mutation "
                   "schemes for real-parameter genetic algorithms.\" "
                   "International Journal of Artificial Intelligence "
                   "and Soft Computing 4.1 (2014): 1-28.");
}

} // namespace Operators
} // namespace Tigon
