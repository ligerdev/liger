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
#include <tigon/Operators/Initialisation/WeightVectorInit.h>
#include <tigon/Operators/Initialisation/RandomInit.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Problems/Problem.h>

#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/ExceptionHandling/TException.h>

// Boost includes
#include <boost/math/special_functions/factorials.hpp>
using namespace boost::math;

namespace Tigon {
namespace Operators {

WeightVectorInit::WeightVectorInit()
{
    initialise();
}

WeightVectorInit::WeightVectorInit(Representation::IPSet* ipset) :
    IInitialisation(ipset)
{
    initialise();
}

void WeightVectorInit::initialise()
{
    addProperty("NumberPointsPerDimension"
                , TString("Defines the number of weight vectors per dimension in"
                          " objective space. This corresponds to the precision"
                          " of the simplex lattice structure where higher values"
                          " lead to higher precision (conversely more weight"
                          " vectors). When the value is equal to -1, the total"
                          " number of weight vectors is determined based on the"
                          " population size. The criterion adopted in this case"
                          " gives a total number of weight vectors that is equal"
                          " to or higher than the population size."
                          "Default is -1")
                , getTType(int));

//    m_propertyNames.push_back("Regularisation approach that determines the association between weight vectors and solutions");
//    m_propertyDescriptions.push_back("When weight vectors are created, or modified, they need to be associated with "
//                                  "the solution set. For each weight vector, it is important to identify the "
//                                  "closest solution in order to reduce the path that a solution needs to travel "
//                                  "across the search space until it provides a good POF--approximation for that "
//                                  "particular direction.\n"
//                                  "Two approaches are available:\n"
//                                  "1) Initial order: the weight vectors are simply paired with the solutions "
//                                  " in their original order."
//                                  "2) Centroid based order with angular distance: First, the weight vectors are "
//                                  "sorted in ascending order in terms of their distance to the centroid. The centroid is "
//                                  "determined using all the weight vectors, and corresponds to their mean "
//                                  "value. Then, each weight vector is paired with the closest solution: "
//                                  "starting first with the weight vector that is closer to the centroid, "
//                                  "and finishing on the weight vectors that is farther away from the "
//                                  "centroid. The distance measure determines the angle formed between two "
//                                  "vectors. The vectors have their starting point on zero of the Cartesian "
//                                  "grid, and their endpoints on the coordinates of either the solutions or "
//                                  "the weight vectors. The same distance measure is applied to measure "
//                                  "distances between: (i) the weight vectors and the centroid; and "
//                                  "(ii) the weight vectors and the solutions.\n"
//                                  "Default is CentroidOrderAngle.");

    TP_defineNumberPointsPerDimension(Tigon::DefaultNumberPointsPerDimension);
    TP_defineRegularisationMethod(Tigon::CentroidBasedOrder);
    TP_defineDistanceMeasure(Tigon::AngleDistance);

    defineKeepArchive(true);  // to enable calculation of the nadir vector

    m_determineWeightVectors = true;
    m_fixedPopulationSize = -1;
}

WeightVectorInit::~WeightVectorInit()
{

}

void WeightVectorInit::evaluateNode()
{    
    TVector<IElementSPtr> goals = goalVec();
    TVector<IElementSPtr>::iterator iter =
            std::find_if(goals.begin(),goals.end(),
            [](IElementSPtr g){
                tribool rs = *g==g->minValue();
                return (rs.value == false);
            });

    // indicates if goals have been specified
    bool isGoalVectorUsed = iter != goals.end();

    if(isGoalVectorUsed) {
        if(m_normGoalVec.empty()) { // first time here
            m_normGoalVec.resize(objectiveVecSize());
            m_determineWeightVectors = true;
            updateGoalVec();
        }

        if(isNewGoalVec()) {    // check if goal vector has changed
            m_determineWeightVectors = true;
            updateGoalVec();
        }
    }

    if(m_determineWeightVectors) {

        while( hasNextOutputSet() ) {
            ISet* oSet = nextOutputSet();

            if(m_fixedPopulationSize==-1) {
                m_fixedPopulationSize = oSet->size();
            }

            if(m_fixedPopulationSize < 3) return;   // it should not happen

            // read lattice resolution
            int H = TP_numberPointsPerDimension();  // read current value

            /// 1. if H==-1
            ///    determine the closest H (equal or higher) to the
            ///    population size

            // Initial estimation of the lattice resolution
            int h=1;
            int M = objectiveVecSize();
            int completeLatticeSetSize = M;

            // find the closest H which is equal or bigger than
            // the given initialPopSize
            if(H==-1) {
                while(completeLatticeSetSize<m_fixedPopulationSize) {
                    h++;
                    completeLatticeSetSize = simplexLatticeSize(h,M);
                }
                H = h;
            }
            // set the popsize equal to the given H and M
            else {
                completeLatticeSetSize = simplexLatticeSize(H,M);
            }

            // ****************************************************************
            // 2. generate a simplex lattice design
            //    remove vectors that are non-dominated with the goal vector
            //    (normalised according to the ideal and nadir vectors)
            //    increase the lattice resolution until the number of solutions
            //    reaches the desired size
            // ****************************************************************

            int finalPopSize = 0;

            if(isGoalVectorUsed) {

                TVector<bool> essentialObs = problem()->essentialObjectives();
                bool isThereRedundantObjs = vectorContains(essentialObs,false);
                if(!isThereRedundantObjs) {
                    m_refSet = preferredSimplexLattice(H, M,
                                                       m_normGoalVec);
                } else {
                    m_refSet = preferredSimplexLatticeRedundantComponents(H, M,
                                                                  m_normGoalVec,
                                                                  essentialObs);
                }

            } else {
                m_refSet = simplexLatticeIterative(H, M);
            }

            // add the extreme weight vectors
//            for(int i=0; i<M; i++) {
//                TVector<double> wvObj(M,0.0);
//                wvObj[i]=1.0;

//                bool addVector=true;
//                for(auto row : m_refSet) {
//                    if(areVectorsEqual(row,wvObj)) {
//                        addVector=false;
//                        break;
//                    }
//                }
//                if(addVector) {
//                    m_refSet.push_back(wvObj);
//                }
//            }
            finalPopSize = m_refSet.size();

            // *****************************************
            // 3. Resize the MainSet size (if necessary)
            // *****************************************

            int sizeDiff = oSet->size() - finalPopSize;
            if(sizeDiff!=0) {
                if(sizeDiff > 0) {    // delete solutions
                    for(int i=0; i<sizeDiff; i++) {
                        oSet->remove(finalPopSize);
                    }
                }
                else {  // add solutions

                    RandomInit* init = new RandomInit(this);
                    init->defineOutputTags(
                                TStringList({Tigon::TTempNewSolutions}));
                    init->TP_defineSetSize(-sizeDiff);
                    // we are doing this because RandomInit only operates if the
                    // "set" doesn't exist, i.e., has been initialised.
                    init->evaluateOnlyThisNode();
                    // we don't call evaluate because it will also evaluate all
                    // the nodes in (this)

                    // append the entire created set
                    oSet->append(init->outputSet(0));

                    delete init;
                }
            }

            // *********************************************
            // 4. Attribute to each solution a weight vector
            // *********************************************

            // map between a solution and a weight vector.
            // solMap[i]=j maps the ith solution with the jth weight vector.
            TVector<int> solMap;

            switch(TP_regularisationMethod()) {
            case (Tigon::InitialOrder):
            {
                solMap.resize(finalPopSize);
                std::iota(solMap.begin(),solMap.end(),0);
                break;
            }
            case (Tigon::CentroidBasedOrder):
            {
                solMap = centroidDistanceOrder(m_refSet, oSet->all(),
                                               TP_distanceMeasure());
                break;
            }
            default:
                throw TException(this->className(),
                                 UnrecognisedRegularisationMethod);
            }

            // Assign to each solution a weight vector by using the map
            for(int i=0; i<finalPopSize; i++) {
                oSet->at(i)->defineWeightingVec(m_refSet[solMap[i]]);
            }
            solMap.clear();
        }
    }
    m_determineWeightVectors = false;
}

bool WeightVectorInit::isNewGoalVec()
{
    TVector<double> ideal = IElementVecToRealVec(idealVec());
    TVector<double> nadir = IElementVecToRealVec(nadirVec());
    TVector<double> goalVector = normaliseForSimplexLattice(
                                    IElementVecToRealVec(goalVec()),
                                    ideal, nadir);

    if(areVectorsEqual(goalVector, m_normGoalVec)) {
        return false;
    }
    return true;
}

void WeightVectorInit::updateGoalVec() {

    TVector<double> ideal = IElementVecToRealVec(idealVec());
    TVector<double> nadir = IElementVecToRealVec(nadirVec());
    TVector<double> goalVector = normaliseForSimplexLattice(
                                    IElementVecToRealVec(goalVec()),
                                    ideal, nadir);

    // update the goal vector
    m_normGoalVec = goalVector;
}

// Define number of points per dimension (H)
void WeightVectorInit::TP_defineNumberPointsPerDimension(int H)
{
    if(H >= 2) {
        m_numberPointsPerDimension = H;
    }
    if(H==-1) {
        m_numberPointsPerDimension = -1;
    }
}

// Get number of points per dimension (H)
int WeightVectorInit::TP_numberPointsPerDimension() const
{
    return m_numberPointsPerDimension;
}

void WeightVectorInit::TP_defineRegularisationMethod(
        Tigon::RegularisationApproach r)
{
    switch(r) {
    case (Tigon::InitialOrder):
        m_regularisationMethod = Tigon::InitialOrder;
        break;
    case (Tigon::CentroidBasedOrder):
        m_regularisationMethod = Tigon::CentroidBasedOrder;
        break;
    default:
        throw TException(this->className(),
                         UnrecognisedRegularisationMethod);
    }
}

RegularisationApproach WeightVectorInit::TP_regularisationMethod() const
{
    return m_regularisationMethod;
}

void WeightVectorInit::TP_defineDistanceMeasure(DistanceMeasure distance)
{
    m_distanceMeasure = distance;
}

DistanceMeasure WeightVectorInit::TP_distanceMeasure() const
{
    return m_distanceMeasure;
}

TString WeightVectorInit::name()
{
    return TString("Weight Vectors Initialization");
}

TString WeightVectorInit::description()
{
    return TString("Creates a set of reference direction vectors"
                   "(or weight vectors) using Simplex Lattice method.\n"
                   "The weight vectors that are dominated by a goal vector are"
                   " removed from the set.\n"
                   "The number of weight vectors is given by C^{H+M-1}_{M-1}"
                   " if no weight vectors are dominated by the goal vector.\n"
                   "Attributes a weight vector to each solution in the MainSet\n"
                   "If necessary, resizes the MainSet to correspond to the size"
                   " of the number of weight vectors.");
}

} // namespace Operators
} // namespace Tigon
