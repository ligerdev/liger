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
#include <tigon/Representation/Indicators/ConfidenceIndicator.h>
#include <tigon/Representation/Distributions/IDistribution.h>


namespace Tigon {
namespace Representation {

ConfidenceIndicator::ConfidenceIndicator()
{
    defineConfidence(0.5);
}

ConfidenceIndicator::ConfidenceIndicator(double c)
{
    defineConfidence(c);
}

ConfidenceIndicator::~ConfidenceIndicator()
{

}

double ConfidenceIndicator::evaluate(IDistribution* dist) const
{
    return dist->percentile(m_confidence);
}

void ConfidenceIndicator::defineConfidence(double c)
{
    c = std::max(c, 0.0);
    c = std::min(c, 1.0);
    m_confidence = c;
}

double ConfidenceIndicator::confidence() const
{
    return m_confidence;
}


} // namespace Representation
} // namespace Tigon
