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
#include <tigon/Representation/Distributions/UniformDistribution.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Representation {

UniformDistribution::UniformDistribution()
{
    m_uniDist = nullptr;
    m_type = Tigon::UniformDistType;
    defineBoundaries(0.0, 1.0);
}

UniformDistribution::UniformDistribution(const UniformDistribution& dist)
    : IDistribution(dist)
{
    m_type = Tigon::UniformDistType;
    m_uniDist = new boost::math::uniform_distribution<double>(m_lb, m_ub);
}

UniformDistribution::UniformDistribution(double lb, double ub)
{
    m_uniDist = nullptr;
    m_type = Tigon::UniformDistType;
    defineBoundaries(lb, ub);
}

UniformDistribution::UniformDistribution(TVector<double> parameters)
{
    m_uniDist = nullptr;
    m_type = Tigon::UniformDistType;
    double lb = 0.0;
    double ub = 1.0;
    if(parameters.size() > 0) {
        lb = parameters[0];
        if((parameters.size() > 1) && (parameters[1] > lb)) {
            ub = parameters[1];
        } else {
            ub = lb + Tigon::DistMinInterval;
        }
    }
    defineBoundaries(lb, ub);
}

UniformDistribution::~UniformDistribution()
{
    delete m_uniDist;
    m_uniDist = nullptr;
}

UniformDistribution* UniformDistribution::clone() const
{
    return (new UniformDistribution(*this));
}

void UniformDistribution::defineBoundaries(double lb, double ub)
{
    clearDistribution();

    if(lb >= ub) {
        if(lb == 0.0) {
            ub = Tigon::DistMinInterval;
        } else if(lb > 0.0) {
            ub = lb * (1.0 + Tigon::DistMinInterval);
        } else {
            lb = ub * (1.0 + Tigon::DistMinInterval);
        }
    }

    IDistribution::defineBoundaries(lb, ub);

    defineResolution(m_ub - m_lb);

    if(m_uniDist != nullptr) {
        delete m_uniDist;
        m_uniDist = nullptr;
    }

    m_uniDist = new boost::math::uniform_distribution<double>(m_lb, m_ub);
}


double UniformDistribution::sample()
{
    return TRAND.randUni(m_ub - m_lb, m_lb);
}

double UniformDistribution::mean()
{
    return (m_ub + m_lb) / 2.0;
}

double UniformDistribution::median()
{
    return boost::math::median(*m_uniDist);
}

double UniformDistribution::percentile(double p)
{
    if(p >= 1.0) {
        return m_ub;
    } else if(p <= 0.0) {
        return m_lb;
    }
    return boost::math::quantile(*m_uniDist, p);
}

double UniformDistribution::variance()
{
    return boost::math::variance(*m_uniDist);
}

double UniformDistribution::std()
{
    return boost::math::standard_deviation(*m_uniDist);
}

double UniformDistribution::pdf(double z)
{
    return boost::math::pdf(*m_uniDist, z);
}

double UniformDistribution::cdf(double z)
{
    return boost::math::cdf(*m_uniDist, z);
}

void UniformDistribution::negate()
{
    defineBoundaries(-m_ub, -m_lb);
}

void UniformDistribution::generatePDF()
{
    if(m_z.empty()) {
        generateZ();
    }

    double probability = 1.0/(m_ub - m_lb);
    m_pdf = TVector<double>(m_nSamples, probability);
}

TStringList UniformDistribution::parameterNames() const
{
    TStringList names;
    names << "Lower Bound" << "Upper Bound";
    return names;
}

TVector<double> UniformDistribution::parameters() const
{
    TVector<double> params;
    params << lowerBound() << upperBound();
    return params;
}

void UniformDistribution::defineParameters(const TVector<double> &parameters)
{
    if(parameters.size() > 1) {
        clearDistribution();
        double lb = parameters[0];
        double ub = parameters[1];
        defineBoundaries(lb, ub);
    }
}

} // namespace Representation
} // namespace Tigon
