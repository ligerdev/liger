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
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Utils/ZeroOrderInterpolator.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

#include <cmath>

namespace Tigon {
namespace Representation {

SampledDistribution::SampledDistribution()
{
    m_type = SampledDistType;
    m_updated = false;
}

SampledDistribution::SampledDistribution(const SampledDistribution& dist)
    : IDistribution(dist)
{
    m_type = SampledDistType;
    m_samples = dist.m_samples;
    m_weights = dist.m_weights;
    m_mean = dist.m_mean;
    m_variance = dist.m_variance;
    m_updated = dist.m_updated;
}

SampledDistribution::SampledDistribution(TVector<double> samples,
                                         TVector<double> weights)
{
    m_type = SampledDistType;
    addSamples(samples, weights);
}

SampledDistribution::~SampledDistribution()
{

}

SampledDistribution* SampledDistribution::clone() const
{
    return (new SampledDistribution(*this));
}

double SampledDistribution::mean()
{
    if(!m_updated) {
        update();
    }
    return m_mean;
}

double SampledDistribution::variance()
{
    if(!m_updated) {
        update();
    }
    return m_variance;
}

TVector<double> SampledDistribution::sampledVec() const
{
    return m_samples;
}

double SampledDistribution::percentile(double p)
{
    if(m_cdf.empty() || m_cdf.size() != m_nSamples) {
        calculateCDF();
    }

    if(p>=1.0) {
        return m_ub;
    } else if(p<=0.0) {
        return m_lb;
    }

    if(m_quantileInterpolator == nullptr) {
        m_quantileInterpolator = new ZeroOrderInterpolator(cdf(), zSamples());
    } else {
        m_quantileInterpolator->defineXY(cdf(), zSamples());
    }
    return m_quantileInterpolator->interpolate(p);
}

double SampledDistribution::pdf(double z)
{
    int idx = vectorIndexOf(zSamples(), z);
    if(idx >= 0) {
        return pdf().at(idx);
    } else {
        return 0.0;
    }
}

double SampledDistribution::cdf(double z)
{
    if(m_cdf.empty() || m_cdf.size() != m_nSamples) {
        calculateCDF();
    }

    if(z <= m_lb) {
        return 0.0;
    } else if(z >= m_ub) {
        return 1.0;
    } else {
        if(m_cdfInterpolator == nullptr) {
            m_cdfInterpolator = new ZeroOrderInterpolator(zSamples(), cdf());
        } else {
            m_cdfInterpolator->defineXY(zSamples(), cdf());
        }
        return m_cdfInterpolator->interpolate(z);
    }
}

double SampledDistribution::sampledVal(int idx) const
{
    if((idx >= nSamples()) || (idx < 0)) {
        throw TException("IFunction::SampledDistribution", DomainException);
    }

    return m_samples[idx];
}

double SampledDistribution::weight(int idx) const
{
    if((idx >= nSamples()) || (idx < 0)) {
        throw TException("IFunction::SampledDistribution", DomainException);
    }

    return m_weights[idx];
}


TVector<double> SampledDistribution::weights() const
{
    return m_weights;
}

int SampledDistribution::nSamples() const
{
    return m_samples.size();
}

void SampledDistribution::defineSamples(TVector<double> samples, TVector<double> weights)
{
    clear();
    addSamples(samples, weights);
}

void SampledDistribution::addSample(double samp, double weight)
{
    clearDistribution();
    weight = std::max(weight, 0.0);
    int reSampIdx = vectorIndexOf(m_samples, samp);
    if(reSampIdx >= 0) {
        m_weights[reSampIdx] += weight;
    } else {
        m_samples.push_back(samp);
        m_weights.push_back(weight);
    }
    m_updated = false;
}

void SampledDistribution::addSamples(TVector<double> samples, TVector<double> weights)
{
    clearDistribution();
    if(samples.size() == weights.size()) {
        for(int i=0; i<samples.size(); i++) {
            addSample(samples[i], weights[i]);
        }
    } else if(weights.empty()) {
        for(int i=0; i<samples.size(); i++) {
            addSample(samples[i]);
        }
    } else {
        // ERROR -  different number of samples and weights
    }
}

void SampledDistribution::removeSample(int idx)
{
    clearDistribution();
    if(m_samples.size() > idx && idx >= 0) {
        m_samples.erase(m_samples.begin()+idx);
        m_weights.erase(m_samples.begin()+idx);
    }
}

void SampledDistribution::clear()
{
    clearDistribution();
    m_samples.clear();
    m_weights.clear();
    m_updated = false;
}

void SampledDistribution::negate()
{
    for(int i = 0; i < m_samples.size(); i++) {
        m_samples[i] = -m_samples[i];
    }
    IDistribution::negate();
}

void SampledDistribution::generateZ()
{
    clearDistribution();
    if(!(m_samples.empty())) {
        m_nSamples = m_samples.size();

        /// \todo: consider the declaration of ind
        /// as a class member, that is, m_ind.
        /// It is then possible to get rid of the
        /// vector samples and weights
        // sort the samples and weights
        TVector<int> ind = indSort(m_samples);
        TVector<double> samples(m_nSamples);
        TVector<double> weights(m_nSamples);
        for(int i=0; i<m_nSamples; i++) {
            samples[i] = m_samples[ind[i]];
            weights[i] = m_weights[ind[i]];
        }
        m_samples.swap(samples);
        m_weights.swap(weights);

        m_z = m_samples;
        m_lb = m_z[0];
        m_ub = m_z[m_nSamples-1];
        defineResolution((m_ub-m_lb)/m_nSamples);
    } else {
        // ERROR - no samples
        return;
    }
}

void SampledDistribution::generatePDF()
{
    if(m_samples.empty()) {
        // ERROR - no samples
        return;
    }
    if(m_z.empty() || (m_nSamples != m_samples.size())) {
        generateZ();
    }

    m_pdf = m_weights;
    toUnitVec(m_pdf, 1.0);
}

void SampledDistribution::calculateCDF()
{
    if(m_samples.empty()) {
        // ERROR - no samples
        return;
    }
    if(m_pdf.empty() || (m_nSamples != m_samples.size())) {
        generatePDF();
    }

    m_cdf = m_pdf;
    for(int i=1; i<m_nSamples;i++) {
        m_cdf[i] += m_cdf[i-1];
    }
}

void SampledDistribution::update()
{
    if(m_samples.empty()) {
        m_mean     = std::nan(0);
        m_variance = std::nan(0);
        return;
    }

    if(m_samples.size() == 1) {
        m_mean     = m_samples[0];
        m_variance = Tigon::Epsilon;
        m_updated  = true;
        return;
    }

    m_mean = 0.0;
    double totalWeights = 0.0;
    double tSquaredWeights = 0.0;
    // Calculate mean
    for(int i=0; i<m_samples.size(); i++) {
        m_mean += m_weights[i] * m_samples[i];
        totalWeights += m_weights[i];
        tSquaredWeights += m_weights[i] * m_weights[i];
    }
    m_mean /= totalWeights;

    // Calculate unbiased variance (from Wikipedia)
    m_variance  = 0.0;
    for(int i=0; i<m_samples.size(); i++) {
        m_variance += m_weights[i] * std::pow(m_samples[i] - m_mean, 2.0);
    }
    m_variance /= (totalWeights - tSquaredWeights / totalWeights);

    m_updated = true;
}

} // namespace Representation
} // namespace Tigon
