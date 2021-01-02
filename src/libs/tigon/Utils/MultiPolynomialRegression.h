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

class LIGER_TIGON_EXPORT MultiPolynomialRegression
{
public:
    explicit MultiPolynomialRegression(const TVector<TVector<double>> &X,
                                       const TVector<double> &Y,
                                       int degree);
    explicit MultiPolynomialRegression(const TVector<double> &coeff,
                                       const TVector<TVector<double>> &powerMap);

    ~MultiPolynomialRegression(){}

    TVector<TVector<double> > powerMap() const;

    TVector<double> coeff() const;
    void setCoeff(const TVector<double> &coeff);

    double error() const;

    double interpolate(const TVector<double> &X) const;

    TString toString() const;

    static TVector<TVector<double> > generatePowerMap(int nvar, int degree);


private:
    int             m_degree;
    double          m_error;
    int             m_nDim;
    TVector<double> m_coeff;
    TVector<TVector<double> > m_powerMap;

    /*!
     * \brief Generate the powers of all polynomial terms
     * \param degree The maximum order of the polynomial
     * \return Power map
     */
    TVector<double>  multiPolyFit(const TVector<TVector<double>> &X,
                                  const TVector<double> &Y,
                                  int degree);
};

}
