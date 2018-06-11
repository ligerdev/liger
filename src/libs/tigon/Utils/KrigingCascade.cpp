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
#include <tigon/Utils/KrigingCascade.h>
#include <tigon/Utils/Kriging.h>
#include <tigon/Utils/KrigingVariogram.h>
#include <tigon/Utils/PowerVariogram.h>
#include <tigon/Utils/SphericalVariogram.h>

namespace Tigon {

KrigingCascade::KrigingCascade()
{
    m_produceErrorEstimates = false;
}

KrigingCascade::KrigingCascade(TVector<TVector<double> > XX,
                       TVector<TVector<double> > YY,
                       TString basis)
{
    m_produceErrorEstimates = false;
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

    Kriging* interp;
    TVector<TVector<double> > yl;
    yl.resize(YY[0].size());
    for(int i=0; i<YY[0].size(); i++) {
        yl[i].resize(YY.size());
        for(int j=0; j<YY.size(); j++) {
            yl[i][j] = YY[j][i];
        }
    }

    // TODO - other variograms
    if(basis == "power") {
        for(int i=0; i<ny; i++) {
            interp = new Kriging(new PowerVariogram(XX, yl[i]));
            m_interpolators.push_back(interp);
        }
    } else if(basis=="spherical") {
        for(int i=0; i<ny; i++) {
            interp = new Kriging(new SphericalVariogram(XX,yl[i]));
            m_interpolators.push_back(interp);
        }
    } else {
        for(int i=0; i<ny; i++) {
            interp = new Kriging(new PowerVariogram(XX, yl[i]));
            m_interpolators.push_back(interp);
        }
    }
}

KrigingCascade::KrigingCascade(TVector<KrigingVariogram *> variograms)
{
    m_produceErrorEstimates = false;
    for(int i=0; i<variograms.size(); i++) {
        m_interpolators.push_back(new Kriging(variograms[i]));
    }
    m_xDim = variograms[0]->inputData()[0].size();
}

KrigingCascade::~KrigingCascade()
{
    int sz = m_interpolators.size();
    for(int i=0; i<sz; i++) {
        delete m_interpolators[i];
    }
    // Unnecessary but tidy
    m_interpolators.clear();
}

TVector<double> KrigingCascade::interpolate(TVector<double> x)
{
    TVector<double> y;
    if(x.size() != m_xDim) {
        return y;
    }
    int sz = m_interpolators.size();
    y.resize(sz);
    if(m_produceErrorEstimates) {
        m_errors.clear();
        m_errors.resize(sz);
    }
    for(int i=0; i<sz; i++) {
        y[i] = m_interpolators[i]->interpolate(x);
        if(m_produceErrorEstimates) {
            m_errors[i] = m_interpolators[i]->errorEstimate();
        }
    }

    return y;
}

TVector<double> KrigingCascade::errors()
{
    TVector<double> empty;
    if(m_produceErrorEstimates) {
        return m_errors;
    }

    return empty;
}

TVector<Kriging*> KrigingCascade::interpolators()
{
    return m_interpolators;
}

void KrigingCascade::append(Kriging* interpolator)
{
    if(interpolator != nullptr) {
        m_interpolators.push_back(interpolator);
    }
}

void KrigingCascade::clear()
{
    int sz = m_interpolators.size();
    for(int i=0; i<sz; i++) {
        delete m_interpolators[i];
    }
    m_interpolators.clear();
}

void KrigingCascade::fitVariogram()
{
    for(int i=0; i<m_interpolators.size(); i++) {
        m_interpolators[i]->variogram()->fitVariogram();
    }
}

void KrigingCascade::enableErrorEstimates()
{
    m_produceErrorEstimates = true;
    int sz = m_interpolators.size();
    for(int i=0; i<sz; i++) {
        m_interpolators[i]->estimateErrors(true);
    }
}

void KrigingCascade::disableErrorEstimates()
{
    m_produceErrorEstimates = false;
    int sz = m_interpolators.size();
    for(int i=0; i<sz; i++) {
        m_interpolators[i]->estimateErrors(false);
    }
}

} // namespace Tigon
