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
#ifndef NORMALISATIONUTILS_H
#define NORMALISATIONUTILS_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
using namespace Tigon::Representation;

//TODO: test the IElement normalisation functions once double functions are merged

// The vector is normlaised to be a unit vector in p-norm.
// Return value is the magnitude.
LIGER_TIGON_EXPORT double magnitudeAndDirectionP(TVector<double>& vec,
                                                double p = 2.0);

LIGER_TIGON_EXPORT double magnitudeAndDirectionP(TVector<IElementSPtr>& vec,
                                                double p = 2.0);

LIGER_TIGON_EXPORT void toUnitVec(TVector<double>& vec,        double norm = 2.0);

LIGER_TIGON_EXPORT void toUnitVec(TVector<IElementSPtr>& vec, double norm = 2.0);

// The vector is scaled by a factor
LIGER_TIGON_EXPORT void scale(TVector<double>& dir,
                              double factor);

LIGER_TIGON_EXPORT void scale(TVector<IElementSPtr> dir,
                              double factor);

// The vector is normlaised to be within a unit hyperbox,
// where the ideal and the anti-ideal are set to 0 and 1, respectively.
LIGER_TIGON_EXPORT void normaliseToUnitInterval(double& val,
                                                double  lBound,
                                                double  uBound);

/*!
 * \brief normaliseToUnitInterval
 * The normalised IElement is converted to Real type.
*/
LIGER_TIGON_EXPORT void normaliseToUnitInterval(IElement& val,
                                                const IElement& lBound,
                                                const IElement& uBound);

/*!
 * \brief normaliseToUnitBoxInMemory
 * Normalises the input vector based on the lower and upper bound vectors
 * vec = (vec-lBound)/(uBound-lBound)
*/
LIGER_TIGON_EXPORT void normaliseToUnitBoxInMemory(TVector<double>& vec,
                                           const TVector<double> &lBounds,
                                           const TVector<double> &uBounds);

/*!
 * \brief normaliseToUnitBoxInMemory
 * Normalises the input vector based on the bounds provided
 * vec = (vec-lb)/(ub-lb)
*/
LIGER_TIGON_EXPORT void normaliseToUnitBoxInMemory(TVector<IElementSPtr> vec,
                                           BoxConstraintsDataSPtr box);

/*!
 * \brief normaliseToUnitBox
 * Normalises the input vector based on the lower and upper bound vectors
 * Returns a copy of the normalised vector without modifying the original one
 * new_vec = (vec-lBounds)/(uBounds-lBounds)
*/
LIGER_TIGON_EXPORT TVector<double> normaliseToUnitBox(const TVector<double>& vec,
                                           const TVector<double>& lBounds,
                                           const TVector<double>& uBounds);

LIGER_TIGON_EXPORT void scaleBackFromUnitInterval(double& normVal,
                                                  double  lBound,
                                                  double  uBound);

LIGER_TIGON_EXPORT void scaleBackFromUnitInterval(IElement& normVal,
                                                  const IElement& lBound,
                                                  const IElement& uBound);

LIGER_TIGON_EXPORT void scaleBackFromUnitBox(TVector<double>& normVec,
                                             const TVector<double> &lBounds,
                                             const TVector<double> &uBounds);

LIGER_TIGON_EXPORT void scaleBackFromUnitBox(TVector<IElementSPtr> normVec,
                                             BoxConstraintsDataSPtr box);

/*
 * Formats a preference goal vector for the simplex lattice.
 * Takes into account components of the goal vector that have not been set.
 * */
LIGER_TIGON_EXPORT
TVector<double> normaliseForSimplexLattice(const TVector<double>& vec,
                                           const TVector<double>& lBounds,
                                           const TVector<double>& uBounds,
                                           double valueNotSet=Tigon::Lowest);

///\brief The normalised difference between two nominal categorical values is
/// either zero if they are the same or 1/(n-1) if they are different, where n is
/// the number of categories.
LIGER_TIGON_EXPORT double normalisedDistanceP(TVector<IElementSPtr> m,
                                              TVector<IElementSPtr> c,
                                              BoxConstraintsDataSPtr box,
                                              double p=2.0);

LIGER_TIGON_EXPORT double normalisedDistanceVecP(const TVector<double> &m,
                                                 const TVector<double> &c,
                                                 BoxConstraintsDataSPtr box,
                                                 double p=2.0);

// The length of a vector passing through the 1-norm direction vector dir,
// ending at the boundary of the unit box.
LIGER_TIGON_EXPORT double directedBoxedIntervalLength(const TVector<double> dir,
                                                      double p=2.0);

} // namespace Tigon
#endif // NORMALISATIONUTILS_H
