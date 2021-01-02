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
#ifndef PEAKDISTRIBUTION_H
#define PEAKDISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Distributions/IDistribution.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT PeakDistribution : public IDistribution
{
public:
    PeakDistribution();
    PeakDistribution(double tendency, double locality,
                     double scale = 1.0, double bias = 0.0);
    PeakDistribution(const TVector<double> &parameters);
    ~PeakDistribution();

    PeakDistribution* clone() const;

    double tendency()  const;
    double locality()  const;

    void negate();

    void generatePDF();

    TStringList parameterNames() const;
    TVector<double> parameters() const;
    void defineParameters(const TVector<double> &parameters);

private:
    void  defineTendencyAndLocality(double tendency, double locality);
    double m_tendency;
    double m_locality;
    double m_scale;
    double m_bias;

    TVector<double> eigenFunction(int n);
};

} // namespace Representation
} // namespace Tigon

#endif // PEAKDISTRIBUTION_H
