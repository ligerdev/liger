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
#include <tigon/Operators/AlgorithmSpecific/MOEADNeighbourhoodUpdate.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

MOEADNeighbourhoodUpdate::MOEADNeighbourhoodUpdate()
{
    MOEADNeighbourhoodUpdate::initialise();
}

MOEADNeighbourhoodUpdate::MOEADNeighbourhoodUpdate(IPSet *ipset)
    : IDirection(ipset)
{
    MOEADNeighbourhoodUpdate::initialise();
}

MOEADNeighbourhoodUpdate::~MOEADNeighbourhoodUpdate()
{

}

void MOEADNeighbourhoodUpdate::initialise()
{
    m_name = TString("Replaces the population (or neighbouring) solutions with"
                     " the child solutions based on their scalarised values");
    m_description = TString("Each solution in the ouput set can be replace by a"
                            " child solution (from the input set). For this to"
                            " happen, first an output set' solution is selected"
                            " with the corresponding child solution. Then, the"
                            " child solution is scalarised by using the output"
                            " set' solution weights."
                            "The output set' solution is replaced by the child"
                            " if the scalarised value of the latter is better"
                            " than the scalarised value of the former.");

    //m_propertyNames.push_back("Scalarising function");
    //m_propertyDescriptions.push_back("This selects the scalarising function used "
    //                              " for scalarising the child solutions."
    //                              "The available scalarising functions are: weighted sum, "
    //                              "weighted Chebyshev, and weighted Lp.\n"
    //                              "Default is weighted sum\n");

    addProperty("MaxNumberReplacements",
                TString("Defines the maximum number of solutions that can be "
                        "replaced by the same child solution.\n"
                        "The default is 2")
                , getTType(int));

    addProperty("IsGeneralisedDecomposition"
                , TString("Flag that indicates if the weight vectors follow the"
                          " principles of generalised decomposition or if the"
                          " original weight vectors are to be used instead.\n"
                          "The default is false\n")
                ,getTType(bool));

    TP_defineScalarisingFunction(Tigon::DefaultScalarisationFunction);
    TP_defineMaxNumberReplacements(Tigon::DefaultMaxNumberReplacements);
}

void MOEADNeighbourhoodUpdate::evaluateNode()
{
    /// \note When Generalised decomposition is used.
    /// Currently, to determine the 'optimal' weight vector, the inverse of the
    /// direction of each component is used. However, this is only optimal for
    /// Weighted Sum and Weighted Chebyshev scalarising functions, and a more
    /// general method is required in case other scalarising functions are to
    /// be used in the future.

    TVector<double> ideal = IElementVecToRealVec(idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());

    // check if input and output sets have the same size
    while(hasNextInputSet()) {

        int n_replacements = 0;

        // child set: expect only one solution per set
        ISet* iSet = nextInputSet();
        ISet* oSet = nextOutputSet(); // population or neighbourhood set

        int oSetSize = oSet->size();
        TVector<int> permIdx = randomPermutationSampling(oSetSize);

        IMappingSPtr child = iSet->at(0); // pick the child
        TVector<double> childObjNormVec = child->doubleObjectiveVec();
        normaliseToUnitBoxInMemory(childObjNormVec, ideal, antiIdeal);

        for(auto pindex : permIdx) {

            IMappingSPtr current = oSet->at(pindex);
            TVector<double> weights = current->weightingVec();

            if(TP_isGeneralisedDecomposition()) {

                std::transform(weights.begin(), weights.end(), weights.begin(),
                    [](double w) { return 1.0/(w+0.01); });
                toUnitVec(weights, 1.0);
            }

            double childCost;

            // calculate cost
            switch(TP_scalarisingFunction()) {
            case (Tigon::WeightedSum):
                childCost = weightedSum(weights, childObjNormVec);
                break;
            case (Tigon::WeightedChebyshev):
                childCost = weightedChebyshev(weights, childObjNormVec);
                break;
            case (Tigon::WeightedChebyshevAugmented):
                childCost = weightedChebyshevAugmented(weights, childObjNormVec);
                break;
            case (Tigon::WeightedLp):
                childCost = weightedLp(weights, childObjNormVec, pNorm());
                break;
            default:
                throw TException(this->className(),
                                 UnrecognisedScalarisationFunction);
            }

            if(childCost < current->doubleCost()) {

                // replace current with child
                current->defineDecisionVec(child->decisionVec());
                current->defineObjectiveVec(child->objectiveVec());
                current->defineEvaluated();
                current->defineCost(childCost);
                current->defineScalarised(true);

                updateIdealNadirVec(current);

                // set constraints (TODO)

                n_replacements++;
                if(n_replacements>=TP_maxNumberReplacements()) {
                    break;
                }
            }
        }
    }
}

void MOEADNeighbourhoodUpdate::TP_defineScalarisingFunction(
        Tigon::ScalarisationType t, double p)
{
    m_scalarisingFunc = t;
    if(p > 0.0) {
        m_pNorm = p;
    } else {
        m_pNorm = 1.0;
    }
}

Tigon::ScalarisationType
MOEADNeighbourhoodUpdate::TP_scalarisingFunction() const
{
    return m_scalarisingFunc;
}

void MOEADNeighbourhoodUpdate::TP_defineMaxNumberReplacements(int r)
{
    if(r>0) {
        m_maxNumberReplacements = r;
    }
}

int MOEADNeighbourhoodUpdate::TP_maxNumberReplacements() const
{
    return m_maxNumberReplacements;
}

void MOEADNeighbourhoodUpdate::TP_defineIsGeneralisedDecomposition(bool b)
{
    m_isGeneralisedDecomposition=b;
}

bool MOEADNeighbourhoodUpdate::TP_isGeneralisedDecomposition() const
{
    return m_isGeneralisedDecomposition;
}

double MOEADNeighbourhoodUpdate::pNorm() const
{
    return m_pNorm;
}

TString MOEADNeighbourhoodUpdate::name()
{
    return m_name;
}

TString MOEADNeighbourhoodUpdate::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
