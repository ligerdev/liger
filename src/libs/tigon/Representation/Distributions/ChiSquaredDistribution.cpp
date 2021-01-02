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
#include <tigon/Representation/Distributions/ChiSquaredDistribution.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Representation {

ChiSquaredDistribution::ChiSquaredDistribution()
{
    m_type = Tigon::ChiSquaredDistType;
    m_dof = 1.0;
    m_chiSqDist = new boost::math::chi_squared_distribution<double>(m_dof);
    double ub = boost::math::quantile(*m_chiSqDist, 0.999);
    defineBoundaries(0.0, ub);
}

ChiSquaredDistribution::ChiSquaredDistribution(const ChiSquaredDistribution& dist)
    : IDistribution(dist)
{
    m_type = Tigon::ChiSquaredDistType;
    m_dof = dist.m_dof;
    m_chiSqDist = new boost::math::chi_squared_distribution<double>(m_dof);
}

ChiSquaredDistribution::ChiSquaredDistribution(double k)
{
    m_type = Tigon::ChiSquaredDistType;
    if(k>0) {
        m_dof = k;
    } else {
        // ERROR
        m_dof = 1.0;
    }
    m_chiSqDist = new boost::math::chi_squared_distribution<double>(m_dof);
    double ub = boost::math::quantile(*m_chiSqDist, 0.999);
    defineBoundaries(0.0, ub);
}

ChiSquaredDistribution::ChiSquaredDistribution(TVector<double> parameters)
{
    m_type = Tigon::ChiSquaredDistType;
    if((parameters.size() > 0) && (parameters[0] > 0.0)) {
        m_dof = parameters[0];
    } else {
        // ERROR
        m_dof = 1.0;
    }
    m_chiSqDist = new boost::math::chi_squared_distribution<double>(m_dof);
    double ub = boost::math::quantile(*m_chiSqDist, 0.999);
    defineBoundaries(0.0, ub);
}

ChiSquaredDistribution::~ChiSquaredDistribution()
{
    delete m_chiSqDist;
}

ChiSquaredDistribution* ChiSquaredDistribution::clone() const
{
    return (new ChiSquaredDistribution(*this));
}

void ChiSquaredDistribution::defineDOF(double k)
{
    if(k > 0.0) {
        m_dof = k;
    } else {
        // ERROR
    }
}

double ChiSquaredDistribution::sample()
{
    return TRAND.randChiSquared(m_dof);
}

double ChiSquaredDistribution::mean()
{
    return boost::math::mean(*m_chiSqDist);
}

double ChiSquaredDistribution::median()
{
    return boost::math::median(*m_chiSqDist);
}

double ChiSquaredDistribution::percentile(double p)
{
    if(p>=1.0) {
        p = 1.0;
    } else if(p<=0.0) {
        p = 0.0;
    }
    return boost::math::quantile(*m_chiSqDist, p);
}

double ChiSquaredDistribution::variance()
{
    return boost::math::variance(*m_chiSqDist);
}

double ChiSquaredDistribution::std()
{
    return boost::math::standard_deviation(*m_chiSqDist);
}

double ChiSquaredDistribution::pdf(double z)
{
    return boost::math::pdf(*m_chiSqDist, z);
}

double ChiSquaredDistribution::cdf(double z)
{
    return boost::math::cdf(*m_chiSqDist, z);
}

void ChiSquaredDistribution::negate()
{

}

void ChiSquaredDistribution::generatePDF()
{
    if(m_z.empty()) {
        generateZ();
    }

    m_pdf.resize(m_nSamples);
    for(int i=0; i<m_nSamples; i++) {
        m_pdf[i] = pdf(m_z[i]);
    }
}

TVector<double> ChiSquaredDistribution::parameters() const
{
    TVector<double> params;
    params << m_dof;
    return params;
}

} // namespace Representation
} // namespace Tigon
