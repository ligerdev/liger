/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Utils/KernelDensityEstimation.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <boost/math/constants/constants.hpp>
#include <numeric>

using namespace std;
using namespace boost::math::constants;

namespace Tigon {

double gaussianKDE(const TVector<TVector<double>>& xx,
                   const TVector<double> &xq, double h)
{
    double d = xq.size();
    double n = xx.size();
    double f = 0.0;
    double twoHSquare = 2.0*h*h;

    for(int i=0; i<n; i++) {
        double dist = l2dist(xx[i], xq);
        f += std::exp(-dist*dist / twoHSquare);
    }
    f /= std::pow(twoHSquare * pi<double>(), d/2.0) ;
    f /= n;

    return f;
}

TVector<double> gaussianKDE(const TVector<TVector<double> > & xx,
                            const TVector<TVector<double> > & xq,
                            double                    h)
{
    TVector<double> fVec;

    for(int i=0; i<xq.size(); i++) {
        double f = gaussianKDE(xx, xq[i], h);
        fVec.push_back(f);
    }

    return fVec;
}

TVector<double> gaussianKDE(const TVector<TVector<double> > & xx,
                            double                    h)
{
    return gaussianKDE(xx, xx, h);
}

double silvermanBandwidth(const TVector<double> &x)
{
    // h = std(data)*(4/3/n)^(1/5); % Silverman's rule of thumb

    int n = x.size();
    // Calculate mean
    double mean = std::accumulate(x.begin(), x.end(), 0.0);
    mean = mean/n;

    // Calculate unbiased variance (from Wikipedia)
    double stdv  = 0.0;
    for(int i=0; i<x.size(); i++) {
        stdv += std::pow(x[i] - mean, 2.0);
    }
    stdv /= n-1;
    stdv = sqrt(stdv);

    return stdv * pow(4.0/3.0/n, 1.0/5.0);
}

double gaussianKDE(const TVector<double> &xx, double xq, double h)
{
    if (h<0) {
        h = silvermanBandwidth(xx);
    }
    int n = xx.size();
    double f = 0.0;

    for(int i=0; i<n; i++) {
        double u = (xx[i] - xq)/h;
        f += std::exp(-u*u/2.0)/std::sqrt(2.0*pi<double>());
    }
    f /= n*h;

    return f;
}

double gaussianKDE(const TVector<double> & xx,
                   double xq,
                   std::function<double(const TVector<double> &)> hFuc)
{
    double h = hFuc(xx);
    return gaussianKDE(xx, xq, h);
}


TVector<double> gaussianKDE(const TVector<double> &xx,
                            const TVector<double> &xq, double h)
{
    if (h<0) {
        h = silvermanBandwidth(xx);
    }

    TVector<double> fVec;
    std::transform(xq.begin(), xq.end(), std::back_inserter(fVec),
                   [xx, h](double x) { return gaussianKDE(xx, x, h);});

    return fVec;
}

TVector<double> gaussianKDE(const TVector<double> &xx, const TVector<double> &xq,
                            std::function<double (const TVector<double> &)> hFuc)
{
    double h = hFuc(xx);
    return gaussianKDE(xx, xq, h);
}

} // namespace Tigon
