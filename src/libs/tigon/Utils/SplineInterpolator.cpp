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
#include <tigon/Utils/SplineInterpolator.h>


namespace Tigon {

SplineInterpolator::SplineInterpolator(const TVector<double>& xv,
                                       const TVector<double>& yv,
                                       double yp1, double ypn)
: AbstractInterpolator(xv,yv,2)
{
    y2.resize(xv.size());
    sety2(xv,yv,yp1,ypn);
    m_isConfigured = (AbstractInterpolator::checkConfiguration() && checkConfiguration());
}

SplineInterpolator::~SplineInterpolator()
{

}

void SplineInterpolator::sety2(TVector<double> xv, TVector<double> yv, double yp1, double ypn)
{
    int i,k;
    double p,qn,sig,un;
    TVector<double> u(n-1);
    if (yp1 > 0.99e99)
        y2[0]=u[0]=0.0;
    else {
        y2[0] = -0.5;
        u[0]=(3.0/(xv[1]-xv[0]))*((yv[1]-yv[0])/(xv[1]-xv[0])-yp1);
    }
    for (i=1;i<n-1;i++) {
        sig=(xv[i]-xv[i-1])/(xv[i+1]-xv[i-1]);
        p=sig*y2[i-1]+2.0;
        y2[i]=(sig-1.0)/p;
        u[i]=(yv[i+1]-yv[i])/(xv[i+1]-xv[i]) - (yv[i]-yv[i-1])/(xv[i]-xv[i-1]);
        u[i]=(6.0*u[i]/(xv[i+1]-xv[i-1])-sig*u[i-1])/p;
    }
    if (ypn > 0.99e99)
        qn=un=0.0;
    else {
        qn=0.5;
        un=(3.0/(xv[n-1]-xv[n-2]))*(ypn-(yv[n-1]-yv[n-2])/(xv[n-1]-xv[n-2]));
    }
    y2[n-1]=(un-qn*u[n-2])/(qn*y2[n-2]+1.0);
    for (k=n-2;k>=0;k--)
        y2[k]=y2[k]*y2[k+1]+u[k];
}

double SplineInterpolator::baseInterpolate(int jl, double x)
{
    int klo=jl;
    int khi=jl+1;
    double y,h,b,a;
    h=xx[khi]-xx[klo];
    a=(xx[khi]-x)/h;
    b=(x-xx[klo])/h;
    y=a*yy[klo]+b*yy[khi]+((a*a*a-a)*y2[klo]
        +(b*b*b-b)*y2[khi])*(h*h)/6.0;
    return y;
}

bool SplineInterpolator::checkConfiguration()
{
    bool status = true;
    int sz = xx.size();
    double hh;
    for(int i=0; i<sz-1; i++) {
        hh = xx[i+1] - xx[i];
        if(hh == 0.0) {
            status = false;
            break;
        }
    }
    return status;
}

} // namespace Tigon
