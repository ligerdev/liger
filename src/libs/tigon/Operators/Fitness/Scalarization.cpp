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
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Fitness/Scalarization.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

Scalarization::Scalarization()
{
    initialise();
}

Scalarization::Scalarization(Tigon::Representation::IPSet* ipset)
    : IFitness(ipset)
{
    initialise();
}

Scalarization::~Scalarization()
{

}

void Scalarization::initialise()
{
    TP_defineScalarisingFunction(Tigon::DefaultScalarisationFunction);
    TP_defineWeightScopeApproach(Tigon::GlobalWeight);

    TStringList types;
    types << Tigon::ETReal << Tigon::ETInteger;
    defineSupportedElementTypes(types);
}

void Scalarization::evaluateNode()
{
    // determine the global optimal weighting vector
    TVector<double> weight;
    switch(TP_weightScopeApproach()) {
    case LocalWeight:
        // do nothing
        break;
    case GlobalWeight: // construct global weight
    default:
        weight = dirVec();
        break;
    }

    TVector<double> ideal = IElementVecToRealVec(idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());

    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();

        for(auto sol : oSet->all()) {

            if(!sol->isScalarised()) {

                // determine the local optimal weighting vector
                switch(TP_weightScopeApproach()) {
                case LocalWeight:
                    weight = sol->weightingVec();
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

void Scalarization::
TP_defineScalarisingFunction(Tigon::ScalarisationType t, double p)
{
    m_scalarisingFunc = t;
    if(p > 0.0) {
        m_pNorm = p;
    } else {
        m_pNorm = 1.0;
    }
}

Tigon::ScalarisationType Scalarization::TP_scalarisingFunction() const
{
    return m_scalarisingFunc;
}

void Scalarization::TP_defineWeightScopeApproach(WeightScope s)
{
    m_weightScopeApproach = s;
}

Tigon::WeightScope Scalarization::TP_weightScopeApproach() const
{
    return m_weightScopeApproach;
}

double Scalarization::pNorm() const
{
    return m_pNorm;
}

TString Scalarization::name()
{
    return TString("Scalarization");
}

TString Scalarization::description()
{
    return TString("Assigns every solution in the set a scalar value that is "
                   "determined by a scalarization function.");
}

} // namespace Operators
} // namespace Tigon
