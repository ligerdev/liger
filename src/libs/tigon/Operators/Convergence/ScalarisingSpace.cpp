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
#include <tigon/Operators/Convergence/ScalarisingSpace.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/ScalarisingSpaceUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/SimplexLattice.h>

namespace Tigon {
namespace Operators {

ScalarisingSpace::ScalarisingSpace()
{
    initialise();
}

ScalarisingSpace::ScalarisingSpace(Tigon::Representation::IPSet* ipset)
    : IConvergence(ipset)
{
    initialise();
}

ScalarisingSpace::~ScalarisingSpace()
{

}

void ScalarisingSpace::initialise()
{
    addProperty("ReferenceSetSize",
                TString("The size of the reference set.\n"
                        "When a certain size is requested,\n"
                        "the actual size is equal or larger. Default is 1."),
                getTType(int));

    TP_defineReferenceSetSizeScalarisingSpace(1);
}

void ScalarisingSpace::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    ISet* iSet = setsUnion(inputSets());   

    int nSolutions = 0;
    for(int i=0; i<iSet->size(); i++) {
        if(iSet->at(i)->isEvaluated() == true) {
            nSolutions++;
        }
    }

    if(nSolutions == 0) {
        // ERROR?
        return;
    }

    if(currentIteration() == 0) {
        m_ideal = IElementVecToRealVec(idealVec());
        m_antiIdeal = IElementVecToRealVec(antiIdealVec());
        createReferenceSet();
    }

    // Calculate convergence metric value
    TVector<TVector<double> > actualSet(nSolutions,
                                        TVector<double>(objectiveVecSize()));
    for(int j = 0; j < nSolutions; j++) {        
        actualSet[j] = iSet->at(j)->doubleObjectiveVec();
    }    
    m_convIndicatorValue = scalarisingSpace(actualSet, m_initRefSet, m_ideal, m_antiIdeal);

    delete iSet;
}

void ScalarisingSpace::createReferenceSet()
{
    // Normalise the goal vector
    TVector<double> nadir = IElementVecToRealVec(nadirVec());
    //TVector<double> normGoalVec = IElementVecToRealVec(goalVec());
    //normaliseToUnitBoxInMemory(normGoalVec, m_ideal, nadir);
    TVector<double> normGoalVec = normaliseForSimplexLattice(
                IElementVecToRealVec(goalVec()), m_ideal, nadir);

    //using namespace boost::math;

    // Initial estimation of the lattice resolution
    int h = 1;
    int k = objectiveVecSize();
    double setSize = k;

    while(setSize < m_refSetSize) {
        ++h;
        setSize = factorial<double>(h+k-1) /
                (factorial<double>(k-1) * factorial<double>(h));
    }

    // Create a simplex lattice and remove vectors that are non-dominated
    // with the goal vector (normalised according to the ideal and nadir vectors)
    m_initRefSet = preferredSimplexLattice(h, k, normGoalVec);
}

void ScalarisingSpace::TP_defineReferenceSetSizeScalarisingSpace(int s)
{
    m_refSetSize = s;
}

int ScalarisingSpace::TP_referenceSetSizeScalarisingSpace() const
{
    return m_refSetSize;
}

TString ScalarisingSpace::name()
{
    return TString("ScalarisingSpace Convergence");
}

TString ScalarisingSpace::description()
{
    return TString("A measure for convergence of the solutions to a "
                   "multiobjective problem.\n"
                   "The convergence metric calculates the best costs of the "
                   "current population of the algorithm, aggregated across a "
                   "set of reference directions. The metric is calculated at "
                   "each iteration of the algorithm when solution set is "
                   "updated. The metric is for the single instances of "
                   "performance for each solution in the population.\n"
                   "In order to keep in the same metric space, reference "
                   "directions, ideal and anti-ideal vectors are obtained "
                   "only from the start of the optimisation process, and are "
                   "not updated.");
}

} // namespace Operators
} // namespace Tigon
