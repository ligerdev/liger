/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Utils/Dominance/PreferabilityConstraintHandling.h>

namespace Tigon {
namespace Representation {

PreferabilityConstraintHandling::PreferabilityConstraintHandling()
{

}

PreferabilityConstraintHandling::~PreferabilityConstraintHandling()
{

}

tribool PreferabilityConstraintHandling::isBetterThan(const TVector<double> &a,
                                                      const TVector<double> &b) const
{
    if( (goalVec().empty()==true) || (setGoals().empty()==true) ) {

        // Do normal dominance
        return dominates(a,b);
    }

    tribool dom;
    if ( (!thresholdVec().empty()) &&
         (!constraintVecA().empty()) && (!constraintVecB().empty())) {

        // Constraint Optimization
        dom = preferability_constraint(a, b);

    }
    else {
        // Normal preferability with just goals
        dom = preferability(a, b);
    }

    return dom;
}

void PreferabilityConstraintHandling::defineParameters(
        const TVector<TVector<double>> &c)
{
    if( (c.size()==2) || (c.size()==5) ) {
        defineGoalVec(c[0]);

        TVector<bool> p1;
        for(auto p : c[1]) {
            p1.push_back((bool)p);
        }

        defineSetGoals(p1);
    }

    if(c.size()==5) {
        defineThresholdVec(c[2]);
        defineConstraintVecA(c[3]);
        defineConstraintVecB(c[4]);
    }
}


tribool PreferabilityConstraintHandling::preferability_constraint(
        const TVector<double> &a, const TVector<double> &b) const
{
    int C = thresholdVec().size(); // number of constraints

    // check if 'a' is feasible
    bool aFeasible = true;
    for(int i=0; i<C; i++) {
        if(constraintVecA()[i] > thresholdVec()[i]) {
            aFeasible = false; break;
        }
    }

    // check if 'a' is feasible
    bool bFeasible = true;
    for(int i=0; i<C; i++) {
        if(constraintVecB()[i] > thresholdVec()[i]) {
            bFeasible = false; break;
        }
    }

    tribool dom;

    if( (!aFeasible) && (!bFeasible) ) { // both a and b are not feasible

        double constrViolationA = solutionConstraintViolation(constraintVecA());
        double constrViolationB = solutionConstraintViolation(constraintVecB());

        // a violates more than b
        if(constrViolationA > constrViolationB) {
            dom = false; // b dominates
        }
        else {
            // b violates more than a
            if(constrViolationA < constrViolationB) {
                dom = true; // a dominates
            }
            else {
                // non-dominated since both equally violate
                dom = incomparable;
            }
        }
    }
    else {

        if( (!aFeasible) && (bFeasible) ) { // a violates but b doesn't
            dom = false; // b dominates
        }
        else {
            if( (aFeasible) && (!bFeasible) ) { // b violates but a doesn't
                dom = true; // a dominates
            }
            else {

                // normal preferability
                dom = preferability(a, b);
            }
        }
    }

    return dom;
}

double PreferabilityConstraintHandling::solutionConstraintViolation(
        const TVector<double>& g) const
{
    double constrViolation = 0.0;
    int C = g.size();
    for(int i=0; i<C; i++) {
        double diff = thresholdVec()[i] - g[i];
        if(diff > 0.0) {
            constrViolation += diff;
        }
    }

    return constrViolation;
}

void PreferabilityConstraintHandling::defineThresholdVec(
        const TVector<double> &t)
{
    m_thresholdVec = t;
}

TVector<double> PreferabilityConstraintHandling::thresholdVec() const
{
    return m_thresholdVec;
}

void PreferabilityConstraintHandling::defineConstraintVecA(
        const TVector<double> &ag)
{
    m_constrainedVecA=ag;
}

TVector<double> PreferabilityConstraintHandling::constraintVecA() const
{
    return m_constrainedVecA;
}

void PreferabilityConstraintHandling::defineConstraintVecB(
        const TVector<double> &bg)
{
    m_constrainedVecB=bg;
}

TVector<double> PreferabilityConstraintHandling::constraintVecB() const
{
    return m_constrainedVecB;
}

} // namespace Representation
} // namespace Tigon
