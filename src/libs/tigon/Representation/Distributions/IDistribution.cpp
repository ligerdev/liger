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
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/LinearInterpolator.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Representation {

IDistribution::IDistribution()
    : m_type(Tigon::GenericDistType),
      m_lb(0.0),
      m_ub(1.0),
      m_dz(1.0/(Tigon::DistMinNSamples - 1.0)),
      m_nSamples(0),
      m_quantileInterpolator(0),
      m_pdfInterpolator     (0),
      m_cdfInterpolator     (0)
{
}

IDistribution::IDistribution(const IDistribution& dist)
{
    m_type = dist.m_type;
    m_dz = dist.m_dz;
    m_lb = dist.m_lb;
    m_ub = dist.m_ub;
    m_z = dist.m_z;
    m_pdf = dist.m_pdf;
    m_cdf = dist.m_cdf;
    m_nSamples = dist.m_nSamples;
    m_pdfInterpolator      = 0;
    m_cdfInterpolator      = 0;
    m_quantileInterpolator = 0;
}

IDistribution::IDistribution(double value)
    : m_type(Tigon::GenericDistType),
      m_lb(0),
      m_ub(1),
      m_dz((m_ub-m_lb)/(Tigon::DistMinNSamples - 1)),
      m_nSamples(0),
      m_quantileInterpolator(0),
      m_pdfInterpolator     (0),
      m_cdfInterpolator     (0)
{
    defineBoundaries(value,value);
}

IDistribution::~IDistribution()
{
    delete m_pdfInterpolator;
    delete m_cdfInterpolator;
    delete m_quantileInterpolator;
}

IDistribution* IDistribution::clone() const
{
    return (new IDistribution(*this));
}

Tigon::DistributionType IDistribution::type() const
{
    return m_type;
}

TStringList IDistribution::parameterNames() const
{
    return TStringList();
}

TVector<double> IDistribution::parameters() const
{
    return TVector<double>();
}

void IDistribution::defineParameters(const TVector<double> &parameters)
{
    T_UNUSED(parameters);
}

void IDistribution::defineSamples(TVector<double> samples, TVector<double> weights)
{
    T_UNUSED(samples);
    T_UNUSED(weights);
}

double IDistribution::sample()
{
    if(m_cdf.empty() || m_cdf.size() != m_nSamples) {
        calculateCDF();
    }
    if(m_quantileInterpolator == nullptr) {
        m_quantileInterpolator = new LinearInterpolator(m_cdf, m_z);
    } else {
        m_quantileInterpolator->defineXY(m_cdf, m_z);
    }
    double r = TRAND.randUni();
    // A value between 0-1: 0==>lb , 1==>ub
    /// \attention Shaul: check this.
    /// \note Set to value of lowest sample if r is contained within probability mass of lowest sample
    double sample = m_lb;
    if(r>m_cdf[0]) {
        sample = m_quantileInterpolator->interpolate(r);
    }
    return sample;
}

double IDistribution::mean()
{
    if(m_pdf.empty()) {
        calculateCDF();
    }
    double sum  = 0.0;
    for(int i=0; i<m_nSamples-1; i++) {
        double cur  = m_pdf[i] * m_z[i];
        double next = m_pdf[i+1] * m_z[i+1];
        sum += (cur+next)/2 * (m_z[i+1] - m_z[i]);
    }
    return sum;
}

double IDistribution::median()
{
    return percentile(0.5);
}

double IDistribution::percentile(double p)
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
        m_quantileInterpolator = new LinearInterpolator(m_cdf, m_z);
    } else {
        m_quantileInterpolator->defineXY(m_cdf, m_z);
    }
    return m_quantileInterpolator->interpolate(p);
}

double IDistribution::variance()
{
    double m = mean();
    double sum  = 0.0;
    for(int i=0; i<m_nSamples-1; i++) {
        double cur  = m_pdf[i] * m_z[i]*m_z[i];
        double next = m_pdf[i+1] * m_z[i+1]*m_z[i+1];
        sum += (cur+next)/2 * (m_z[i+1] - m_z[i]);
    }
    return sum - m*m;
}

double IDistribution::std()
{
    return sqrt(variance());
}

