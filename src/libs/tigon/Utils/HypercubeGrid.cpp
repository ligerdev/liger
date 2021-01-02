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
#include <tigon/Utils/HypercubeGrid.h>
#include <tigon/Utils/TigonUtils.h>

#include <iostream>
#include <numeric>

namespace Tigon {

using namespace std;

/*!
 * \brief  creates a direct product of R8VEC's.
 * Ported from John Burkardt's implementation.
 * https://people.sc.fsu.edu/~jburkardt/cpp_src/hypercube_grid/hypercube_grid.html
 */
void r8vecDirectProduct(int factorIndex, int factorOrder,
                        const TVector<double> &factorValue,
                        int pointNum, TVector<TVector<double> > &x,
                        int& contig, int& rep, int& skip)
{
    int start = 0;

    if (factorIndex == 0) {
        contig = 1;
        skip = 1;
        rep = pointNum;
    }

    rep  /= factorOrder;
    skip *= factorOrder;

    for ( int i = 0; i < factorOrder; i++ ) {
        start = i*contig;

        for (int k = 1; k <= rep; k++) {
            for (int j = start; j < start + contig; j++) {
                //x[factorIndex][j] = factorValue[i];
                x[j][factorIndex] = factorValue[i];
            }
            start += skip;
        }
    }
    contig *= factorOrder;

    return;
}

TVector<TVector<double>> hypercubeGrid(int m, const TVector<int> &ns,
                                       const TVector<double> &lbs,
                                       const TVector<double> &ubs,
                                       const TVector<int> &centerScheme)
{
    // [] Initialise x
    int N = std::accumulate(ns.begin(), ns.end(), 1, std::multiplies<int>());
    TVector<TVector<double>> x(N, TVector<double>(m, 0.0));

    // [] Initialise counter
    int contig = 0;
    int rep    = 0;
    int skip   = 0;

    // [] Create the 1D grids in each dimension.
    for (int i = 0; i < m; i++) {
        int numPoint = ns[i];

        TVector<double> xs(numPoint);

        int cScheme = 1;
        if (i < centerScheme.size()) {
            cScheme = centerScheme[i];
        }

        for (int j = 0; j < numPoint; j++ ) {
            switch (cScheme) {
            case 1: {
                // Incldue all end points
                if (numPoint == 1) {
                    xs[j] = 0.5*(lbs[i]+ubs[i]);
                } else {
                    xs[j] = ((numPoint-j-1.0)*lbs[i] + j*ubs[i])/(numPoint - 1.0);
                }
            }
                break;
            case 2: {
                // Exclude both end points
                xs[j] = ((numPoint-j)*lbs[i]+(j+1)*ubs[i])/(numPoint+1.0);
            }
                break;
            case 3: {
                // Exclude the left end points
                xs[j] = ((numPoint-j)*lbs[i]+ (j - 2.0 ) * ubs[i] )/numPoint;
            }
                break;
            case 4: {
                // Exclude the right end point
                xs[j] = ((numPoint-j-1)*lbs[i]+(j+1)*ubs[i])/numPoint;
            }
                break;
            case 5: {
                // Half spacing at the end points
                xs[j] = ((2.0*numPoint-2.0*j-1.0)*lbs[i]+(2.0*j+1)*ubs[i])/(2.0*numPoint);
            }
                break;
            }
        }

        r8vecDirectProduct(i, numPoint, xs, N, x, contig, rep, skip);
    }

    return x;
}

}
