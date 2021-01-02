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
#include <tigon/Utils/Kriging/KrigingVariogram.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {

KrigingVariogram::KrigingVariogram()
    : m_alpha(1.0), m_c0(0.0), m_c(1.0)
{

}

KrigingVariogram::KrigingVariogram(double nug, double cparam, double alpha)
{
    assignNugget(nug);
    assignC(cparam);
    assignAlpha(alpha);
}

KrigingVariogram::~KrigingVariogram()
{
    m_empiricalVariograms.resize(1);
    m_empiricalLags.resize(1);
}

void KrigingVariogram::assignEta(double val)
{
    T_UNUSED(val);
}

double KrigingVariogram::eta() const
{
    return 0;
}

double KrigingVariogram::beta() const
{
    return 0;
}

void KrigingVariogram::assignAlpha(double val)
{
    if(val>0) {
        m_alpha = val;
    } else {
        m_alpha = 1.0;
    }
}

void KrigingVariogram::assignNugget(double val)
{
    if(val>0) {
        m_c0 = val;
    } else {
        m_c0 = 0.0;
    }
}

void KrigingVariogram::assignC(double val)
{
    if(val>=0) {
        m_c = val;
    } else {
        m_c = 1.0;
    }
}

void KrigingVariogram::assignVariogramData(const TVector<TVector<double>>& x,
                                           const TVector<double>& y)
{
    if((x.size() == 0) || (y.size() == 0)) {
        return;
    }
    if(x.size() != y.size()) {
        return;
    }
    m_x.clear();
    m_y.clear();
    m_x = x;
    m_y = y;
}

void KrigingVariogram::calculateEmpiricalVariogram(const TCVectorReal& dist,
                                                   const TCVectorReal& Zx)
{
    // dist: || xi-xj ||_2
    // Zx contains all realization of x's, i.e z(xi)

    int N = dist.rows();
    // number of samples per bin
    int Npb;
    if(N < 20) {
        Npb = std::floor(N/3);
    } else {
        Npb = std::floor(8*std::log(N)/std::log(10));
    }
    int bin = std::ceil(static_cast<double>(N/Npb));

    m_empiricalVariograms.resize(bin);
    m_empiricalLags.resize(bin);

    TVector<int>   sortedIndex(N);
    TVector<double> sortedDist(N);
    for(int i=0; i<N; i++) {
        sortedDist[i] = dist(i);
    }
    sortedIndex = indSort(sortedDist);

    double gamma = 0;
    double lh;
    double uh = sortedDist[0];
    for(int i=0; i<bin; i++) {
        int mh;
        lh = uh;
        if(i == bin-1) {
            uh = sortedDist[N-1];
            mh = N - (bin-1)*Npb;
        } else {
            mh = Npb;
            uh = sortedDist[(i+1)*Npb-1];
        }

        for(int j=0; j<mh; j++) {
            int idx = sortedIndex[i*Npb+j];
            gamma += std::pow(Zx(idx), 2);
        }
        gamma = gamma/mh; // do not need to be divided by 2
        m_empiricalVariograms(i) = gamma;
        m_empiricalLags(i) = (lh+uh)/2.0;
    }
}

void KrigingVariogram::calculateEmpiricalVariogram(
        const TVector<TVector<double>>& x,
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
}

TCVectorReal KrigingVariogram::empiricalVariograms() const
{
    return m_empiricalVariograms;
}

TCVectorReal KrigingVariogram::empiricalLags() const
{
    return m_empiricalLags;
}

double KrigingVariogram::alpha() const
{
    return m_alpha;
}

double KrigingVariogram::paramC() const
{
    return m_c;
}

double KrigingVariogram::nugget() const
{
    return m_c0;
}

double KrigingVariogram::sill() const
{
    return (m_c+m_c0);
}

int KrigingVariogram::nInputs() const
{
    if(!m_x.empty()) {
        return m_x.at(0).size();
    } else {
        return 0;
    }
}

TVector<TVector<double> > KrigingVariogram::inputData()  const
{
    return m_x;
}