TVector<double> IDistribution::pdf()
{
    if(m_pdf.empty() || m_pdf.size() != m_nSamples) {
        generatePDF();
    }
    return m_pdf;
}

TVector<double> IDistribution::cdf()
{
    if(m_cdf.empty() || m_cdf.size() != m_nSamples) {
        calculateCDF();
    }
    return m_cdf;
}

TVector<double> IDistribution::pdf(const TVector<double> zVec)
{
    TVector<double> ret(zVec.size());
    for(int i=0; i< zVec.size(); i++) {
        ret[i] = pdf(zVec[i]);
    }
    return ret;
}

TVector<double> IDistribution::cdf(const TVector<double> zVec)
{
    TVector<double> ret(zVec.size());
    for(int i=0; i< zVec.size(); i++) {
        ret[i] = cdf(zVec[i]);
    }
    return ret;
}

double IDistribution::pdf(double z)
{
    if(m_pdf.empty() || m_pdf.size() != m_nSamples) {
        generatePDF();
    }

    if(z < m_lb || z > m_ub) {
        return 0.0;
    }

    if(m_pdfInterpolator == nullptr) {
        m_pdfInterpolator = new LinearInterpolator(m_z, m_pdf);
    } else {
        m_pdfInterpolator->defineXY(m_z, m_pdf);
    }


    return m_pdfInterpolator->interpolate(z);
}

double IDistribution::cdf(double z)
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
            m_cdfInterpolator = new LinearInterpolator(m_z, m_cdf);
        } else {
            m_cdfInterpolator->defineXY(m_z, m_cdf);
        }
        return m_cdfInterpolator->interpolate(z);
    }
}

void IDistribution::defineResolution(double dz)
{
    if(dz > 0) {
        // make sure the range is a multiple of m_dz, and m_dz <= dz
        m_dz = (m_ub-m_lb) / ceil((m_ub-m_lb)/dz);
    }
}

double IDistribution::resolution() const
{
    return m_dz;
}

void IDistribution::defineBoundaries(double lb, double ub)
{
    if(lb >= ub) {
        if(lb == 0.0) {
            ub = Tigon::DistMinInterval;
        } else if(lb > 0.0) {
            ub = lb * (1.0 + Tigon::DistMinInterval);
        } else {
            lb = ub * (1.0 + Tigon::DistMinInterval);
        }
    }

    // TODO: test scaling

    double oldRange = m_ub - m_lb;
    double newRange = ub - lb;
    double ratio    = newRange / oldRange;

    m_lb = lb;
    m_ub = ub;

    defineResolution(m_dz * ratio);

    if(!m_z.empty()) {
        for(int i=0; i<m_nSamples; i++) {
            m_z[i] = lb + ratio * (m_z[i] - m_lb);
        }
    }

    if(!m_pdf.empty()) {
        normalise();
    }
}

double IDistribution::lowerBound() const
{
    return m_lb;
}

double IDistribution::upperBound() const
{
    return m_ub;
}

void IDistribution::defineZ(TVector<double> z)
{
    // sort the elements and remove the duplicates
    std::sort(z.begin(),z.end());
    z.erase(std::unique(z.begin(), z.end() ), z.end());

    if(m_pdf.size() != z.size()) {
        m_pdf.clear();
    }

    if(z.size() >= 2) {
        m_z = z;
        m_lb = m_z.front();
        m_ub = m_z.back();
        m_nSamples = m_z.size();
    } else {
        m_z.clear();
        m_nSamples = 0;
        defineBoundaries(z[0],z[0]);
    }
}

TVector<double> IDistribution::zSamples()
{
    if(m_z.empty()) {
        generateZ();
    }
    return m_z;
}

void IDistribution::generateZ()

{
    generateEquallySpacedZ();
}

void IDistribution::generatePDF()
{
    // uniform distribution
    double probability = 1.0/(m_ub - m_lb);
    fill(m_pdf, probability, m_nSamples);
}

void IDistribution::generateEquallySpacedZ()
{
    m_nSamples = (int)((m_ub-m_lb)/m_dz) + 1;
    m_z.resize(m_nSamples);
    double zz=m_lb;
    for(int i=0; i<m_z.size()-1; i++) {
        m_z[i] = zz;
        zz += m_dz;
    }
    m_z[m_z.size()-1] = m_ub;
}

