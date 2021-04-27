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
#ifndef NONPARADISTRIBUTION_H
#define NONPARADISTRIBUTION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Distributions/IDistribution.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT NonParaDistribution : public IDistribution
{
public:
    NonParaDistribution();
    NonParaDistribution(const NonParaDistribution& dist);
    NonParaDistribution(double sample);
    NonParaDistribution(const TVector<double> &pdf); //assumes even spaces in z = [0 1]
    NonParaDistribution(const TVector<double> &z, const TVector<double> &pdf);
    virtual ~NonParaDistribution();

    NonParaDistribution* clone() const;

    void  definePDF(const TVector<double> &pdf);

};

} // namespace Representation
} // namespace Tigon

#endif // NONPARADISTRIBUTION_H
