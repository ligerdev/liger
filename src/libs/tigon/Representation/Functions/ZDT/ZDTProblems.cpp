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

#include <tigon/Representation/Functions/ZDT/ZDTProblems.h>
#include <boost/math/constants/constants.hpp>

using namespace boost::math::constants;
using namespace Tigon;

namespace ZDT {

TVector<double> ZDT1(const TVector<double>& x)
{
    TVector<double> y(2);

    y[0] = x[0];

    int n = x.size();
    double g = 0.0;
    for(int i=1; i<n; i++) {
        g += x[i];
    }
    g = 9.0 / ((double)(n-1)) * g + 1.0;

    double h = 1.0 - std::sqrt(y[0]/g);

    y[1] = g * h;
  
    return y;
}

TVector<double> ZDT2(const TVector<double>& x)
{
    TVector<double> y(2);

    y[0] = x[0];

    int n = x.size();
    double g = 0.0;
    for(int i=1; i<n; i++) {
        g += x[i];
    }
    g = 9.0 / ((double)(n-1)) * g + 1.0;

    double h = 1.0 - std::pow(y[0]/g,2.0);

    y[1] = g * h;

    return y;
}

}  // namespace ZDT
