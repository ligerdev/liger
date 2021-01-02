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
#include <tigon/Utils/PolynomialRegression.h>
#include <tigon/tigonconstants.h>

DISABLE_WARNINGS
#include <eigen/Core>
ENABLE_WARNINGS

using namespace Eigen;

namespace Tigon {

PolynomialRegression::PolynomialRegression(const TVector<double> & X,
                                           const TVector<double> & Y,
                                           int degree)
    : m_degree(degree)
    , m_error(Tigon::Highest)
{
    if (X.size() == Y.size()
            && degree < X.size() && degree > 0) {
        m_coeff = polyfit(X, Y, degree);
    }
}

PolynomialRegression::PolynomialRegression(const TVector<double> &coeff)
    : m_error(0)
{
    setCoeff(coeff);
}

void PolynomialRegression::setCoeff(const TVector<double> &coeff)
{
    if(coeff.size() > 0) {
        m_coeff = coeff;
        m_degree = coeff.size()-1;
    }
}

TVector<double> PolynomialRegression::coeff() const
{
    return m_coeff;
}

int PolynomialRegression::degree() const
{
    return m_degree;
}

TString PolynomialRegression::toString() const
{
    TString str;
    if (m_coeff.size()>0) {
        for (std::size_t i=0; i<m_coeff.size(); i++) {
            double val = m_coeff[i];
            if (i == 0) {
                str.append(convertToString(val));
            } else {
                TString sign(" + ");
                if (val < 0) {
                    sign = " - ";
                }
                str.append(sign + convertToString(std::abs(val)) + "*x"
                           + "^" + convertToString(i));
            }
        }
    }
    return str;
}

TVector<double> PolynomialRegression::polyfit(const TVector<double> &X,
                                              const TVector<double> &Y,
                                              int degree)
{
    Eigen::MatrixXd A(X.size(), degree+1);
    Eigen::VectorXd y = Eigen::VectorXd::Map(&Y.front(), Y.size());
    Eigen::VectorXd result;

    // Create coeff matrix A
    for (size_t i = 0; i < X.size(); i++) {
        for (size_t j = 0; j < degree+1; j++) {
            A(i, j) = std::pow(X.at(i), j);
        }
    }

    // Least square fitting Ax=y => y = (A'A)^{-1} * (Ay)
    result = A.householderQr().solve(y);
    m_error = (A*result - y).norm() / y.norm();

    // Copy data to a std vector
    TVector<double> coeffs(degree+1);
    VectorXd::Map(&coeffs[0], result.size()) = result;

    return coeffs;
}

double PolynomialRegression::error() const
{
    return m_error;
}

double PolynomialRegression::interpolate(double x) const
{
    double res(0);
    for(int i=0; i<m_degree+1; i++) {
        res += m_coeff[i]*std::pow(x, i);
    }
    return res;
}

}
