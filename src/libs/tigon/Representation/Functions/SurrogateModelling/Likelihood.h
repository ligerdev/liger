/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef LIKELIHOOD_H
#define LIKELIHOOD_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT Likelihood : public IFunction
{
public:
    Likelihood();
    Likelihood(const TVector<TVector<double>>& x, const TVector<double>& y);
    ~Likelihood();

    // Information about the function(s)
    TString name();
    TString description();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

    void defineData(const TVector<TVector<double>>& x, const TVector<double>& y);
    TVector<TVector<double>> xValues() const;
    TVector<double>          yValues() const;

private:
    void defineInputPrpts();
    void defineOutputPrpts();

    int  m_N;    // number of points
    int  m_xDim; // number of dimensions

    TVector<TVector<double>> m_x; // x-data
    TVector<double>          m_y; // y-data
};

} // namespace Representation
} // namespace Tigon

#endif // LIKELIHOOD_H
