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
#ifndef ABSTRACTINTERPOLATOR_H
#define ABSTRACTINTERPOLATOR_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {

class LIGER_TIGON_EXPORT AbstractInterpolator
{
public:
    explicit AbstractInterpolator(const TVector<double> &x,
                                  const TVector<double> &y, int m);
    virtual ~AbstractInterpolator();

    double interpolate(double xq);
    TVector<double> interpolate(const TVector<double> &xq);
    virtual void defineXY(const TVector<double> &x, const TVector<double> &y);
    bool isConfigured();

protected:
    int locate(const double x);
    int hunt(const double x);
    virtual double baseInterpolate(int jlo, double x) = 0;
    virtual bool checkConfiguration();

    int n;
    int mm;
    int jsav;
    int cor;
    int dj;
    bool m_isConfigured;
    TVector<double> xx;
    TVector<double> yy;
};

} // namespace Tigon

#endif // ABSTRACTINTERPOLATOR_H
