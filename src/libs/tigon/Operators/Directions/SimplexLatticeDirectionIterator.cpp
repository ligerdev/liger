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
#include <tigon/Operators/Directions/SimplexLatticeDirectionIterator.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Representation/Problems/Problem.h>

// Boost includes
#include <boost/math/special_functions/factorials.hpp>

namespace Tigon {
namespace Operators {

SimplexLatticeDirectionIterator::SimplexLatticeDirectionIterator()
{
    initialise();
}

SimplexLatticeDirectionIterator::SimplexLatticeDirectionIterator(
        Tigon::Representation::IPSet* ipset)
    : IDirection(ipset)
{
    initialise();
}

SimplexLatticeDirectionIterator::~SimplexLatticeDirectionIterator()
{

}

void SimplexLatticeDirectionIterator::initialise()
{
    addProperty("ReferenceSetSize"
                , TString("The size of the reference set.\n"
                           "When a certain size is requseted,\n"
                           "the actual size is equal or larger.\n"
                           "Default is 1.")
                , getTType(int));

    TP_defineReferenceSetSize(1);
    defineKeepArchive(true);  // to enable calculation of the nadir vector
    clearInputTags();
    clearOutputTags();

}

void SimplexLatticeDirectionIterator::evaluateNode()
{
    // This part is only executed on the first iteration
    if(m_referenceSet.empty()) {
        m_normGoalVec.resize(objectiveVecSize());
        isNewGoalVec();
        createReferenceSet();
    }

    nextDirection();
}

void SimplexLatticeDirectionIterator::TP_defineReferenceSetSize(int s)
{
    m_refSetSize = s;
}

int SimplexLatticeDirectionIterator::TP_referenceSetSize() const
{
    return m_refSetSize;
}

int SimplexLatticeDirectionIterator::actualReferenceSetSize() const
{
    return m_actualRefSetSize;
}

void SimplexLatticeDirectionIterator::createReferenceSet()
{
    // Initial estimation of the lattice resolution
    int h = 1;
    int k = objectiveVecSize();
    double setSize = k;

    while(setSize < m_refSetSize) {
        h++;
        setSize = simplexLatticeSize(h, k);
    }

    // Create a simplex lattice and remove vectors that are non-dominated
    // with the goal vector (normalised according to the ideal and nadir vectors)

    TVector<bool> essentialObs = problem()->essentialObjectives();
    bool isThereRedundantObjs = vectorContains(essentialObs,false);

    if(!isThereRedundantObjs) {
        m_referenceSet = preferredSimplexLattice(h, k, m_normGoalVec);
    } else {
        m_referenceSet = preferredSimplexLatticeRedundantComponents(h, k,
                                                                m_normGoalVec,
                                                                essentialObs);
    }
    m_actualRefSetSize = m_referenceSet.size();

    m_referenceSet = Tigon::shuffle(m_referenceSet);
    m_currentVec = -1;
}

void SimplexLatticeDirectionIterator::nextDirection()
{
    if(m_currentVec < m_actualRefSetSize - 1) {
        m_currentVec++;
    } else if(isNewGoalVec()) {
        createReferenceSet();
        m_currentVec = 0;
    } else {        
        m_currentVec = 0;
        m_referenceSet = Tigon::shuffle(m_referenceSet);
    }
    defineDirVec(m_referenceSet[m_currentVec]);
}

bool SimplexLatticeDirectionIterator::isNewGoalVec()
{
    bool isChanged = false;

    // Normalise the goal vector
    TVector<double> ideal = IElementVecToRealVec(idealVec());
    TVector<double> nadir = IElementVecToRealVec(nadirVec());
    TVector<double> normGoalVec = normaliseForSimplexLattice(
                                      IElementVecToRealVec(goalVec()),
                                      ideal, nadir);

    for(int i=0; i<objectiveVecSize(); i++) {
        if(!areDoublesEqual(m_normGoalVec[i],normGoalVec[i])) {
            for(int j=i; j<objectiveVecSize(); j++) {
                m_normGoalVec[j] = normGoalVec[j];
            }
            isChanged = true;
            break;
        }
    }
    return isChanged;
}

TString SimplexLatticeDirectionIterator::name()
{
    return TString("Simplex Lattice Direction Iterator");
}

TString SimplexLatticeDirectionIterator::description()
{
    return TString("Creates a set of reference direction vectors using Simplex "
                   "Lattice method.\n"
                   "The reference set is restricted to direction vectors that "
                   "are dominated or dominate the goal vector, normalised "
                   "between the ideal and nadir vectors.\n"
                   "Cycles through the set at every execution.\n"
                   "Reconstructs the set according to preferences at the end of "
                   "each cycle.\n"
                   "Does not have input or output sets.");
}

} // namespace Operators
} // namespace Tigon
