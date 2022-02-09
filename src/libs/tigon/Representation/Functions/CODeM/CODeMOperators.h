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
#ifndef CODEMOPERATORS_H
#define CODEMOPERATORS_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace CODeM {

// Relations between UncertaintyKernel properties and uncertainty parameters
double LIGER_TIGON_EXPORT linearDecrease(double val);
double LIGER_TIGON_EXPORT skewedIncrease(double val, double alpha);
double LIGER_TIGON_EXPORT skewedDecrease(double val, double alpha);
double LIGER_TIGON_EXPORT lowOnValue(double val, double zeroVal, double width);
double LIGER_TIGON_EXPORT highOnValue(double val, double oneVal, double width);

Tigon::TVector<double> LIGER_TIGON_EXPORT directionPerturbation(
        const Tigon::TVector<double> &oVec, double maxRadius, double pNorm=2);

} // namespace GECCO2016

#endif // CODEMOPERATORS_H
