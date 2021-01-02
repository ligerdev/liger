/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef CONSTRDOMRELATION_H
#define CONSTRDOMRELATION_H

#include <tigon/Utils/Dominance/DominanceRelation.h>

namespace Tigon {
namespace Representation {

/**
 * @brief The ConstrDomRelation class
 * Note that this class uses virtual inheritance from DominanceRelation.
 * This ensures that there will be only one instance of DominanceRelation in a
 * diamond inheritance case. Currently the classes DominanceRelation, Preferability,
 * ConstrDomRelation and PreferabilityConstraintHandling are organised in a
 * diamond inheritance structure. When a pointer or reference to DominanceRelation
 * is initialised with PreferabilityConstraintHandling, there will be only one
 * instance of DominanceRelation, as opposed to two in case virtual inheritance
 * wasn't used.
 */
class LIGER_TIGON_EXPORT ConstrDomRelation : virtual public DominanceRelation
{
public:
    ConstrDomRelation();
    ~ConstrDomRelation();

    tribool isBetterThan(const TVector<double> &a, const TVector<double> &b) const;

    // Parameters
    void defineThresholdVec(const TVector<double>& t);
    TVector<double> thresholdVec()              const;

    void defineConstraintVecA(const TVector<double>& ag);
    TVector<double> constraintVecA()               const;

    void defineConstraintVecB(const TVector<double>& bg);
    TVector<double> constraintVecB()               const;

protected:
    double solutionConstraintViolation(const TVector<double>& g) const;

private:
    TVector<double> m_thresholdVec;
    TVector<double> m_constrainedVecA;
    TVector<double> m_constrainedVecB;
};

} // namespace Representation
} // namespace Tigon

#endif // CONSTRDOMRELATION_H
