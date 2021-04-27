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
#include <tigon/Representation/Mappings/UncertaintyMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Distributions/UniformDistribution.h>
#include <tigon/Representation/Distributions/LinearDistribution.h>
#include <tigon/Representation/Distributions/PeakDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Representation/Distributions/ChiSquaredDistribution.h>

namespace Tigon {
namespace Representation {

UncertaintyMapping::UncertaintyMapping()
    : m_nParams(0)
{
    m_distType = GenericDistType;
}

UncertaintyMapping::UncertaintyMapping(const UncertaintyMapping& umap)
{
    m_distType   = umap.m_distType;
    m_nParams    = umap.m_nParams;
    m_constTerm  = umap.m_constTerm;
    m_linTerm    = umap.m_linTerm;
}

UncertaintyMapping::~UncertaintyMapping()
{

}

UncertaintyMapping* UncertaintyMapping::clone() const
{
    return new UncertaintyMapping(*this);
}

void UncertaintyMapping::defineDistributionType(DistributionType t)
{
    switch(t) {
    case PeakDistType:
        m_nParams = 4;
        fill(m_constTerm, 0.0, 4);
        m_linTerm.clear();
        break;

    case LinearDistType:
        m_nParams = 3;
        fill(m_constTerm, 0.0, 3);
        m_linTerm.clear();
        break;

    case UniformDistType:
    case NormalDistType:
    case BetaDistType:
    case GammaDistType:
    case BinomialDistType:
    case CauchyDistType:
    case DiscreteUniformDistType:
        m_nParams = 2;
        fill(m_constTerm, 0.0, 2);
        m_linTerm.clear();
        break;

    case ChiSquaredDistType:
    case ExponentialDistType:
    case PoissonDistType:
    case StudentTDistType:
    case BernoulliDistType:
        m_nParams = 1;
        fill(m_constTerm, 0.0, 1);
        m_linTerm.clear();
        break;

    case GenericDistType:
    case NonParametricDistType:
    case SampledDistType:
    case MergedDistType:
    default:
        // ERROR
        m_nParams = 0;
        m_constTerm.clear();
        m_linTerm.clear();
        break;
    }

    m_distType = t;
}

DistributionType UncertaintyMapping::distributionType() const
{
    return m_distType;
}

void UncertaintyMapping::defineConstTerms(TVector<double> constants)
{
    if(constants.size() == m_nParams) {
        m_constTerm = constants;
    } else {
        // ERROR
    }
}

void UncertaintyMapping::defineLinearTerms(TVector<double> lin)
{
    if(lin.size() == m_nParams) {
        m_linTerm = lin;
    } else {
        // ERROR
    }
}

TVector<double> UncertaintyMapping::constTerms() const
{
    return m_constTerm;
}

TVector<double> UncertaintyMapping::linearTerms() const
{
    return m_linTerm;
}

void UncertaintyMapping::evaluateUncertainty(IElementSPtr elem) const
{
    IDistribution* dist = nullptr;
    TVector<double> parameters = calculateParameters(elem);
    if(m_nParams == 0) {
        // ERROR
    } else if(m_nParams == 1) {
        switch(m_distType) {
        case ChiSquaredDistType:
            dist = new ChiSquaredDistribution(parameters);
            break;
        case ExponentialDistType:
            //TODO
            break;
        case PoissonDistType:
            // TODO
            break;
        case StudentTDistType:
            // TODO
            break;
        case BernoulliDistType:
            // TODO
            break;
        default:
            // ERROR
            break;
        }

    } else if(m_nParams == 2) {
        switch(m_distType) {
        case UniformDistType:
            dist = new UniformDistribution(parameters);
            break;
        case NormalDistType:
            dist = new NormalDistribution(parameters);
            break;
        case LinearDistType:
            dist = new LinearDistribution(parameters);
            break;
        case PeakDistType:
            dist = new PeakDistribution(parameters);
            break;
        case BetaDistType:
            // TODO
            break;
        case GammaDistType:
            // TODO
            break;
        case BinomialDistType:
            // TODO
            break;
        case CauchyDistType:
            // TODO
            break;
        case DiscreteUniformDistType:
            // TODO
            break;
        default:
            // ERROR
            break;
        }

    }

    elem->defineDist(dist);
}

// Private function
TVector<double> UncertaintyMapping::calculateParameters(IElementSPtr elem) const
{
    TVector<double> parameters = m_constTerm;
    if(!(m_linTerm.empty())) {
        double val = elem->value<double>();
        for(int i=0; i<m_nParams; i++) {
            parameters[i] += (val * m_linTerm[i]);
        }
//        if(!(m_quadTerm.empty())) {
//            for(int i=0; i<m_parameters.size(); i++) {
//                m_parameters[i] += (val * m_quadTerm[i]);
//            }
//        }
    }
    return parameters;
}

int UncertaintyMapping::nParams() const
{
    return m_nParams;
}

} // namespace Representation
} // namespace Tigon
