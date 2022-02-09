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
#include <tigon/Representation/Distributions/PeakDistribution.h>
#include <complex>
#include <algorithm>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/math/constants/constants.hpp>

namespace Tigon {
namespace Representation {

PeakDistribution::PeakDistribution()
    : m_scale (1.0)
    , m_bias  (0.0)
{
    defineType(Tigon::PeakDistType);
    defineTendencyAndLocality(0.5, 1.0);
    generatePDF();
}

PeakDistribution::PeakDistribution(double tendency, double locality,
                                   double scale,    double bias)
    : m_scale (scale)
    , m_bias  (bias)
{
    defineType(Tigon::PeakDistType);
    defineTendencyAndLocality(tendency, locality);
    generatePDF();
}

PeakDistribution::PeakDistribution(const TVector<double> &parameters)
{
    defineType(Tigon::PeakDistType);

    double tendency = 0.5;
    double locality = 1.0;
    if(parameters.size() > 0) {
        tendency = parameters[0];
    }
    if(parameters.size() > 1) {
        locality = parameters[1];
    }
    defineTendencyAndLocality(tendency, locality);

    if((parameters.size() > 2) && (parameters[2] > 0.0)) {
        m_scale = parameters[2];
    } else {
        m_scale = 1.0;
    }
    if(parameters.size() > 3) {
        m_bias = parameters[3];
    } else {
        m_bias = 0.0;
    }
    generatePDF();
}

PeakDistribution::~PeakDistribution()
{

}

PeakDistribution* PeakDistribution::clone() const
{
    return (new PeakDistribution(*this));
}

void PeakDistribution::defineTendencyAndLocality(double tendency, double locality)
{
    if(tendency < 0.0) {
        tendency = 0.0;
    } else if(tendency > 1.0) {
        tendency = 1.0;
    }
    m_tendency = tendency;

    if(locality < 0.0) {
        locality = 0.0;
    } else if(locality > 1.0) {
        locality = 1.0;
    }

    m_locality = locality;

    //TODO: define high resolution at the peak and low at the rest
    defineResolution(1.0/(m_locality+0.1)/(Tigon::DistNSamples-1));
}

double PeakDistribution::tendency() const
{
    return m_tendency;
}

double PeakDistribution::locality() const
{
    return m_locality;
}

void PeakDistribution::negate()
{
    double newBias     = -(m_bias + m_scale);
    double newTendency = 1.0 - m_tendency;
    TVector<double> params;
    params << newTendency << m_locality << m_scale << newBias;
    defineParameters(params);
}

void PeakDistribution::generatePDF()
{
    using namespace std;

    if(m_z.empty()) {
        generateZ();
    }

    m_pdf = TVector<double>(m_nSamples);

    double shift = boost::math::constants::pi<double>() * m_tendency;
    double N = Tigon::DistPeakMinN + m_locality
            * (Tigon::DistPeakMaxN - Tigon::DistPeakMinN);
    TVector<TComplex> psiN(m_nSamples,TComplex(0,0));
    double nMax = std::max(3*N, Tigon::DistPeakMinNBasisFunc);
    nMax = std::min(nMax, Tigon::DistPeakMaxNBasisFunc);
    for(double n=1.0; n<=nMax; n++) {
        double cNn = sqrt( (std::pow(N, n) * exp(-N) / boost::math::factorial<double>(n)));
        TVector<double> psi = eigenFunction(n);
        for(int i=0; i<m_nSamples; i++) {
            TComplex j(0, 1);
            psiN[i] += cNn * exp(-j*shift*(n+0.5)) * psi[i];
        }
    }
    for(int i=0; i<m_nSamples; i++) {
        m_pdf[i] = real(conj(psiN[i]) * psiN[i]);
    }

    if(m_scale != 1.0) {
        multiply(m_scale);
    }
    if(m_bias != 0.0) {
        add(m_bias);
    }
}

TStringList PeakDistribution::parameterNames() const
{
    TStringList names;
    names << "Tendency" << "Locality" << "Scale" << "Bias";
    return names;
}

TVector<double> PeakDistribution::parameters() const
{
    TVector<double> params;
    params << m_tendency << m_locality << m_scale << m_bias;
    return params;
}

void PeakDistribution::defineParameters(const TVector<double> &parameters)
{
    if(parameters.size() > 1) {
        clearDistribution();
        defineTendencyAndLocality(parameters[0], parameters[1]);

        if((parameters.size() > 2) && (parameters[2] > 0.0)) {
            m_scale = parameters[2];
        } else {
            m_scale = 1.0;
        }
        if(parameters.size() > 3) {
            m_bias = parameters[3];
        } else {
            m_bias = 0.0;
        }
        generatePDF();
    }
}

TVector<double> PeakDistribution::eigenFunction(int n)
{
    double Lz = m_ub - m_lb;
    double An = std::pow(2.0/Lz, 0.5);

    TVector<double> psi(m_nSamples, 0);
    for(int i=1; i<m_nSamples-1; i++) {
        psi[i] = An*sin(boost::math::constants::pi<double>()*n*(m_z[i]-m_lb)/Lz);
    }
    return psi;
}

} // namespace Representation
} // namespace Tigon
