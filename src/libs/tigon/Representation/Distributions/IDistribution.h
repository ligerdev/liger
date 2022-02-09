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
#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
class AbstractInterpolator;
}

namespace Tigon {
namespace Representation {
/*!
 * \brief The IDistribution class
 * \todo This class and its derived classes need refactored.
 */
class LIGER_TIGON_EXPORT IDistribution
{
public:
    IDistribution();
    IDistribution(const IDistribution& dist);
    IDistribution(double sample);
    virtual ~IDistribution();

    virtual IDistribution* clone() const;

    Tigon::DistributionType type() const;

    virtual TStringList parameterNames() const;
    virtual TVector<double> parameters() const;
    virtual void defineParameters(const TVector<double>& parameters);


    /// \note this is the interface added for sample distribution.
    /// Temporary solution.
    /// \todo Re-struct IDistribution to accommodate the difference between
    /// parametric and non-parametric distributions
    virtual void defineSamples(TVector<double> samples,
                            TVector<double> weights = TVector<double>());
    virtual TVector<double> sampledVec() const {return TVector<double>();}
    virtual TVector<double> weights()    const {return TVector<double>();}

    virtual double sample();
    virtual double mean();
    virtual double variance();
    virtual double median();
    virtual double percentile(double p);
    virtual double std();

    TVector<double>         pdf();
    TVector<double>         cdf();
    virtual TVector<double> pdf(const TVector<double> zVec);
    virtual TVector<double> cdf(const TVector<double> zVec);
    virtual double          pdf(double                   z);
    virtual double          cdf(double                   z);

    virtual void   defineBoundaries(double lb, double ub);
    void           defineResolution(double           dz);
    double          resolution()                   const;
    double          lowerBound()                   const;
    double          upperBound()                   const;
    void           defineZ(TVector<double>            z);
    TVector<double> zSamples();

    virtual void generateZ();
    virtual void generatePDF();
    virtual void calculateCDF();
    void generateEquallySpacedZ();
    void normalise();

    // basic math operations
    virtual void negate();
    virtual void add(double num);
    virtual void add(IDistribution* other);
    virtual void subtract(double num);
    virtual void subtract(const IDistribution* other);
    virtual void multiply(double num);
    virtual void multiply(IDistribution* other);
    virtual void divide(double num);
    virtual void divide(IDistribution* other);
    void reciprocal();

    void defineType(const Tigon::DistributionType &type);

protected:
    void clearDistribution();
    Tigon::DistributionType   m_type;
    double                    m_lb;
    double                    m_ub;
    double                    m_dz;
    TVector<double>           m_z;
    TVector<double>           m_pdf;
    TVector<double>           m_cdf;
    int                      m_nSamples;
    AbstractInterpolator*    m_quantileInterpolator;
    AbstractInterpolator*    m_pdfInterpolator;
    AbstractInterpolator*    m_cdfInterpolator;
};
} // namespace Representation
} // namespace Tigon

#endif // IDISTRIBUTION_H
