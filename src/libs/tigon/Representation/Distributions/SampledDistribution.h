/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef SAMPLEDDISTRIBUTION_H
#define SAMPLEDDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Distributions/IDistribution.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT SampledDistribution : public IDistribution
{
public:
    SampledDistribution();
    SampledDistribution(const SampledDistribution& dist);
    SampledDistribution(TVector<double> samples,
                        TVector<double> weights = TVector<double>());
    ~SampledDistribution();

    SampledDistribution* clone() const;

    double mean();
    double variance();
    double percentile(double p);

    using IDistribution::pdf;
    using IDistribution::cdf;
    double pdf(double z);
    double cdf(double z);

    double   sampledVal(int idx) const;
    double   weight(int idx)     const;
    TVector<double> sampledVec() const;
    TVector<double> weights()    const;

    int nSamples() const;

    void defineSamples(TVector<double> samples,
                       TVector<double> weights = TVector<double>());
    void addSamples(TVector<double> samples,
                    TVector<double> weights = TVector<double>());
    void addSample(double samp, double weight = 1.0);
    void removeSample(int idx);
    void clear();

    void negate();

    void generateZ();
    void generatePDF();
    void calculateCDF();

private:
    void update();

    TVector<double> m_samples;
    TVector<double> m_weights;
    double          m_mean;
    double          m_variance;
    bool           m_updated;
};

} // namespace Representation
} // namespace Tigon

#endif // SAMPLEDDISTRIBUTION_H
