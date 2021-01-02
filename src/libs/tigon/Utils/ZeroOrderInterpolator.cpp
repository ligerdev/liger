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
#include <tigon/Utils/ZeroOrderInterpolator.h>

namespace Tigon {

ZeroOrderInterpolator::ZeroOrderInterpolator(TVector<double> xv, TVector<double> yv)
    : AbstractInterpolator(xv,yv,2)
{

}

ZeroOrderInterpolator::~ZeroOrderInterpolator()
{

}

double ZeroOrderInterpolator::baseInterpolate(int j, double x)
{
    T_UNUSED(x);
    return yy[j];
//    if (xx[j]==xx[j+1]) return yy[j];
//    else return yy[j] + ((x-xx[j])/(xx[j+1]-xx[j]))*(yy[j+1]-yy[j]);
}

} // namespace Tigon

