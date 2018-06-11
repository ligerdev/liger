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
#include <tigon/Utils/AbstractInterpolator.h>


namespace Tigon {

AbstractInterpolator::AbstractInterpolator(const TVector<double> &x,
                                           const TVector<double> &y, int m)
{
    n    = x.size();
    mm   = m;
    jsav = 0;
    cor  = 0;
    xx   = x;
    yy   = y;
    dj = std::max(1,static_cast<int>(std::pow(static_cast<double>(n),0.25)));
    m_isConfigured = checkConfiguration();
}

AbstractInterpolator::~AbstractInterpolator()
{

}

double AbstractInterpolator::interpolate(double xq)
{
    int jlo = cor ? hunt(xq) : locate(xq);
    return baseInterpolate(jlo,xq);
}

TVector<double> AbstractInterpolator::interpolate(const TVector<double> &xq)
{
    TVector<double> yq;
    int sz = xq.size();
    yq.resize(sz);
    for(int i=0; i<sz; i++) {
        yq[i] = interpolate(xq[i]);
    }

    return yq;
}

void AbstractInterpolator::defineXY(const TVector<double> &x, const TVector<double> &y)
{
    if(x.size() == y.size()) {
        xx = x;
        yy = y;
        n  = x.size();
        jsav = 0;
        cor  = 0;
        dj = std::max(1,static_cast<int>(std::pow(static_cast<double>(n),0.25)));
        m_isConfigured = true;
    }

}

bool AbstractInterpolator::isConfigured()
{
    return m_isConfigured;
}

int AbstractInterpolator::locate(const double x)
{
    int ju,jm,jl;
    bool ascnd=(xx[n-1] >= xx[0]);
    jl=0;
    ju=n-1;
    while (ju-jl > 1) {
        jm = (ju+jl) >> 1;
        if (x >= xx[jm] == ascnd)
            jl=jm;
        else
            ju=jm;
    }
    cor = std::abs(jl-jsav) > dj ? 0 : 1;
    jsav = jl;
    return std::max(0,std::min(n-mm,jl-((mm-2)>>1)));
}

int AbstractInterpolator::hunt(const double x)
{
    int jl=jsav, jm, ju, inc=1;
    bool ascnd=(xx[n-1] >= xx[0]);
    if (jl < 0 || jl > n-1) {
        jl=0;
        ju=n-1;
    } else {
        if (x >= xx[jl] == ascnd) {
            for (;;) {
                ju = jl + inc;
                if (ju >= n-1) { ju = n-1; break;}
                else if (x < xx[ju] == ascnd) break;
                else {
                    jl = ju;
                    inc += inc;
                }
            }
        } else {
            ju = jl;
            for (;;) {
                jl = jl - inc;
                if (jl <= 0) { jl = 0; break;}
                else if (x >= xx[jl] == ascnd) break;
                else {
                    ju = jl;
                    inc += inc;
                }
            }
        }
    }
    while (ju-jl > 1) {
        jm = (ju+jl) >> 1;
        if ((x >= xx[jm]) == ascnd)
            jl=jm;
        else
            ju=jm;
    }
    cor = std::abs(jl-jsav) > dj ? 0 : 1;
    jsav = jl;
    return std::max(0,std::min(n-mm,jl-((mm-2)>>1)));
}

bool AbstractInterpolator::checkConfiguration()
{
    bool status = false;
    if(n < 2 || mm < 2 || mm > n) {
        status = false;
    }

    return status;
}

} // namespace Tigon
