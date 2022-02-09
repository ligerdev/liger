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
#include <tigon/Utils/Kriging/PowerVariogram.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {

PowerVariogram::PowerVariogram(const TVector<TVector<double>>& x,
                               const TVector<double>& y)
    : KrigingVariogram(0.0, 1.0, 1.0)
{
    assignNugget(0.0);
    assignVariogramData(x, y);
    fitVariogram();
}

PowerVariogram::PowerVariogram(const TVector<TVector<double>>& x,
                               const TVector<double>& y,
               double eta, double nug)
    : KrigingVariogram(0.0, 1.0, 1.0)
{
    assignNugget(nug*nug);
    assignVariogramData(x, y);
    if((eta > 0.0) && (eta < 2.0)) {
        assignEta(eta);
    } else {
        fitVariogram();
    }
}

PowerVariogram::~PowerVariogram()
{

}

double PowerVariogram::value(const double h) const
{
    if(h>0) {
        return (nugget()+m_beta*std::pow(h, m_eta));
    } else {
        return 0;
    }
}

void PowerVariogram::assignEta(double val)
{
        if((val < 1e-4) || (val > 2-1e-4)) {
            return;
        }
        m_eta = val;
        updateBeta();
}

void PowerVariogram::fitVariogram()
{
    if(!dataCheckPassed()) {
        return;
    }
    int N = inputData().size();

    // Extract sample distances
    TCVectorReal dist(N*(N-1)/2);
    TCVectorReal distZ(N*(N-1)/2);
    int k = 0;
    for(int i=0; i<N; i++) {
        for(int j=i+1; j<N; j++) {
            dist(k)  = l2dist(inputData()[i], inputData()[j]);
            distZ(k) = std::fabs(outputData()[i] - outputData()[j]);
            k++;
        }
    }

    // Normalize data
    double maxDist = dist.maxCoeff();
    if(maxDist < 1e-16) {
        dist = 0.0*dist;
    } else {
        dist = dist/maxDist;
    }
    double maxDistZ = distZ.maxCoeff();
    if(maxDistZ < 1e-16) {
        distZ = 0.0*distZ;
    } else {
        distZ = distZ/(distZ.maxCoeff());
    }

    // Empirical variograms
    calculateEmpiricalVariogram(dist, distZ);
    TCVectorReal eV = empiricalVariograms();
    TCVectorReal eL = empiricalLags();
    double hTh = 0.0;
    double hTv = 0.0;
    for(int i=0; i<eV.rows(); i++) {
        hTh += std::log(eL(i))*std::log(eL(i));
        hTv += std::log(eL(i))*std::log(eV(i));
    }
    double eta_star = hTv/hTh;
    double newEta;
    double tolerance = 1e-04;
    if(eta_star < tolerance) {
        newEta = tolerance;
    } else if(eta_star < 2-tolerance) {
        newEta = eta_star;
    } else {
        newEta = 2-tolerance;
    }
    assignEta(newEta);
}

double PowerVariogram::eta() const
{
    return m_eta;
}

double PowerVariogram::beta() const
{
    return m_beta;
}

bool PowerVariogram::dataCheckPassed() const
{
    bool res = true;
    if((inputData().size() == 0) || (outputData().size() == 0)) {
        res = false;
    }
    if(inputData().size() != outputData().size()) {
        res = false;
    }
    return res;
}

void PowerVariogram::updateBeta()
{
    if(!dataCheckPassed()) {
        return;
    }
    TVector<TVector<double> > x = inputData();
    TVector<double> y = outputData();
    int npt  = x.size();     // number of samples
    int ndim = x[0].size();  // dimension of samples
    double rb,num = 0.0, denom = 0.0;
    for(int i=0; i<npt; i++) {
        for(int j=i+1; j<npt; j++) {
            rb = 0.0;
            for(int k=0; k<ndim; k++) {
                rb += std::pow(x[i][k]-x[j][k], 2);
            }
            rb = std::pow(rb, 0.5*m_eta);
            num += rb*(0.5*std::pow(y[i]-y[j], 2) - nugget());
            denom += std::pow(rb, 2);
        }
    }
    m_beta = num/denom;
}

void PowerVariogram::fitVariogramLS(const TVector<TVector<double>>& x,
                                    const TVector<double>& y)
{
    if(x.size() != y.size()) {
        return;
    }

    // Extract sample distances
    int N = x.size();
    TCVectorReal dist(N*(N-1)/2);
    TCVectorReal distZ(N*(N-1)/2);
    int k = 0;
    for(int i=0; i<N; i++) {
        for(int j=i+1; j<N; j++) {
            dist(k)  = l2dist(x[i], x[j]);
            distZ(k) = std::fabs(y[i] - y[j]);
            k++;
        }
    }

    calculateEmpiricalVariogram(dist, distZ);

    int NN = empiricalVariograms().rows();

    TCVectorReal empiricalVariograms = KrigingVariogram::empiricalVariograms();
    TCVectorReal empiricalLags = KrigingVariogram::empiricalLags();

    TCVectorReal YY(NN);
    TMatrixReal XX(NN, 2);
    for(int i = 0; i < NN; i++) {
        YY(i) = std::log(empiricalVariograms(i));
        XX(i, 0) = 1;
        XX(i, 1) = std::log(empiricalLags(i));
    }

    TMatrixReal Q  = XX.transpose()*XX;
    TCVectorReal c = -XX.transpose()*YY;
    TMatrixReal A(2, 2);
    TCVectorReal b(2);
    A(0, 0) = 0; A(0, 1) = 1;
    A(1, 0) = 0; A(1, 1) = -1;
    b(0) = 1e-4;
    b(1) = -2+1e-4;

    TCVectorReal theta = interiorPointSolver(Q, c, A, b);
    m_beta = std::exp(theta(0));
    m_eta  = theta(1);
}

} // namespace Tigon
