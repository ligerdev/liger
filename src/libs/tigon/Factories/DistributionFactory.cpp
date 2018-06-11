/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Factories/DistributionFactory.h>
#include <tigon/Representation/Distributions/IDistribution.h>

#include <tigon/Representation/Distributions/LinearDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Representation/Distributions/PeakDistribution.h>
#include <tigon/Representation/Distributions/UniformDistribution.h>

#include <tigon/Representation/Distributions/SampledDistribution.h>

namespace Tigon {
namespace Representation {

DistributionFactory::DistributionFactory()
{

}

DistributionFactory *DistributionFactory::instance()
{
    static DistributionFactory factory;
    return &factory;
}

IDistribution *DistributionFactory::createParametricDist(DistributionType type,
                                                     const TVector<double>& parameters)
{
    IDistribution* dist = nullptr;
    switch (type) {
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
    default:
        break;
    }
    return dist;
}

IDistribution *DistributionFactory::createNonParametricDist(DistributionType type,
                                                            const TVector<double> samples,
                                                            const TVector<double> weight)
{
    ///\todo handle more distributions
    IDistribution* dist = nullptr;
    switch (type) {
    case SampledDistType:
        dist = new SampledDistribution(samples, weight);
        break;
    default:
        break;
    }
    return dist;
}

/// \todo Find an efficient way to return names
TStringList DistributionFactory::parameaterNames(DistributionType type)
{
    TStringList params;
    switch (type) {
    case UniformDistType: {
        UniformDistribution dist;
        params = dist.parameterNames();
    }
        break;
    case NormalDistType: {
        NormalDistribution dist;
        params = dist.parameterNames();
    }
        break;
    case LinearDistType: {
        LinearDistribution dist;
        params = dist.parameterNames();
    }
        break;
    case PeakDistType: {
        PeakDistribution dist;
        params = dist.parameterNames();
    }
        break;
    default:
        break;
    }
    return params;
}

}
}
