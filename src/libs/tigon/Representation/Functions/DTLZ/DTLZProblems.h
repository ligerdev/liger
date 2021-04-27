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
#ifndef DTLZPROBLEMS_H
#define DTLZPROBLEMS_H

#include <tigon/tigon_global.h>
namespace DTLZ {

Tigon::TVector<double> LIGER_TIGON_EXPORT DTLZ1Modified(
        const Tigon::TVector<double >& x, int M);

Tigon::TVector<double> LIGER_TIGON_EXPORT DTLZ1(
        const Tigon::TVector<double >& x, int M);
Tigon::TVector<double> LIGER_TIGON_EXPORT DTLZ2(
        const Tigon::TVector<double >& x, int M);
Tigon::TVector<double> LIGER_TIGON_EXPORT DTLZ5_I(
        const Tigon::TVector<double>& x, int I, int N);

// TODO: implement these
//Tigon::TVector<double > DTLZ3(const Tigon::TVector<double >& x, int M);
//Tigon::TVector<double > DTLZ4(const Tigon::TVector<double >& x, int M);
//Tigon::TVector<double > DTLZ5(const Tigon::TVector<double >& x, int M);
//Tigon::TVector<double > DTLZ6(const Tigon::TVector<double >& x, int M);
//Tigon::TVector<double > DTLZ7(const Tigon::TVector<double >& x, int M);

}  // namespace DTLZ

#endif // DTLZPROBLEMS_H
