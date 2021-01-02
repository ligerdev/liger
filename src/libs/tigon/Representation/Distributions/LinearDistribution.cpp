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
#include <tigon/Representation/Distributions/LinearDistribution.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Representation {

LinearDistribution::LinearDistribution()
{
    m_type = Tigon::LinearDistType;
    defineResolution((m_ub-m_lb)/(Tigon::DistNSamples-1));
    m_ascend = true;
}

LinearDistribution::LinearDistribution(const LinearDistribution& dist)
    : IDistribution(dist)
{
    m_type = Tigon::LinearDistType;
    m_ascend = dist.m_ascend;
}

LinearDistribution::LinearDistribution(double lb, double ub, bool ascend)
{
    m_type = Tigon::LinearDistType;
    defineBoundaries(lb, ub);
    defineResolution((m_ub-m_lb)/(Tigon::DistNSamples-1));
    m_ascend = ascend;
}

LinearDistribution::LinearDistribution(TVector<double> parameters)
{
    m_type = Tigon::LinearDistType;
    double lb = 0.0;
    double ub = 1.0;
    m_ascend = true;
    if(parameters.size() > 0) {
        lb = parameters[0];
        if((parameters.size() > 1) && (parameters[1] > lb)) {
            ub = parameters[1];
            if((parameters.size() > 2) && (parameters[2] < 0.0)) {
                m_ascend =  false;
            }
        } else {
            ub = lb + Tigon::DistMinInterval;
        }
    }
    defineBoundaries(lb, ub);
    defineResolution((m_ub-m_lb)/(Tigon::DistNSamples-1));
}

LinearDistribution::~LinearDistribution()
{

}

LinearDistribution* LinearDistribution::clone() const
{
    return (new LinearDistribution(*this));
}

double LinearDistribution::sample()
{
    double r = TRAND.randUni();
    double samp;
    if(m_ascend) {
        samp = m_lb + sqrt(r)*(m_ub-m_lb);
    } else {
        samp = m_ub - sqrt(1-r)*(m_ub-m_lb);
    }
    return samp;
}

void LinearDistribution::generatePDF()
{
    if(m_z.empty()) {
        generateZ();
    }

    double maxProbability = 2/(m_ub - m_lb);
    m_pdf = TVector<double>(m_nSamples);
    if(isAscend()) {
        for(int i=0; i<m_nSamples; i++) {
            m_pdf[i] = maxProbability * (m_z[i] - m_lb) / (m_ub - m_lb);
        }
    } else {
        for(int i=0; i<m_nSamples; i++) {
            m_pdf[i] = maxProbability * (m_ub - m_z[i]) / (m_ub - m_lb);
        }
    }
}

bool LinearDistribution::isAscend() const
{
    return m_ascend;
}

void LinearDistribution::defineAscend(bool a)
{
    bool oldDir = m_ascend;
    m_ascend = a;
    if(m_ascend != oldDir && !m_pdf.empty()) {
        generatePDF();
    }
}

void LinearDistribution::negate()
{
    TVector<double> params;
    params << -m_ub << -m_lb << 0.5 - (double)m_ascend;
    defineParameters(params);
}

TStringList LinearDistribution::parameterNames() const
{
    TStringList names;
    names << "Lower Bound" << "Upper Bound" << "Is Ascend";
    return names;
}

TVector<double> LinearDistribution::parameters() const
{
    TVector<double> params;
    params << lowerBound() << upperBound() << (double)m_ascend - 0.5;
    return params;
}

void LinearDistribution::defineParameters(const TVector<double> &parameters)
{
    if(parameters.size() > 2) {
        clearDistribution();
        double lb = parameters[0];
        double ub = parameters[1];
        if(ub > lb) {
            defineBoundaries(lb, ub);
            defineResolution((m_ub-m_lb)/(Tigon::DistNSamples-1));
            if(parameters[2] < 0.0) {
                m_ascend =  false;
            } else {
                m_ascend = true;
            }
        }
    }
}

} // namespace Representation
} // namespace Tigon
