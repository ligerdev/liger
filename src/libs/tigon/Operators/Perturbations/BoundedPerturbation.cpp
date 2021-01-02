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
#include <tigon/Operators/Perturbations/BoundedPerturbation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/BoxConstraintViolationCorrections.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

BoundedPerturbation::BoundedPerturbation()
{
    BoundedPerturbation::initialise();
}

BoundedPerturbation::BoundedPerturbation(Tigon::Representation::IPSet* ipset) : IPerturbation(ipset)
{
    BoundedPerturbation::initialise();
}

void BoundedPerturbation::initialise()
{
    addProperty("PerturbationRadius"
                , TString("Maximum distance between the original "
                          "and the perturbed solution in normalised "
                          "decision space.\nTo account for "
                          "different scales, the distance is multiplied "
                          "by sqrt(d), where d is the number of decision "
                          "variables.\nThe default is 0.05")
                ,getTType(double));

    TP_definePerturbationRadius(0.05);

    TStringList types;
    types << Tigon::ETReal << Tigon::ETInteger
          << Tigon::ETOrdinal << Tigon::ETNominal;
    defineSupportedElementTypes(types);
}


BoundedPerturbation::~BoundedPerturbation()
{

}

void BoundedPerturbation::evaluateNode()
{
    BoxConstraintsDataSPtr box = boxConstraints();
    TVector<ElementType>   types = decisionVecTypes();
    TVector<int> indices;
    for(int i=0; i<decisionVecSize(); i++) {
        indices.push_back(i);
    }

    double maxDist = m_perturbationRadius * std::sqrt(decisionVecSize());
    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        for(int i=0; i<oSet->size(); i++) {
            // probability of mutating a solution
            if( doSolutionMutation() ) {
                double maxPert = maxDist;

                // Normalise the decision variables
                IMappingSPtr imap = oSet->at(i);

                // Shuffle the indices to avoid bias
                TVector<int> shuffledInd = Tigon::shuffle(indices);
                for(int k=0; k<decisionVecSize(); k++) {
                    int j = shuffledInd[k];

                    if( isSupportedElementType(types[j]) ) {
                        IElementSPtr var = imap->decisionVar(j);

                        // Uniform sample in the range [-maxPert, maxPert]
                        double u = TRAND.randUni(2*maxPert, -maxPert);

                        switch(types[j]) {
                        case RealType:
                        {
                            // use the value in order to ignore the distribution
                            IElement normVar(types[j], var->value());
                            normaliseToUnitInterval(normVar,
                                                    box->lowerBound(j),
                                                    box->upperBound(j));
                            normVar = normVar + u;
                            scaleBackFromUnitInterval(normVar,
                                                      box->lowerBound(j),
                                                      box->upperBound(j));
                            imap->defineDecisionVar(j, normVar);
                            break;
                        }
                        case IntegerType:
                        case OrdinalType:
                        {
                            int nIntervals =  box->upperBound(j).value<int>() -
                                    box->lowerBound(j).value<int>();
                            int idxDiff = std::floor(std::abs(u) * nIntervals);
                            if(u < 0) {
                                idxDiff = -idxDiff;
                            }
                            imap->defineDecisionVar(j, *var + idxDiff);
                            u = (double)idxDiff / (double)nIntervals;
                            break;
                        }
                        case NominalType:
                        default:
                        {
                            int nIntervals =  box->upperBound(j).value<int>() -
                                    box->lowerBound(j).value<int>();
                            if(std::abs(u) > 1.0/nIntervals) {
                                int newVal = TRAND.randInt(nIntervals + 1,
                                                           box->lowerBound(j).value<int>());
                                imap->defineDecisionVar(j, newVal);
                            } else {
                                u = 0.0;
                            }
                            break;
                        }
                        }

                        // Update the remaining maximum perturbation radius
                        maxPert = std::sqrt(maxPert*maxPert - u*u);
                    }
                }

                // reflect the solution variables back to feasible decision space if necessary
                reflectIntoFeasibleDomain(imap, box);
            }
        }
    }
}

// Perturbation Radius
double BoundedPerturbation::TP_perturbationRadius() const
{
    return m_perturbationRadius;
}

void BoundedPerturbation::TP_definePerturbationRadius(double r)
{
    if((r>0) && (r<1)) {
        m_perturbationRadius = r;
    }
}

TString BoundedPerturbation::name()
{
    return TString("Bounded Perturbation");
}

TString BoundedPerturbation::description()
{
    return TString("Perturbs a decision vector within a hyper-sphere with\n"
                   "a predefined radius.\n"
                   "The distance is defined in a normalised decision space, "
                   "according to the box constraints of each variable.");
}

} // namespace Operators
} // namespace Tigon
