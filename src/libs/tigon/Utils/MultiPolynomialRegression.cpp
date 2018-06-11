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
#include <tigon/Utils/MultiPolynomialRegression.h>
#include <tigon/Utils/SimplexLattice.h>
#include <tigon/tigonconstants.h>
#include <tigon/ExceptionHandling/TException.h>

#include <eigen/Core>

using namespace Eigen;

namespace Tigon {

MultiPolynomialRegression::MultiPolynomialRegression(const TVector<TVector<double> > &X,
                                                     const TVector<double> &Y,
                                                     int degree)
    : m_degree(degree)
    , m_error(Highest)
{
    if(X.size() > 0 && Y.size()) {
        m_nDim = X[0].size();
        m_coeff = multiPolyFit(X, Y, degree);
    }
}

MultiPolynomialRegression::MultiPolynomialRegression(const TVector<double> &coeff,
                                                     const TVector<TVector<double>> &powerMap)
    : m_error(0)
{
    if(coeff.size() > 0 && powerMap.size() > 0
            && coeff.size() == powerMap.size()) {
        m_coeff = coeff;
        m_powerMap = powerMap;
        m_degree = m_coeff.size()-1;
        m_nDim = m_powerMap[0].size();
    }
}

TVector<TVector<double> > MultiPolynomialRegression::powerMap() const
{
    return m_powerMap;
}

TVector<double> MultiPolynomialRegression::coeff() const
{
    return m_coeff;
}

double MultiPolynomialRegression::error() const
{
    return m_error;
}

double MultiPolynomialRegression::interpolate(const TVector<double> &X) const
{
    int nTerms = m_powerMap.size();
    double val(0.0);
    for (int i = 0; i < nTerms; i++) {
        // ith term
        double jval(1.0);
        for (int j=0; j<m_nDim; j++) {
            // jth dim
            jval *= pow(X[j], m_powerMap[i][j]);
        }
        val += m_coeff[i]*jval;
    }
    return val;
}

TString MultiPolynomialRegression::toString() const
{
    int nTerms = m_powerMap.size();
    TString str;
    for (int i = 0; i < nTerms; i++) {
        // ith term
        TString termsString;
        for (int j=0; j<m_nDim; j++) {
            // jth dim
            int pw = static_cast<int>(m_powerMap[i][j]);
            if(pw > 0) {
                if (pw > 1) {
                    termsString.append( "x" + convertToString(j)
                                        + "^{" + convertToString(pw) + "}");
                } else {
                    termsString.append( "x" + convertToString(j));
                }
            }
        }

        double coeff = m_coeff[i];
        TString signStr = "";

        if (i > 0) {
            if(coeff < 0) {
                signStr = " - ";
                coeff = -coeff;
            } else {
                signStr = " + ";
            }
        }
        if(termsString.empty()) {
            str.append(signStr + convertToString(coeff));
        } else {
            str.append(signStr + convertToString(coeff) + "*" + termsString);
        }
    }
    return str;
}

TVector<TVector<double> > MultiPolynomialRegression::generatePowerMap(int nvar,
                                                                      int degree)
{
    TVector<TVector<double>> powers;
    powers.push_back(TVector<double>(nvar));
    for (int pw = 1; pw < degree+1; pw++) {
        TVector<TVector<double>> simplex = simplexLatticeRecursive(pw, nvar, pw);
        powers.insert(powers.end(), simplex.begin(), simplex.end());
    }
    return powers;
}

void MultiPolynomialRegression::setCoeff(const TVector<double> &coeff)
{
    if(coeff.size() > 0 && m_powerMap.size() > 0
            && coeff.size() == m_powerMap.size()) {
        m_coeff = coeff;
    } else {
        throw TException("MultiPolynomialRegression", "Coeff does not match the power map");
    }
}

TVector<double> MultiPolynomialRegression::multiPolyFit(const TVector<TVector<double> > &X,
                                                        const TVector<double> &Y,
                                                        int degree)
{
    m_powerMap = generatePowerMap(X[0].size(), degree);
    int nTerms = m_powerMap.size();

    Eigen::MatrixXd A(X.size(), nTerms);
    Eigen::VectorXd y = Eigen::VectorXd::Map(&Y.front(), Y.size());

    // Create coeff matrix A
    for (size_t i = 0; i<X.size(); i++) {
        // the ith row of the data
        for (int j = 0; j < nTerms; j++) {
            // jth term
            double aij(1.0);
            for (int k=0; k<m_nDim; k++) {
                // kth dim
                aij *= pow(X[i][k], m_powerMap[j][k]);
            }
            A(i,j) = aij;
        }
    }

    // Least square fitting Ax=y => y = (A'A)^{-1} * (Ay)
    Eigen::VectorXd result = A.householderQr().solve(y);
    m_error = (A*result - y).norm() / y.norm();

    // Copy data to a std vector
    TVector<double> coeffs(nTerms);
    VectorXd::Map(&coeffs[0], result.size()) = result;

    return coeffs;
}

} // namespace Tigon
