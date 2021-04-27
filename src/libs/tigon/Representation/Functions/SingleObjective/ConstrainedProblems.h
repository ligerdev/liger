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

#ifndef CONSTRAINEDPROBLEMS_H
#define CONSTRAINEDPROBLEMS_H

#include <tigon/tigon_global.h>

namespace ConstrainedProblems
{
Tigon::TVector<double> CBranin(const Tigon::TVector<double> &x);
Tigon::TVector<double> CBraninM1(const Tigon::TVector<double> &x);
Tigon::TVector<double> Camel(const Tigon::TVector<double> &x);
Tigon::TVector<double> Sasena(const Tigon::TVector<double> &x);
Tigon::TVector<double> BraninGomez(const Tigon::TVector<double> &x);

} // namespace ConstrainedProblems

#endif // CONSTRAINEDPROBLEMS_H
