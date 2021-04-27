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
#ifndef KERNELDENSITYESTIMATION_H
#define KERNELDENSITYESTIMATION_H
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class ISet;
}
}

namespace Tigon {
using namespace Tigon::Representation;

/// 1D
/*!
 * \brief Silverman's rule of thumb for bandwith
 * \param x data vector
 * \return the optimal bandwith for gaussian kde with 1D data
 */
LIGER_TIGON_EXPORT double silvermanBandwidth(const TVector<double> &x);

LIGER_TIGON_EXPORT double gaussianKDE(const TVector<double> & xx,
                                      double xq, double h = -1.0);

LIGER_TIGON_EXPORT double gaussianKDE(const TVector<double> & xx,
                                      double xq,
                                      std::function<double(const TVector<double> &)> hFuc);

LIGER_TIGON_EXPORT TVector<double> gaussianKDE(const TVector<double> & xx,
                                               const TVector<double> & xq,
                                               double h = -1.0);

LIGER_TIGON_EXPORT TVector<double> gaussianKDE(const TVector<double> & xx,
                                               const TVector<double> & xq,
                                               std::function<double(const TVector<double> &)> hFuc);

/// Multi-dimension
LIGER_TIGON_EXPORT double gaussianKDE(const TVector<TVector<double> > & xx,
                                      const TVector<double> & xq,
                                      double h);

LIGER_TIGON_EXPORT TVector<double> gaussianKDE(const TVector<TVector<double> > & xx,
                                               const TVector<TVector<double> > & xq,
                                               double                    h);

LIGER_TIGON_EXPORT TVector<double> gaussianKDE(const TVector<TVector<double> > & xx,
                                               double                    h);
} // namespace Tigon
#endif // KERNELDENSITYESTIMATION_H