TVector<double> KrigingVariogram::outputData() const
{
    return m_y;
}

TCVectorReal KrigingVariogram::interiorPointSolver(
        const TMatrixReal& Q, const TCVectorReal& c,
        const TMatrixReal& A, const TCVectorReal& b)
{
    // Interior point solver
    int64 m = A.rows();
    int64 n = Q.cols();

    double eps = 1e-06;
    int maxN  = 50;
    double xi  = 0.9995;
    double bd  = 1.0 + std::max(b.cwiseAbs().maxCoeff(), c.cwiseAbs().maxCoeff());

    // Initialization
    TCVectorReal x(n);
    TCVectorReal lambda(m); // dual vars
    TCVectorReal s(m);      // slacks

    TCVectorReal Rd(n);
    TCVectorReal Rp(m);
    TCVectorReal Rls(m);
    TCVectorReal Res(n+m+m);

    for(int i=0; i<n; i++) {
        x(i) = 1.0;
    }
    for(int i=0; i<m; i++) {
        s(i) = 1.0;
        lambda(i) = 1.0;
    }

    // Main loop
    double residual = bd;
    double mu = bd;
    double alpha_max = 0.0;
    double alphal = 0.0;
    double alphas = 0.0;
    for(int k=0; k<maxN; k++) {
        // Get residuals
        Rd  = Q*x - A.transpose()*lambda + c;
        Rp  = A*x - s - b;
        Rls = lambda.cwiseProduct(s);
        Res << Rd,
                Rp,
                Rls;
        mu = Rls.sum()/n;
        residual = Res.cwiseAbs().maxCoeff();

        if((residual < eps) && (mu < eps)) {
            break;
        }

        // Affine-scaling step
        TMatrixReal  D_2(m, m);
        TCVectorReal rhs(Rd.rows()+Rp.rows());
        TMatrixReal  B(m+n, m+n);

        rhs << -Rd,
                -Rp - Rls.cwiseQuotient(lambda);
        D_2 = s.cwiseQuotient(lambda).asDiagonal();
        B   << Q,  -A.transpose(),
                A, D_2;
        Eigen::FullPivHouseholderQR<TMatrixReal> factor = B.fullPivHouseholderQr();
        rhs = factor.solve(rhs);
        TCVectorReal dx = rhs.head(n);
        TCVectorReal dl = rhs.tail(m);
        TCVectorReal ds = -Rls.cwiseQuotient(lambda)-D_2*dl;

        alphal = (dl.cwiseQuotient(lambda)).minCoeff();
        alphas = (ds.cwiseQuotient(s)).minCoeff();
        alphal = -1.0/std::min(alphal, -1.0);
        alphas = -1.0/std::min(alphas, -1.0);
        alphal = std::min(xi*alphal, 1.0);
        alphas = std::min(xi*alphas, 1.0);

        // Corrector + centering step
        double mu_aff = (lambda+alphal*dl).dot(s+alphas*ds)/n;
        double sigma  = std::pow(mu_aff/mu, 3.0);
        Rls = Rls+dl.cwiseProduct(ds) - sigma*mu*TCVectorReal::Ones(m);
        rhs << -Rd,
                -Rp - Rls.cwiseQuotient(lambda);
        rhs = factor.solve(rhs);

        dx = rhs.head(n);
        dl = rhs.tail(m);
        ds = -Rls.cwiseQuotient(lambda)-D_2*dl;

        alphal = (dl.cwiseQuotient(lambda)).minCoeff();
        alphas = (ds.cwiseQuotient(s)).minCoeff();
        alphal = -1.0/std::min(alphal, -1.0);
        alphas = -1.0/std::min(alphas, -1.0);
        alphal = std::min(xi*alphal, 1.0);
        alphas = std::min(xi*alphas, 1.0);

        // Update
        alpha_max = std::max(alphal, alphas);
        x = x + alpha_max*dx;
        s = s + alphas*ds;
        lambda = lambda + alphal*dl;
    }
    return x;
}
} // namespace Tigon
