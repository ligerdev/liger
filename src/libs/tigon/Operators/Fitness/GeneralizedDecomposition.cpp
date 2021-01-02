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
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Fitness/GeneralizedDecomposition.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

GeneralizedDecomposition::GeneralizedDecomposition()
{
    initialise();
}

GeneralizedDecomposition::GeneralizedDecomposition(
        Tigon::Representation::IPSet* ipset)
    : Scalarization(ipset)
{
    initialise();
}

GeneralizedDecomposition::~GeneralizedDecomposition()
{

}

void GeneralizedDecomposition::initialise()
{

}

void GeneralizedDecomposition::evaluateNode()
{
    /// Currently, to determine the 'optimal' weight vector, the inverse of the
    /// direction of each component is used. However, this is only optimal for
    /// Weighted Sum and Weighted Chebyshev scalarising functions, and a more
    /// general method is required in case other scalarising functions are to
    /// be used in the future.

    auto gfunc = [](double w)
    {
        return 1.0/(w+0.01);
    };

    // define the optimal weighting vector
    TVector<double> weight;
    switch(TP_weightScopeApproach()) {
    case LocalWeight:
        // do nothing
        break;
    case GlobalWeight: // construct global weight
    default:
        weight = dirVec();

        std::transform(weight.begin(), weight.end(), weight.begin(), gfunc);
        toUnitVec(weight, 1.0);

        break;
    }

    TVector<double> ideal = IElementVecToRealVec(idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();

        for(auto sol : oSet->all()) {

            if(!sol->isScalarised()) {

                switch(TP_weightScopeApproach()) {
                case LocalWeight: // construct local weight

                    weight = sol->weightingVec();

                    std::transform(weight.begin(), weight.end(),
                                   weight.begin(), gfunc);
                    toUnitVec(weight, 1.0);

                    break;
                case GlobalWeight:
                default:
                    // do nothing
                    break;
                }

                // normalise the objectives according to the ideal and antiIdeal
                TVector<double> normObj =
                        normaliseToUnitBox(sol->doubleObjectiveVec(),
                                           ideal, antiIdeal);

                // Scalarise
                double cost;
                switch(TP_scalarisingFunction()) {
                case (Tigon::WeightedSum):
                    cost = weightedSum(weight, normObj);
                    break;
                case (Tigon::WeightedChebyshev):
                    cost = weightedChebyshev(weight, normObj);
                    break;
                case (Tigon::WeightedChebyshevAugmented):
                    cost = weightedChebyshevAugmented(weight, normObj);
                    break;
                case (Tigon::WeightedLp):
                    cost = weightedLp(weight, normObj, pNorm());
                    break;
                default:
                    throw TException(this->className(),
                                     UnrecognisedScalarisationFunction);
                }
                sol->defineCost(cost);

                if(TP_weightScopeApproach() == LocalWeight) {
                    sol->defineScalarised();
                }

            }
        }
    }
}

TString GeneralizedDecomposition::name()
{
    return TString("Generalized Decomposition");
}

TString GeneralizedDecomposition::description()
{
    return TString("Assigns every solution in the set a scalar value that is "
                   "determined by a scalarization function according to the "
                   "concept of generalized decomposition, based on \"Ioannis "
                   "Giagkiozis, Robin C. Purshouse, Peter J. Fleming, "
                   "Generalized Decomposition, in International Conference on "
                   "Evolutionary Multi-Criterion Optimization, EMO 2013, "
                   "pp 428-442.\"");
}

} // namespace Operators
} // namespace Tigon
