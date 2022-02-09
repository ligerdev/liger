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
#ifndef RBFGAUSS_H
#define RBFGAUSS_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/RBFBasis.h>

namespace Tigon {

class LIGER_TIGON_EXPORT RBFGauss : public RBFBasis
{
public:
    RBFGauss(double scale=1.0);
    ~RBFGauss();

    double rbf(double r);

private:
    double m_rho;
};

} // namespace Tigon

#endif // RBFGAUSS_H
