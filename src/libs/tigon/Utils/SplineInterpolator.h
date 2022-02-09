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
#ifndef SPLINEINTERPOLATOR_H
#define SPLINEINTERPOLATOR_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/AbstractInterpolator.h>

namespace Tigon {

// Cubic spline interpolator
class LIGER_TIGON_EXPORT SplineInterpolator : public AbstractInterpolator
{
public:
    SplineInterpolator(const TVector<double>& xv, const TVector<double>& yv,
                       double yp1=1.e99, double ypn=1.e99);
    ~SplineInterpolator();

protected:
    void sety2(TVector<double> xv, TVector<double> yv, double yp1, double ypn);
    double baseInterpolate(int jl, double xv);
    bool checkConfiguration();

private:    
    TVector<double> y2;
};

} // namespace Tigon

#endif // SPLINEINTERPOLATOR_H
