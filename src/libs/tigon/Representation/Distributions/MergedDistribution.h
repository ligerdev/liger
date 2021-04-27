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
#ifndef MERGEDDISTRIBUTION_H
#define MERGEDDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Distributions/IDistribution.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT MergedDistribution : public IDistribution
{
public:
    MergedDistribution();
    MergedDistribution(const MergedDistribution& dist);
    ~MergedDistribution();

    MergedDistribution* clone() const;

    void generateZ();
    void generatePDF();

    void appendDistribution(IDistribution* d);
    void appendDistribution(IDistribution* d, double ratio);

    void removeDistribution(IDistribution* d);
    void removeDistribution(int          idx);

    void changeRatio(IDistribution* d, double newRatio);
    void changeRatio(int          idx, double newRatio);

    void negate();

private:
    TVector<IDistribution*>    m_distributions;
    TVector<double>             m_ratios;

    void addZSamplesOfOneDistribution(IDistribution* d);
    void addOnePDF(IDistribution* d,       double ratio);
};

} // namespace Representation
} // namespace Tigon

#endif // MERGEDDISTRIBUTION_H
