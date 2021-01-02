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
#pragma once
#include <tigon/tigon_global.h>

namespace Tigon {

class LIGER_TIGON_EXPORT PolynomialRegression
{
public:
    explicit PolynomialRegression(const TVector<double> & X,
                         const TVector<double> & Y,
                         int degree);
    /*!
     * \brief PolynomialRegression
     * \param coeff Vector of coefficients: [a0, a1, a2,...,an] where
     * ``` y = a0 + a1*x + a2*x^2 + ... + an*x^n ```
     */
    explicit PolynomialRegression(const TVector<double>& coeff);

    ~PolynomialRegression() {}

    void setCoeff(const TVector<double> &coeff);
    TVector<double> coeff() const;

    int degree() const;
    TString toString() const;
    double error() const;
    double interpolate(double x) const;

private:
    TVector<double> polyfit(const TVector<double> & X,
                            const TVector<double> & Y,
                            int degree);

    int m_degree;
    TVector<double> m_coeff;
    double m_error;

};

}
