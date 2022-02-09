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
#ifndef CODEMPROBLEMS_H
#define CODEMPROBLEMS_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

using namespace Tigon::Representation;

namespace CODeM {

LIGER_TIGON_EXPORT Tigon::TVector<double> GECCOExample(
        Tigon::TVector<double> iVec, int nObj);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> GECCOExample(
        Tigon::TVector<double> iVec, int nObj, int nSamp);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> GECCOExamplePerturb(
        int iVecSize, Tigon::TVector<double> oVec, int nSamp = 1);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> GECCOExamplePerturb(
        Tigon::TVector<double> iVec, Tigon::TVector<double> oVec, int nSamp = 1);

LIGER_TIGON_EXPORT Tigon::TVector<double> deterministicOVec(
        int prob, Tigon::TVector<double> iVec, int nObj, int k=0);


LIGER_TIGON_EXPORT Tigon::TVector<double> EMO2019P2(
        Tigon::TVector<double> iVec, int k, int nObj);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> EMO2019P2(
        Tigon::TVector<double> iVec, int k, int nObj, int nSamp);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> EMO2019P2Perturb(
        Tigon::TVector<double> oVec, int nSamp = 1);

LIGER_TIGON_EXPORT Tigon::TVector<double> EMO2019P1(
        Tigon::TVector<double> iVec, int k, int nObj);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> EMO2019P1(
        Tigon::TVector<double> iVec, int k, int nObj, int nSamp);
LIGER_TIGON_EXPORT Tigon::TVector<Tigon::TVector<double>> EMO2019P1Perturb(
        Tigon::TVector<double> oVec, int nSamp = 1);

} // namespace CODeM

#endif // CODEMPROBLEMS_H
