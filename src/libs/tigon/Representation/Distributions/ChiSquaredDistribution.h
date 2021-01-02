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
#ifndef CHISQUAREDDISTRIBUTION_H
#define CHISQUAREDDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/Representation/Distributions/IDistribution.h>

// boost does not allow to use forward declerations
#include <boost/math/distributions/chi_squared.hpp>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ChiSquaredDistribution : public IDistribution
{
public:
    ChiSquaredDistribution();
    ChiSquaredDistribution(const ChiSquaredDistribution& dist);
    ChiSquaredDistribution(double k);
    ChiSquaredDistribution(TVector<double> parameters);
    ~ChiSquaredDistribution();

    ChiSquaredDistribution* clone() const;

    void defineDOF(double k);

    double sample();
    double mean();
    double median();
    double percentile(double p);
    double variance();
    double std();

    //to make overrides visible to the compiler
    using IDistribution::pdf;
    using IDistribution::cdf;
    double pdf(double       z);
    double cdf(double       z);

    ///\brief This does not do anything, since the distribution is non-negative
    void negate();

    void  generatePDF();

    TVector<double> parameters() const;

private:
    double m_dof;

    boost::math::chi_squared_distribution<double>*    m_chiSqDist;
};

} // namespace Representation
} // namespace Tigon

#endif // CHISQUAREDDISTRIBUTION_H
