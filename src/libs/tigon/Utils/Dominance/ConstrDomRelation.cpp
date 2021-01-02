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
#include <tigon/Utils/Dominance/ConstrDomRelation.h>

namespace Tigon {
namespace Representation {

ConstrDomRelation::ConstrDomRelation()
{

}

ConstrDomRelation::~ConstrDomRelation()
{

}

tribool ConstrDomRelation::isBetterThan(const TVector<double> &a,
                                        const TVector<double> &b) const
{
    if( (thresholdVec().empty() == true) ||
        (constraintVecA().empty() == true) ||
        (constraintVecB().empty() == true)) {

        return dominates(a,b);
    }

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

                // Do normal dominance
                dom = dominates(a,b);
            }
        }
    }

    return dom;
}

void ConstrDomRelation::defineThresholdVec(const TVector<double> &t)
{
    m_thresholdVec = t;
}

TVector<double> ConstrDomRelation::thresholdVec() const
{
    return m_thresholdVec;
}

void ConstrDomRelation::defineConstraintVecA(const TVector<double> &ag)
{
    m_constrainedVecA=ag;
}

TVector<double> ConstrDomRelation::constraintVecA() const
{
    return m_constrainedVecA;
}

void ConstrDomRelation::defineConstraintVecB(const TVector<double> &bg)
{
    m_constrainedVecB=bg;
}

TVector<double> ConstrDomRelation::constraintVecB() const
{
    return m_constrainedVecB;
}

double ConstrDomRelation::solutionConstraintViolation(
        const TVector<double>& g) const
{
    double constrViolation = 0.0;
    int C = g.size();
    for(int i=0; i<C; i++) {
        double diff = g[i] - thresholdVec()[i];
        if(diff > 0.0) {
            constrViolation += diff;
        }
    }

    return constrViolation;
}

} // namespace Representation
} // namespace Tigon