void IDistribution::calculateCDF()
{
    if(m_pdf.empty()) {
        generatePDF();
    }
    fill(m_cdf, 0.0, m_nSamples);
    double cur  = 0.0;
    double next = 0.0;
    for(int i=0; i<m_nSamples-1; i++) {
        cur  = m_pdf[i];
        next = m_pdf[i+1];
        m_cdf[i+1] = m_cdf[i] + (cur+next)/2 * (m_z[i+1] - m_z[i]);
    }

    // normalise
    double factor = m_cdf.back();
    if(factor == 1.0) {
        return;
    } else if(factor == 0.0) {
        double probability = 1.0/(m_ub - m_lb);
        m_pdf = TVector<double>(m_nSamples, probability);
        calculateCDF();
    } else {
        for(int i=0; i<m_cdf.size(); i++) {
            m_pdf[i] /= factor;
            m_cdf[i] /= factor;
        }
    }
}

void IDistribution::normalise()
{
    if(m_pdf.empty()) {
        return;
    }

    calculateCDF();
}

void IDistribution::negate()
{
    double ub = m_ub;
    m_ub = -m_lb;
    m_lb = -ub;

    if(m_z.empty()) {
        return;
    }
    TVector<double> newZ(m_nSamples);
    for(int i=0; i<m_nSamples; i++) {
        newZ[i] = -m_z[m_nSamples-1-i];
    }
    m_z.swap(newZ);

    if(m_pdf.empty()) {
        return;
    }
    TVector<double> newPdf(m_nSamples);
    for(int i=0; i<m_nSamples; i++) {
        newPdf[i] = m_pdf[m_nSamples-1-i];
    }
    m_pdf.swap(newPdf);
    calculateCDF();
}

void IDistribution::add(double num)
{
    m_ub += num;
    m_lb += num;

    if(m_z.empty()) {
        return;
    }
    for(int i=0; i<m_nSamples; i++) {
        m_z[i] += num;
    }

    if(!m_pdf.empty()) {
        calculateCDF();
    }
}

void IDistribution::add(IDistribution* other)
{
    double lbO = other->lowerBound();
    double ubO = other->upperBound();
    double lb = m_lb + lbO;
    double ub = m_ub + ubO;
    double dz = std::max(m_ub-m_lb, ubO-lbO)/Tigon::DistConvNSamples;

    int nSampT = (int)((m_ub-m_lb)/dz) + 1;
    int nSampO = (int)((ubO-lbO)/dz) + 1;
    int nSamples = nSampT + nSampO - 1;

    // corrections to dz
    dz = (ub-lb)/(nSamples-1);
    double dzT = (m_ub-m_lb) / (nSampT-1);
    double dzO = (ubO - lbO) / (nSampO-1);

    // evenly distributed samples for convoluted distribution
    TVector<double> z(nSamples);
    double zz = lb;
    for(int i=0; i<nSamples-1; i++) {
        z[i] = zz;
        zz += dz;
    }
    z[nSamples-1] = ub;

    // and for original distributions
    TVector<double> pdfT(nSampT);
    zz = m_lb;
    for(int i=0; i<nSampT-1; i++) {
        pdfT[i] = pdf(zz);
        zz += dzT;
    }
    pdfT[nSampT-1] = pdf(m_ub);

    TVector<double> pdfO(nSampO);
    zz = lbO;
    for(int i=0; i<nSampO-1; i++) {
        pdfO[i] = other->pdf(zz);
        zz += dzO;
    }
    pdfO[nSampO-1] = other->pdf(ubO);

    // convolute the two pdfs
    m_pdf = conv(pdfT,pdfO);

    // update the distribution
    m_z.swap(z);
    m_ub = ub;
    m_lb = lb;
    m_dz = dz;
    m_nSamples = nSamples;

    normalise();
}

void IDistribution::subtract(double num)
{
    add(-num);
}

void IDistribution::subtract(const IDistribution* other)
{
    IDistribution* minusOther = other->clone();
    minusOther->negate();
    add(minusOther);
    delete minusOther;
}

