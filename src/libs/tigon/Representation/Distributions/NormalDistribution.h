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
#ifndef NORMALDISTRIBUTION_H
#define NORMALDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/Representation/Distributions/IDistribution.h>

// boost does not allow to use forward declerations
#include <boost/math/distributions/normal.hpp>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT NormalDistribution : public IDistribution
{
public:
    NormalDistribution();
    NormalDistribution(const NormalDistribution& dist);
    NormalDistribution(double mean, double std);
    NormalDistribution(TVector<double> parameters);
    ~NormalDistribution();

    NormalDistribution* clone() const;

    void defineMean(double mean);
    void defineStd(double std);

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

    void negate();

    void  generatePDF();

    TStringList    parameterNames() const;
    TVector<double> parameters() const;
    void defineParameters(const TVector<double> &parameters);

private:
    double m_mean;
    double m_std;

    boost::math::normal_distribution<double>* m_normDist;
};

} // namespace Representation
} // namespace Tigon

#endif // NORMALDISTRIBUTION_H
