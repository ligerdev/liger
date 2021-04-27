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
#include <tigon/Utils/RBFInterpolator.h>
#include <tigon/Utils/RBFBasis.h>

DISABLE_WARNINGS
#include <eigen/Eigen>
ENABLE_WARNINGS

namespace Tigon {

RBFInterpolator::RBFInterpolator(const TVector<TVector<double> > &XX,
                                 const TVector<double> &yy, RBFBasis* basis,
                                 bool toNormalise)
    : m_N          (XX.size())
    , m_y          (yy)
    , m_basis      (basis)
    , m_toNormalise(toNormalise)
{
    if(XX[0].size() > 0) {
        m_xDim  = XX[0].size();
    }
    m_w.resize(m_N);

    double sum;
    TVector<TVector<double> > rbf(m_N);
    for(int i=0; i<m_N; i++) {
        rbf[i].resize(m_N);
    }

    m_x = XX;
    TVector<double> rhs(m_N);

    for(int i=0; i<m_N; i++) {
        sum = 0.0;
        for(int j=0; j<m_N; j++) {
            sum += (rbf[i][j] = m_basis->rbf(l2dist(m_x[i],m_x[j])));
        }
        if(m_toNormalise) {
            rhs[i] = sum*m_y[i];
        } else {
            rhs[i] = m_y[i];
        }
    }

    Eigen::MatrixXd RBF(m_N,m_N);
    Eigen::VectorXd RHS(m_N);
    Eigen::VectorXd W(m_N);
    for(int i=0; i<m_N; i++) {
        for(int j=0; j<m_N; j++) {
            RBF(i,j) = rbf[i][j];
        }
        RHS(i) = rhs[i];
    }
    W = RBF.colPivHouseholderQr().solve(RHS);

    for(int i=0; i<m_N; i++) {
        m_w[i] = W(i);
    }
}

RBFInterpolator::~RBFInterpolator()
{
    if(m_basis) {
        delete m_basis;
    }
}

double RBFInterpolator::interpolate(const TVector<double> &pt)
{
    double fval, sum=0.0, sumw=0.0;
    if(pt.size() != m_xDim) {
        return 0.0;
    }

    for(int i=0; i<m_N; i++) {
        fval = m_basis->rbf(l2dist(pt,m_x[i]));
        sumw += m_w[i]*fval;
        sum += fval;
    }

    return m_toNormalise ? sumw/sum : sumw;
}

double RBFInterpolator::l2dist(const TVector<double> &p1,
                               const TVector<double> &p2)
{
    double sum = 0.0;
    for (int i=0;i<m_xDim;i++) {
        sum += std::pow(p1[i]-p2[i],2);
    }
    return sqrt(sum);
}

} // namespace Tigon
