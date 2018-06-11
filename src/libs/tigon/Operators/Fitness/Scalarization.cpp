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
#include <tigon/Operators/Fitness/Scalarization.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
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
    m_name = TString("Scalarization");
    m_description = TString("Assigns every solution in the set a scalar value"
                            " by using scalarization functions.");

//    addProperty("ScalarisingFunction"
//                , TString("The available scalarising functions are: weighted sum, "
//                          "weighted Chebyshev, and weighted Lp.\n"
//                          "Default is weighted sum.")
//                , typeid(Tigon::ScalarisationType).hash_code());
//    addProperty("WeightScopeApproach"
//                , TString("The weights used for scalarisation "
//                          "correspond to the 'local' weight of each solution "
//                          "or to the 'global' direction vector.\n"
//                          "Default is global.")
//                , typeid(Tigon::WeightScope).hash_code());

    TP_defineScalarisingFunction(Tigon::DefaultScalarisationFunction);
    TP_defineWeightScopeApproach(Tigon::GlobalWeight);

    TStringList types;
    types << Tigon::ETReal << Tigon::ETInteger;
    defineSupportedElementTypes(types);
}

void Scalarization::evaluateNode()
{
    int M=objectiveVecSize();
    // determine the global optimal weighting vector
    TVector<double> weight;
    switch(weightScopeApproach()) {
    case LocalWeight:
        // do nothing
        break;
    case GlobalWeight: // construct global weight
    default:
        weight = dirVec();
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

        TVector<bool> isScalarised(setSize,false);
        TVector<double> costArchive(setSize);
        TVector<TVector<double>> weights(setSize,TVector<double>(M));
        for(int i=0; i<setSize; i++) {

            if(!oSet->at(i)->isScalarised()) {

                isScalarised[i] = true;

                // determine the local optimal weighting vector
                switch(weightScopeApproach()) {
                case LocalWeight:
                    weight = oSet->at(i)->weightingVec();

                    weights[i] = weight;

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
                costArchive[i] = cost;
                oSet->at(i)->defineCost(cost);
                if(weightScopeApproach() == LocalWeight) {
                    oSet->at(i)->defineScalarised();
                }
            }
        }
        int p1=0;
    }
}

void Scalarization::TP_defineScalarisingFunction(Tigon::ScalarisationType t, double p)
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

Tigon::WeightScope Scalarization::weightScopeApproach()
{
    return m_weightScopeApproach;
}

TString Scalarization::name()
{
    return m_name;
}

TString Scalarization::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
