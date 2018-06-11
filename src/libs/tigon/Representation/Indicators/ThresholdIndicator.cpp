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
#include <tigon/Representation/Indicators/ThresholdIndicator.h>
#include <tigon/Representation/Distributions/IDistribution.h>

namespace Tigon {
namespace Representation {

ThresholdIndicator::ThresholdIndicator()
{
    defineThreshold(0.0);
}

ThresholdIndicator::ThresholdIndicator(double q)
{
    defineThreshold(q);
}

ThresholdIndicator::~ThresholdIndicator()
{

}

double ThresholdIndicator::evaluate(IDistribution* dist) const
{
    return dist->cdf(m_threshold);
}

void ThresholdIndicator::defineThreshold(double q)
{
    m_threshold = q;
}

double ThresholdIndicator::threshold() const
{
    return m_threshold;
}


} // namespace Representation
} // namespace Tigon
