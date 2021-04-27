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
#include <tigon/Representation/Distributions/NonParaDistribution.h>

namespace Tigon {
namespace Representation {

NonParaDistribution::NonParaDistribution()
{
    m_type = Tigon::NonParametricDistType;
}

NonParaDistribution::NonParaDistribution(const NonParaDistribution& dist)
    : IDistribution(dist)
{
    m_type = Tigon::NonParametricDistType;
}

NonParaDistribution::NonParaDistribution(double value)
{
    m_type = Tigon::NonParametricDistType;
    defineBoundaries(value,value);
}

NonParaDistribution::NonParaDistribution(const TVector<double> &pdf)
{
    m_type = Tigon::NonParametricDistType;
    definePDF(pdf);
}

NonParaDistribution::NonParaDistribution(const TVector<double> &z, const TVector<double> &pdf)
{
    m_type = Tigon::NonParametricDistType;
    defineZ(z);
    definePDF(pdf);
}

NonParaDistribution::~NonParaDistribution()
{

}

NonParaDistribution *NonParaDistribution::clone() const
{
    return (new NonParaDistribution(*this));
}

void NonParaDistribution::definePDF(const TVector<double> &pdf)
{
    if(pdf.size()>1) {
        // pdf must have the same size as m_z
        m_nSamples = pdf.size();
        if(m_z.size() != m_nSamples) {
            m_dz = (m_ub - m_lb) / (m_nSamples - 1);
            generateZ();
        }
        m_pdf = pdf;
        normalise();
    }
}

} // namespace Representation
} // namespace Tigon
