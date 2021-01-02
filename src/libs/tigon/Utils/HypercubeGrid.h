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
#pragma once
#include <tigon/tigon_global.h>

namespace Tigon {

/*!
 * \brief hypercubeGrid
 * Ported from John Burkardt's C++ implementation.
 * https://people.sc.fsu.edu/~jburkardt/cpp_src/hypercube_grid/hypercube_grid.html
 * \param m   Number of dimensions
 * \param ns  Number of points on each dimension
 * \param lbs Lower bounds on each dimension
 * \param ubs Upper bounds on each dimension
 * \param centerScheme The centering options allow the user to control the
 * centering of the equally spaced points. If the cenerScheme is not provided,
 * it will use the default scheme.
 * Five centring schemes are provided. Take lbs = 0s, ubs = 1s, and Ns = 4s as
 * an example :
 *      1: 0, 1/3, 2/3, 1      (include endpoints)
 *      2: 1/5, 2/5, 3/5, 4/5  (exclude endpoints)
 *      3: 0, 1/4, 2/4, 3/4    (exclude the right endpoint)
 *      4: 1/4, 2/4, 3/4, 1    (exclude the left endpoint)
 *      5: 1/8, 3/8, 5/8, 7/8  (half spacing at the ends)
 *
 * \return The hypercude grid. Each row represents a data sample and each column
 * represents a dimesion.
 */
LIGER_TIGON_EXPORT
TVector<TVector<double> > hypercubeGrid(int m, const TVector<int> &ns,
                                        const TVector<double> &lbs,
                                        const TVector<double> &ubs,
                                        const TVector<int> &centerScheme = TVector<int>());

}
