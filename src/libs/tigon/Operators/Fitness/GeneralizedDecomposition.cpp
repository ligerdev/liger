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
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>

// STL includes
#include <stdlib.h>
#include <ctime>

// Boost includes
#include <boost/math/special_functions/factorials.hpp>

namespace Tigon {
namespace Operators {

GeneralizedDecomposition::GeneralizedDecomposition()
{
    initialise();
}

GeneralizedDecomposition::GeneralizedDecomposition(Tigon::Representation::IPSet* ipset)
    : Scalarization(ipset)
{
    initialise();
}

GeneralizedDecomposition::~GeneralizedDecomposition()
{

}

void GeneralizedDecomposition::initialise()
{
    m_name = TString("Generalized Decomposition");
    m_description = TString("Assigns every solution in the set a scalar value"
                            " using Generalised Decomposition.");
}

void GeneralizedDecomposition::evaluateNode()
{
    /// Currently, to determine the 'optimal' weight vector, the inverse of the
    /// direction of each component is used. However, this is only optimal for
    /// Weighted Sum and Weighted Chebyshev scalarising functions, and a more
    /// general method is required in case other scalarising functions are to
    /// be used in the future.

    //int M=objectiveVecSize();
    // define the optimal weighting vector
    TVector<double> weight;
    switch(weightScopeApproach()) {
    case LocalWeight:
        // do nothing
        break;
    case GlobalWeight: // construct global weight
    default:
        weight = dirVec();

        std::transform(weight.begin(), weight.end(), weight.begin(),
            [](double w) { return 1.0/(w+0.01); });
        toUnitVec(weight, 1.0);

//        for(int i=0; i<M; i++) {
//            weight[i] = 1.0 / dirVec()[i];
//        }
//        toUnitVec(weight, 1.0);
        //magnitudeAndDirectionP(weight, 1.0);
        break;
    }

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();

        // Normalise the objective vectors
        int setSize = oSet->size();
        TVector<TVector<double> > normalObjectives(setSize);
        TVector<double> ideal = IElementVecToRealVec(idealVec());
        TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());
        for (int i=0; i<setSize; i++) {
            if(!oSet->at(i)->isScalarised()) {
                TVector<double> normVec = oSet->at(i)->doubleObjectiveVec();
                normaliseToUnitBoxInMemory(normVec, ideal, antiIdeal);
                normalObjectives[i] = normVec;
            }
        }

        for(int i=0; i<setSize; i++) {

            if(!oSet->at(i)->isScalarised()) {

                switch(weightScopeApproach()) {
                case LocalWeight: // construct local weight

                    weight = oSet->at(i)->weightingVec();

                    std::transform(weight.begin(), weight.end(), weight.begin(),
                        [](double w) { return 1.0/(w+0.01); });
                    toUnitVec(weight, 1.0);

//                    for(int j=0; j<weight.size(); j++) {
//                        weight[j] = 1.0 / oSet->at(i)->weightingVec()[j];
//                    }
//                    toUnitVec(weight, 1.0);
                    break;
                case GlobalWeight:
                default:
                    // do nothing
                    break;
                }

                // Scalarise
                double cost;
                switch(TP_scalarisingFunction()) {
                case (Tigon::WeightedSum):
                    cost = weightedSum(weight, normalObjectives[i]);
                    break;
                case (Tigon::WeightedChebyshev):
                    cost = weightedChebyshev(weight, normalObjectives[i]);
                    break;
                case (Tigon::WeightedChebyshevAugmented):
                    cost = weightedChebyshevAugmented(weight, normalObjectives[i]);
                    break;
                case (Tigon::WeightedLp):
                    cost = weightedLp(weight, normalObjectives[i], pNorm());
                    break;
                default:
                    throw TException(this->className(),
                                     UnrecognisedScalarisationFunction);
                }
                oSet->at(i)->defineCost(cost);
                if(weightScopeApproach() == LocalWeight) {
                    oSet->at(i)->defineScalarised();
                }
            }
        }
    }
}

TString GeneralizedDecomposition::name()
{
    return m_name;
}

TString GeneralizedDecomposition::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
