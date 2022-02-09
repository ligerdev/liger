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
#ifndef CODEMDISTRIBUTION_H
#define CODEMDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
class LinearInterpolator;
namespace Representation
{
class IDistribution;
}
}

using namespace Tigon::Representation;

namespace CODeM{


class LIGER_TIGON_EXPORT CODeMDistribution
{
public:
    CODeMDistribution(IDistribution* d,
                      const Tigon::TVector<double> &oVec,
                      double lowerBound,
                      double upperBound,
                      const Tigon::TVector<double> &ideal,
                      const Tigon::TVector<double> &antiIdeal,
                      double dirPertRad,
                      double dirPertNorm);
    ~CODeMDistribution();

    Tigon::TVector<double> sampleDistribution();

    void defineDirectionPertRadius(double r);
    void definePerturbationNorm(double p);
    // 2-norm direction
    void defineDirection(const Tigon::TVector<double> &oVec);
    void defineIdealAndAntiIdeal(const Tigon::TVector<double> &ideal,
                                 const Tigon::TVector<double> &antiIdeal);
    void defineDistribution(IDistribution* d);

private:
    IDistribution*    m_distribution;
    double             m_directionPertRadius;
    Tigon::TVector<double>    m_direction;
    Tigon::TVector<double>    m_ideal;
    Tigon::TVector<double>    m_antiIdeal;
    double             m_lb;
    double             m_ub;
    double             m_pNorm;
};

} //namespace CODeM

#endif // CODEMDISTRIBUTION_H
