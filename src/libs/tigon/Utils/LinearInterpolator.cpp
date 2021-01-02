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
#include <tigon/Utils/LinearInterpolator.h>

namespace Tigon {

LinearInterpolator::LinearInterpolator(const TVector<double> &xv, const TVector<double> &yv)
    : AbstractInterpolator(xv,yv,2)
{
    m_isConfigured = AbstractInterpolator::isConfigured();
}

LinearInterpolator::~LinearInterpolator()
{

}

double LinearInterpolator::baseInterpolate(int j, double x)
{
    if (xx[j]==xx[j+1]) {
        return yy[j];
    }

    return yy[j] + ((x-xx[j])/(xx[j+1]-xx[j]))*(yy[j+1]-yy[j]);
}

} // namespace Tigon

