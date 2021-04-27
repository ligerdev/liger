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

#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>
#include <boost/math/constants/constants.hpp>

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

TVector<double> DTLZ1(const TVector<double>& x, int M)
{
    int n = x.size();
    int k = n - M + 1;
    double g = 0.0;
    for (int i = n - k; i < n; i++) {
        g += std::pow((x[i] - 0.5), 2.0) -
                std::cos(20.0 * pi<double>() * (x[i] - 0.5));
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

TVector<double> DTLZ2(const TVector<double>& x, int M)
{
    int i,j;
    int n = x.size();
    int k = n - M + 1;
    double g = 0.0;
    double coss, sine;
    for (int i = n - k; i < n; i++) {
        g += std::pow((x[i] - 0.5), 2.0);
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

TVector<double> DTLZ5_I(const TVector<double>& x, int I, int N)
{
    int M = (N + I - 1)/2;    // number of objectives
    int ncon = (N - I + 1)/2; // number of constraints
    int n = x.size();
    int k = n - M + 1;

    double g = 0.0;
    for (int i = n - k; i < n; i++) {
        g += std::pow((x[i] - 0.5), 2.0);
    }

    TVector<double> theta(M-1);
    for(int i=0; i<(I-1); i++) {
        theta[i] = (pi<double>()*x[i])/2.0;
    }
    for(int i=I-1; i<(M-1); i++) {
        theta[i] = (pi<double>()*(1.0+2.0*g*x[i]))/(4.0*(1.0+g));
    }

    TVector<double> y(M + ncon);

    for(int i=0; i<M; i++) {
        double sine = (i==0) ? 1.0 : std::sin(theta[M-1-i]);
        double coss = 1.0;
        for(int j=0; j<(M-1-i); j++) {
            coss *= std::cos(theta[j]);
        }
        y[i] = (1.0+g)*coss * sine;
    }

    double ind_fsquare = 0.0;
    for(int j=0; j<=I-2; j++) {
        ind_fsquare += std::pow(y[M-j-1], 2.0);
    }
    for(int i=0; i<ncon; i++) {
        int p = (i==0) ? M-I : M-I+1-i;
        y[i+M] = -1.0*(ind_fsquare - 1.0 + std::pow(2,p) * y[i] * y[i]);
    }

    return y;
}

}  // namespace DTLZ
