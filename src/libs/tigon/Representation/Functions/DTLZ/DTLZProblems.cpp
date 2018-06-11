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

#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>



#include <boost/math/constants/constants.hpp>

//namespace Tigon {
//namespace Operators {
using namespace boost::math::constants;
using namespace Tigon;
namespace DTLZ {

TVector<double> DTLZ1Modified(const TVector<double> &x, int M)
{
    int n = x.size();
    int k = n - M + 1;
    double g = 0.0;
    for (int i = n - k; i < n; i++) {
        g += (x[i] - 0.5)*(x[i] - 0.5) - std::cos(20.0 * pi<double>() * (x[i] - 0.5));
    }
    // This is the paper version, without the 100 scaling factor on the distace
    // function g
    g = (k + g);

    TVector<double> y(M, (1.0 + g) * 0.5);

    for (int i = 0; i < M; i++) {
        int aux = M - (i + 1);
        for (int j = 0; j < aux; j++) {
            y[i] *= x.at(j);
        }
        if (i != 0){
            y[i] *= (1 - x[aux]);
        }
    }

    return y;
}

TVector<double > DTLZ1(const TVector<double >& x, int M)
{
    int n = x.size();
    int k = n - M + 1;
    double g = 0.0;
    for (int i = n - k; i < n; i++) {
        g += (x[i] - 0.5)*(x[i] - 0.5) - std::cos(20.0 * pi<double>() * (x[i] - 0.5));
    }
    g = 100.0 * ((double)k + g);

    TVector<double> y(M, (1.0 + g) * 0.5);

    for (int i = 0; i < M; i++) {
        int aux = M - (i + 1);
        for (int j = 0; j < aux; j++) {
            y[i] *= x.at(j);
        }
        if (i != 0){
            y[i] *= (1 - x[aux]);
        }
    }

    return y;
}

TVector<double > DTLZ2(const TVector<double >& x, int M)
{
    int i,j;
    int n = x.size();
    int k = n - M + 1;
    double g = 0.0;
    double coss, sine;
    for (int i = n - k; i < n; i++) {
        g += (x[i] - 0.5)*(x[i] - 0.5);
    }

    TVector<double> y(M, 0.0);

    for (j=(M-1); j >= 0; j--) {
        coss = 1.0;
        for (i=0; i<M-j-1; i++) {
            coss *= std::cos(x[i]*pi<double>()/2.0) ;
        }
        sine = (j>0) ? ((j==M-1) ? std::sin(x[M-j-1]*pi<double>()/2.0) : std::sin(x[i]*pi<double>()/2.0)) : 1.0;
        y[j] = (1.0+g) * coss * sine;
    }

    return y;
}

}  // namespace DTLZ
//}  // namespace Tigon
//}  // namespace Operators