void IDistribution::multiply(double num)
{
    if(num == 0)
    {
        m_lb = 0.0;
        m_ub = Tigon::DistMinInterval;
        m_dz = Tigon::DistMinInterval/(Tigon::DistMinNSamples-1);
        m_z.clear();
        m_z << m_lb << (m_lb+m_ub)/2 << m_ub;
        fill(m_pdf, 1.0, 2);
        normalise();
        return;
    }

    if(num < 0) {
        negate();
        num = -num;
    }

    m_ub *= num;
    m_lb *= num;

    if(m_z.empty()) {
        return;
    }
    for(int i=0; i<m_nSamples; i++) {
        m_z[i] *= num;
    }

    if(!m_pdf.empty()) {
        calculateCDF();
    }
}

void IDistribution::multiply(IDistribution* other)
{
    double lbO = other->lowerBound();
    double ubO = other->upperBound();
    double lb  = std::min(m_lb*lbO,std::min(m_lb*ubO,std::min(m_ub*lbO,m_ub*ubO)));
    double ub  = std::max(m_lb*lbO,std::max(m_lb*ubO,std::max(m_ub*lbO,m_ub*ubO)));

    int nSamples = Tigon::DistMultNSamples;
    double dz  = (ub-lb) / (nSamples-1);
    double dzT = (m_ub-m_lb) / (nSamples-1);

    // evenly distributed samples for joint multiplied distribution
    TVector<double> z(nSamples);
    double zz = lb;
    for(int i=0; i<nSamples-1; i++) {
        z[i] = zz;
        zz += dz;
    }
    z[nSamples-1] = ub;

    // and for original distributions
    TVector<double> zT(nSamples);
    zz = m_lb;
    for(int i=0; i<nSamples-1; i++) {
        zT[i] = zz;
        zz += dzT;
    }
    zT[nSamples-1] = m_ub;

    // multiply the two distributions
    TVector<double> newPDF(nSamples);
    for(int i=0; i<nSamples; i++) {
        for(int j=0; j<nSamples; j++) {
            double zt = zT[j];
            if(std::abs(zt) >= dzT/2) {
                double zo = z[i]/zt;
                if(zo >= lbO && zo <= ubO) {
                    newPDF[i] += pdf(zt) * other->pdf(zo) / std::abs(zt);
                }
            } else {
                zt = -dzT/2;
                double zo = z[i]/zt;
                if(zo >= lbO && zo <= ubO) {
                    newPDF[i] += pdf(zt) * other->pdf(zo) / std::abs(zt) / 2.0;
                }
                zt = dzT/2;
                zo = z[i]/zt;
                if(zo >= lbO && zo <= ubO) {
                    newPDF[i] += pdf(zt) * other->pdf(zo) / std::abs(zt) / 2.0;
                }
            }
        }
        newPDF[i] *= dzT;
    }

    // update the distribution
    m_pdf.swap(newPDF);
    m_z.swap(z);
    m_ub = ub;
    m_lb = lb;
    m_dz = dz;
    m_nSamples = nSamples;

    normalise();
}

void IDistribution::divide(double num)
{
    if(num == 0)
    {
        return;
    }

    multiply(1.0/num);
}

