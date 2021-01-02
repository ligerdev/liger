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
#ifndef RBFINTERPOLATORCASCADE_H
#define RBFINTERPOLATORCASCADE_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
class RBFBasis;
class RBFInterpolator;

class LIGER_TIGON_EXPORT RBFInterpolatorCascade
{
public:
    RBFInterpolatorCascade();
    RBFInterpolatorCascade(const TVector<TVector<double> > &XX,
                           const TVector<TVector<double> > &YY,
                           TString basis, double scale = 1.0,
                           bool toNormalise=false);
    ~RBFInterpolatorCascade();

    TVector<double> interpolate(const TVector<double> &x);
    void append(RBFInterpolator* interpolator);
    void clear();

private:
    int m_xDim;
    TVector<RBFInterpolator*> m_interpolators;
};

} // namespace Tigon

#endif // RBFINTERPOLATORCASCADE_H
