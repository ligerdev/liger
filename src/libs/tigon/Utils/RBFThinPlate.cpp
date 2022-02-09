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
#include <tigon/Utils/RBFThinPlate.h>

namespace Tigon {

RBFThinPlate::RBFThinPlate(double scale)
    : m_rho(scale)
{

}

RBFThinPlate::~RBFThinPlate()
{

}

double RBFThinPlate::rbf(double r)
{
    return r <= 0.0 ? 0.0 : std::pow(r,2)*log(r/m_rho);
}

} // namespace Tigon

