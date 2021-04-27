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
#ifndef SIMPLEXLATTICE_H
#define SIMPLEXLATTICE_H
#include <tigon/tigon_global.h>
#include <tigon/Utils/DominanceUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/NormalisationUtils.h>

namespace Tigon {

/*
 * Maximum number of points for a simplex lattice
 * */
enum CONSTANTS {MAX_COMPLETE_SIMPLEX_LATTICE_SIZE = 10000000};

/*
 * Determines the size of a k-1 simplex with h+1 values for every component.
 * */
int LIGER_TIGON_EXPORT simplexLatticeSize(int h, int k);

/*
 * A recursive function that generates a k-1 simplex with h+1 values
 * for every component.
 * |x| = s, for every point x = [x_1,...,x_k].
 * The default for s is 1.
 * */
TVector<TVector<double>> LIGER_TIGON_EXPORT
simplexLatticeRecursive(int h, int k, double s=1.0);

/*
 * An iterative function that generates a k-1 simplex with h+1 values
 * for every component.
 * |x| = 1, for every point x = [x_1,...,x_k].
 * */
TVector<TVector<double>> LIGER_TIGON_EXPORT
simplexLatticeIterative(int h, int k);

/*
 * Generates a k-1 simplex with h+1 values for every component,
 * where the values in each component are restricted by an upper bound.
 * |x| = 1, for every point x = [x_1,...,x_k], and x_i<=u_i for all i=1,...,k.
 * */
TVector<TVector<double>> LIGER_TIGON_EXPORT
constrainedSimplexLattice(int h, int k, const TVector<double>& upperBound);

/*
 * Generates a k-1 simplex with h+1 values for every component,
 * and removes the points that:
 * (1) do not weakly dominate the goal vector,
 *     if the goal vector is on the top of the simplex; or
 * (2) are not weakly dominated by the goal vector,
 *     if the goal vector is below the simplex.
 * |x| = 1, for every point x = [x_1,...,x_k].
 * */
TVector<TVector<double>> LIGER_TIGON_EXPORT
preferredSimplexLattice(int h, int k, const TVector<double> &goalVec);

/*
 * Generates a k-1 simplex with h+1 values for every component,
 * and removes the points that:
 * (1) do not weakly dominate the goal vector,
 *     if the goal vector is on the top of the simplex; or
 * (2) are not weakly dominated by the goal vector,
 *     if the goal vector is below the simplex.
 * |x| = 1, for every point x = [x_1,...,x_k]. *
 * If the determined number of weight vectors exceed a given number
 * (i.e., maxNumberWeightVector), each weight vectors gets an estimate of its
 * own crowding distance, and the most crowded weight vectors are deleted until
 * the final number is equal to maxNumberWeightVector.
 * */
TVector<TVector<double>> LIGER_TIGON_EXPORT
preferredSimplexLatticeCrowding(int h, int k, const TVector<double> &goalVec,
                                int maxNumberWeightVector);

/*
 * Generates a preferred simplex, where the number of components is equal to the
 * number of true elements in componentsSubset vector, and some points are
 * removed based on the goal vector (as in preferredSimplexLattice).
 * The number of components in the simplex is extended to k-1, where the added
 * components have elements with value 0.
 * */
TVector<TVector<double>> LIGER_TIGON_EXPORT
preferredSimplexLatticeRedundantComponents(int h, int k,
                                    const TVector<double> &goalVec,
                                    const TVector<bool> &componentsSubset);

/*
 * Applies a linear transformation to a simplex lattice based on the
 * goal vector.
 * xi = xi*s + gi, i=1,...,k, where s=1-\sum gi
 * */
void LIGER_TIGON_EXPORT
simplexLatticeLinearTransformation(TVector<TVector<double>>& set,
                                   const TVector<double> &goalVec);

} // namespace Tigon
#endif // SIMPLEXLATTICE_H
