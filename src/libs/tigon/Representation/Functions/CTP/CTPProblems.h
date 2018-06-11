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
#ifndef CTPPROBLEMS_H
#define CTPPROBLEMS_H

#include <tigon/tigon_global.h>


namespace CTP {
  
Tigon::TVector<double> CTP1(const Tigon::TVector<double> &x);
Tigon::TVector<double> CTP2(const Tigon::TVector<double> &x);
Tigon::TVector<double> CTP3(const Tigon::TVector<double> &x);
Tigon::TVector<double> CTP4(const Tigon::TVector<double> &x);
Tigon::TVector<double> CTP5(const Tigon::TVector<double> &x);
Tigon::TVector<double> CTP6(const Tigon::TVector<double> &x);
Tigon::TVector<double> CTP7(const Tigon::TVector<double> &x);

}  // namespace CTP

#endif // CTPPROBLEMS_H
