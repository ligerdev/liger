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
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Representation {

NormalDistribution::NormalDistribution()
{
    m_type = Tigon::NormalDistType;
    m_mean = 0.0;
    m_std  = 1.0;
    defineBoundaries(m_mean - 6.0*m_std, m_mean + 6.0*m_std);
    m_normDist = new boost::math::normal_distribution<double>(m_mean, m_std);
}

NormalDistribution::NormalDistribution(const NormalDistribution &dist)
    : IDistribution(dist)
{
    m_type = Tigon::NormalDistType;
    m_mean = dist.m_mean;
    m_std  = dist.m_std;
    m_normDist = new boost::math::normal_distribution<double>(m_mean, m_std);
}

NormalDistribution::NormalDistribution(double mean, double std)
{
    m_type = Tigon::NormalDistType;
    m_mean = mean;
    m_std = (std > 0.0 ? std : 1.0);
    defineBoundaries(m_mean - 6.0*m_std, m_mean + 6.0*m_std);
    m_normDist = new boost::math::normal_distribution<double>(m_mean, m_std);
}

NormalDistribution::NormalDistribution(TVector<double> parameters)
{
    m_type = Tigon::NormalDistType;
    m_mean = (parameters.size() > 0 ? parameters[0] : 0.0);
    m_std = ( (parameters.size() > 1) && (parameters[1] > 0.0)
            ? parameters[1] : 1.0);
    defineBoundaries(m_mean - 6.0*m_std, m_mean + 6.0*m_std);
    m_normDist = new boost::math::normal_distribution<double>(m_mean, m_std);
}

NormalDistribution::~NormalDistribution()
{
    delete m_normDist;
}

NormalDistribution* NormalDistribution::clone() const
{
    return (new NormalDistribution(*this));
}

void NormalDistribution::defineMean(double mean)
{
    clearDistribution();
    m_mean      = mean;
    defineBoundaries(m_mean - 6.0*m_std, m_mean + 6.0*m_std);
    *m_normDist = boost::math::normal_distribution<double>(m_mean, m_std);
}

void NormalDistribution::defineStd(double std)
{
    clearDistribution();
    if (std > 0.0) {
        m_std       = std;
        defineBoundaries(m_mean - 6.0*m_std, m_mean + 6.0*m_std);
        *m_normDist = boost::math::normal_distribution<double>(m_mean, m_std);
    }
}

double NormalDistribution::sample()
{
    return TRAND.randNorm(m_mean, m_std);
}

double NormalDistribution::mean()
{
    return m_normDist->mean();
}

double NormalDistribution::median()
{
    return boost::math::median(*m_normDist);
}

double NormalDistribution::percentile(double p)
{
    if(p>=1.0) {
        p = 1.0;
    } else if(p<=0.0) {
        p = 0.0;
    }
    return boost::math::quantile(*m_normDist, p);
}

double NormalDistribution::variance()
{
    return boost::math::variance(*m_normDist);
}

double NormalDistribution::std()
{
    return boost::math::standard_deviation(*m_normDist);
}

double NormalDistribution::pdf(double z)
{
    return boost::math::pdf(*m_normDist, z);
}

double NormalDistribution::cdf(double z)
{
    return boost::math::cdf(*m_normDist, z);
}

void NormalDistribution::negate()
{
    defineMean(-m_mean);
}

void NormalDistribution::generatePDF()
{
    if(m_z.empty()) {
        generateZ();
    }

    m_pdf.resize(m_nSamples);
    for(int i=0; i<m_nSamples; i++) {
        m_pdf[i] = pdf(m_z[i]);
    }
}

TStringList NormalDistribution::parameterNames() const
{
    TStringList names;
    names << "mean" << "std";
    return names;
}

TVector<double> NormalDistribution::parameters() const
{
    TVector<double> params;
    params << m_mean << m_std;
    return params;
}

void NormalDistribution::defineParameters(const TVector<double> &parameters)
{
    if(parameters.size() > 1) {
        clearDistribution();
        m_mean = parameters[0];
        m_std  = parameters[1];
        defineBoundaries(m_mean - 6.0*m_std, m_mean + 6.0*m_std);
        if(m_normDist) {
            delete m_normDist;
        }
        m_normDist = new boost::math::normal_distribution<double>(m_mean, m_std);
    }
}

} // namespace Representation
} // namespace Tigon
