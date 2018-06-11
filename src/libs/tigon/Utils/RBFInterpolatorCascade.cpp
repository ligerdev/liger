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
#include <tigon/Utils/RBFInterpolatorCascade.h>
#include <tigon/Utils/RBFInterpolator.h>
#include <tigon/Utils/RBFGauss.h>
#include <tigon/Utils/RBFThinPlate.h>
#include <tigon/Utils/RBFMultiQuadric.h>
#include <tigon/Utils/RBFInverseMultiQuadric.h>

namespace Tigon {

RBFInterpolatorCascade::RBFInterpolatorCascade()
{

}

RBFInterpolatorCascade::RBFInterpolatorCascade(const TVector<TVector<double> > &XX,
                                               const TVector<TVector<double> > &YY,
                                               TString basis, double scale,
                                               bool toNormalise)
{
    bool isok = true;
    if(YY.size() > 0 && XX.size() >0 ) {
        int np = XX.size();
        for(int i=0; i<np; i++) {
            if(YY[i].size() != np) {
                isok = false;
                break;
            }
        }
        m_xDim = XX[0].size();
    } else {
        isok = false;
    }

    int ny = YY[0].size();

    RBFInterpolator* interp;
    TVector<TVector<double> > yl;
    yl.resize(YY[0].size());
    for(int i=0; i<YY[0].size(); i++) {
        yl[i].resize(YY.size());
        for(int j=0; j<YY.size(); j++) {
            yl[i][j] = YY[j][i];
        }
    }
    if(basis == "gauss") {
        for(int i=0; i<ny; i++) {
            interp = new RBFInterpolator(XX,yl[i],
                                         new RBFGauss(scale),toNormalise);
            m_interpolators.push_back(interp);
        }
    } else if(basis == "thinplate") {
        for(int i=0; i<ny; i++) {
            interp = new RBFInterpolator(XX,yl[i],
                                         new RBFThinPlate(scale),toNormalise);
            m_interpolators.push_back(interp);
        }
    } else if(basis == "multiquadric") {
        for(int i=0; i<ny; i++) {
            interp = new RBFInterpolator(XX,yl[i],
                                         new RBFMultiQuadric(scale),toNormalise);
            m_interpolators.push_back(interp);
        }
    } else if(basis == "inversemultiquadric") {
        for(int i=0; i<ny; i++) {
            interp = new RBFInterpolator(XX,yl[i],
                                         new RBFInverseMultiQuadric(scale),toNormalise);
            m_interpolators.push_back(interp);
        }
    } else {
        for(int i=0; i<ny; i++) {
            interp = new RBFInterpolator(XX,yl[i],
                                         new RBFGauss(scale),toNormalise);
            m_interpolators.push_back(interp);
        }
    }
}

RBFInterpolatorCascade::~RBFInterpolatorCascade()
{
    int sz = m_interpolators.size();
    for(int i=0; i<sz; i++) {
        delete m_interpolators[i];
    }
    // Unnecessary but tidy
    m_interpolators.clear();
}

TVector<double> RBFInterpolatorCascade::interpolate(const TVector<double> &x)
{
    TVector<double> y;
    if(x.size() != m_xDim) {
        return y;
    }
    int sz = m_interpolators.size();
    y.resize(sz);
    for(int i=0; i<sz; i++) {
        y[i] = m_interpolators[i]->interpolate(x);
    }

    return y;
}

void RBFInterpolatorCascade::append(RBFInterpolator* interpolator)
{
    if(interpolator != nullptr) {
        m_interpolators.push_back(interpolator);
    }
}

void RBFInterpolatorCascade::clear()
{
    int sz = m_interpolators.size();
    for(int i=0; i<sz; i++) {
        delete m_interpolators[i];
    }
    m_interpolators.clear();
}

} // namespace Tigon
