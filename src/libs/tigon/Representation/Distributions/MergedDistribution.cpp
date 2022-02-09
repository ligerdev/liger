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
#include <tigon/Representation/Distributions/MergedDistribution.h>
#include <tigon/Utils/SplineInterpolator.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Representation {

MergedDistribution::MergedDistribution()
{
    m_type = Tigon::MergedDistType;
}

MergedDistribution::MergedDistribution(const MergedDistribution& dist)
    : IDistribution(dist)
{
    m_type = Tigon::MergedDistType;
    int n = dist.m_distributions.size();
    m_distributions.resize(n);
    m_ratios.resize(n);
    for(int i=0; i<m_distributions.size(); i++) {
        m_distributions[i] = dist.m_distributions[i]->clone();
        m_ratios[i] = dist.m_ratios[i];
    }

}

MergedDistribution::~MergedDistribution()
{
    clearPointerVector(m_distributions);
}

MergedDistribution* MergedDistribution::clone() const
{
    return (new MergedDistribution(*this));
}

void MergedDistribution::generateZ()
{
    int nDistributions = m_distributions.size();

    if(nDistributions == 0) {
        return;
    }

    for(int i=0; i<nDistributions; i++) {
        addZSamplesOfOneDistribution(m_distributions[i]);
    }
}

void MergedDistribution::generatePDF()
{
    int nDistributions = m_distributions.size();

    if(nDistributions == 0) {
        return;
    }

    generateZ();

    m_pdf = TVector<double>(m_nSamples, 0.0);

    for(int i=0; i<nDistributions; i++) {
        addOnePDF(m_distributions[i], m_ratios[i]);
    }
    normalise();
}

void MergedDistribution::appendDistribution(IDistribution* d)
{
    clearDistribution();
    appendDistribution(d, 1.0);
}

void MergedDistribution::appendDistribution(IDistribution* d, double ratio)
{
    clearDistribution();
    m_distributions.push_back(d);
    m_ratios.push_back(std::max(ratio, 0.0));
}

void MergedDistribution::removeDistribution(IDistribution* d)
{
    int idx = vectorIndexOf(m_distributions, d);
    if(idx >= 0) {
        clearDistribution();
        removeDistribution(idx);
    }
}

void MergedDistribution::removeDistribution(int idx)
{
    if(isInRange(idx, m_distributions.size())) {
        clearDistribution();
        IDistribution* d = m_distributions[idx];
        delete d;
        m_distributions.erase(m_distributions.begin()+idx);
        m_ratios.erase(m_ratios.begin()+idx);
    }
}

void MergedDistribution::changeRatio(IDistribution* d, double newRatio)
{
    int idx = vectorIndexOf(m_distributions, d);
    if(idx >= 0) {
        changeRatio(idx, newRatio);
    }
}

void MergedDistribution::changeRatio(int idx, double newRatio)
{
    if(isInRange(idx, m_ratios.size())) {
        clearDistribution();
        m_ratios[idx] = std::max(newRatio, 0.0);
    }
}

void MergedDistribution::negate()
{
    clearDistribution();
    for(int i = 0; i < m_distributions.size(); i++) {
        m_distributions[i]->negate();
    }
}

void MergedDistribution::addZSamplesOfOneDistribution(IDistribution* d)
{
    TVector<double> newZ = d->zSamples();
    // merge newZ and m_z into augZ
    TVector<double> augZ;
    int iNew = 0; // iterator for newZ
    int iAug = 0; // iterator for m_z

    //TODO: avoid adding samples from lower frequency distributions
    //      to regions with higher freqwency samples
    while(iNew < newZ.size() && iAug < m_nSamples) {
        if(newZ[iNew] == m_z[iAug]) {
            augZ << m_z[iAug++];
            iNew++;
        } else if(newZ[iNew] < m_z[iAug]) {
            augZ << newZ[iNew++];
        } else {
            augZ << m_z[iAug++];
        }
    }

    while(iNew < newZ.size()) {
        augZ << newZ[iNew++];
    }

    while(iAug < m_nSamples) {
        augZ << m_z[iAug++];
    }

    m_z.swap(augZ);
    m_lb = m_z.front();
    m_ub = m_z.back();
    m_nSamples = m_z.size();
}

void MergedDistribution::addOnePDF(IDistribution* d, double ratio)
{
    // call this function only after the samples are integrated into m_z
    TVector<double> newPDF = d->pdf();
    TVector<double> newZ   = d->zSamples();

    // find the range of the new pdf
    int firstIdx = 0;
    int lastIdx  = m_nSamples - 1;

    while(newZ.front() > m_z[firstIdx]) {
        ++firstIdx;
    }
    while(newZ.back() < m_z[lastIdx]) {
        --lastIdx;
    }

    int nNewSamples = lastIdx - firstIdx + 1;

    SplineInterpolator* pdfInterpolator = new SplineInterpolator(newZ, newPDF);
    newPDF = pdfInterpolator->interpolate(mid(m_z, firstIdx, nNewSamples));

    for(int i=0; i<nNewSamples; i++) {
        m_pdf[firstIdx+i] += (newPDF[i] * ratio);
    }
}

} // namespace Representation
} // namespace Tigon
