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
#ifndef RBFINTERPOLATOR_H
#define RBFINTERPOLATOR_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
class RBFBasis;

class LIGER_TIGON_EXPORT RBFInterpolator
{
public:
    RBFInterpolator(const TVector<TVector<double> > &XX, const TVector<double> &yy,
                    RBFBasis* basis, bool toNormalise=false);
    ~RBFInterpolator();

    double interpolate(const TVector<double> &pt);
    double l2dist(const TVector<double> &p1, const TVector<double> &p2);

private:
    bool m_toNormalise;
    int  m_xDim;
    int  m_N;
    TVector<TVector<double> > m_x;
    TVector<double>           m_y;
    TVector<double> m_w;
    RBFBasis*       m_basis;
};

} // namespace Tigon

#endif // RBFINTERPOLATOR_H