void IDistribution::divide(IDistribution* other)
{
    double lbO = other->lowerBound();
    double ubO = other->upperBound();
    double lb,ub;

    // Division by zero
    if((sgn(lbO) != sgn(ubO)) || (lbO == 0.0) || (ubO == 0.0)) {
        if((sgn(m_lb) != sgn(m_ub)) || (sgn(lbO) != sgn(ubO))){
            lb = Tigon::Lowest;
            ub =  Tigon::Highest;
        } else if(lbO == 0.0) {
            if(m_lb >= 0.0) {
                lb = 0.0;
                ub =  Tigon::Highest;
            } else {
                lb = Tigon::Lowest;
                ub =  0.0;
            }
        } else {
            if(m_lb >= 0.0) {
                lb = Tigon::Lowest;
                ub =  0.0;
            } else {
                lb = 0.0;
                ub =  Tigon::Highest;
            }
        }

        IDistribution::defineBoundaries(lb, ub);
        generateEquallySpacedZ();
        IDistribution::generatePDF();
        return;
    }

    lb  = std::min(m_lb/lbO,std::min(m_lb/ubO,std::min(m_ub/lbO,m_ub/ubO)));
    ub  = std::max(m_lb/lbO,std::max(m_lb/ubO,std::max(m_ub/lbO,m_ub/ubO)));


    int nSamples = Tigon::DistMultNSamples;
    double dz  = (ub-lb) / (nSamples-1);
    double dzO = (ubO-lbO) / (nSamples-1);

    // evenly distributed samples for joint divided distribution
    TVector<double> z(nSamples);
    double zz = lb;
    for(int i=0; i<nSamples-1; i++) {
        z[i] = zz;
        zz += dz;
    }
    z[nSamples-1] = ub;

    // and for original distribution
    TVector<double> zO(nSamples);
    zz = lbO;
    for(int i=0; i<nSamples-1; i++) {
        zO[i] = zz;
        zz += dzO;
    }
    zO[nSamples-1] = ubO;

    // divide the two distributions
    TVector<double> newPDF(nSamples);
    for(int i=0; i<nSamples; i++) {
        for(int j=0; j<nSamples; j++) {
            double zo = zO[j];
                double zt = z[i]*zo;
                if(zt >= m_lb && zt <= m_ub) {
                    newPDF[i] += other->pdf(zo) * pdf(zt) * std::abs(zo);
                }
        }
        newPDF[i] *= dzO;
    }

    // update the distribution
    m_pdf.swap(newPDF);
    m_z.swap(z);
    m_ub = ub;
    m_lb = lb;
    m_dz = dz;
    m_nSamples = nSamples;

    normalise();
}

void IDistribution::reciprocal()
{
    double lb,ub;

    // Division by zero
    if( (sgn(m_lb) != sgn(m_ub)) || (m_lb == 0.0) || (m_ub == 0.0) ) {
        if(sgn(m_lb) != sgn(m_ub)) {
            lb = Tigon::Lowest;
            ub =  Tigon::Highest;
        } else if(m_lb == 0.0) {
            lb = 0.0;
            ub =  Tigon::Highest;
        } else {
            lb = Tigon::Lowest;
            ub =  0.0;
        }

        IDistribution::defineBoundaries(lb, ub);
        generateEquallySpacedZ();
        IDistribution::generatePDF();
        return;
    }

    lb = 1.0 / m_ub;
    ub = 1.0 / m_lb;

    int nSamples = Tigon::DistMultNSamples;
    double dz  = (ub-lb) / (nSamples-1);

    // evenly distributed samples for reciprocal distribution
    TVector<double> z(nSamples);
    double zz = lb;
    for(int i=0; i<nSamples-1; i++) {
        z[i] = zz;
        zz += dz;
    }
    z[nSamples-1] = ub;

//    // and for original distribution
//    TVector<double> zT(nSamples);
//    zz = m_lb;
//    for(int i=0; i<nSamples-1; i++) {
//        zT[i] = zz;
//        zz += dzT;
//    }
//    zT[nSamples-1] = m_ub;

    // invert the distribution
    TVector<double> newPDF(nSamples);
    for(int i=0; i<nSamples; i++) {
        double zt = 1.0 / z[i];
        newPDF[i] = pdf(zt) * std::abs(zt*zt);
    }

    // update the distribution
    m_pdf.swap(newPDF);
    m_z.swap(z);
    m_ub = ub;
    m_lb = lb;
    m_dz = dz;
    m_nSamples = nSamples;

    normalise();
}

void IDistribution::clearDistribution()
{
    m_lb = 0.0;
    m_ub = 1.0;
    m_dz = 1.0 / (Tigon::DistMinNSamples - 1.0);
    m_nSamples = 0;
    m_z.clear();
    m_pdf.clear();
    m_cdf.clear();
    if(m_quantileInterpolator != nullptr) {
        delete m_quantileInterpolator;
        m_quantileInterpolator = nullptr;
    }
    if(m_pdfInterpolator != nullptr) {
        delete m_pdfInterpolator;
        m_pdfInterpolator = nullptr;
    }
    if(m_cdfInterpolator != nullptr) {
        delete m_cdfInterpolator;
        m_cdfInterpolator = nullptr;
    }
}

void IDistribution::defineType(const Tigon::DistributionType &type)
{
    m_type = type;
}

} // namespace Representation
} // namespace Tigon
