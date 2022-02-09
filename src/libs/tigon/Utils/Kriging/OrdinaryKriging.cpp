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
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {

OrdinaryKriging::OrdinaryKriging()
{
    // Default constructor
    m_xDim = 0;
    m_N = 0;
    m_lastEstimate = 0;
    m_predictionErrorEstimate = 0;
    m_localErrorEstimate = 0;
    m_krigingStdEstimate = 0;
    m_variogram = 0;
}

OrdinaryKriging::OrdinaryKriging(KrigingVariogram* vargram, const TVector<double> &err)
{
    m_xDim = 0;
    m_N = 0;
    m_lastEstimate = 0;
    m_predictionErrorEstimate = 0;
    m_localErrorEstimate = 0;
    m_krigingStdEstimate = 0;
    m_variogram = 0;

    assignVariogram(vargram, err);
}

OrdinaryKriging::~OrdinaryKriging()
{
    if(m_variogram) {
        delete m_variogram;
    }
}

void OrdinaryKriging::assignVariogram(KrigingVariogram* vargram, const TVector<double> &err)
{
    if(m_variogram) {
        delete m_variogram;
    }
    m_variogram = vargram;
    configureKriging(err);
}

double OrdinaryKriging::interpolate(const TVector<double> &xstar) {
    for(int i = 0; i < m_N; i++) {
        double dist = l2dist(inputData()[i], xstar);
        m_vStar(i) = m_variogram->value(dist);
    }

    m_vStar(m_N) = 1.0;
    m_weight = m_factor.solve(m_vStar);

    m_lastEstimate = 0.0;
    for (int i = 0; i < m_N; i++) {
        m_lastEstimate += m_weight(i)*outputData()[i];
    }

    if(estimateErrors()) {
        // Prediction error std
        m_predictionErrorEstimate = 0.0;
        for(int i = 0; i < m_N; i++) {
            m_predictionErrorEstimate += m_weight(i)*m_vStar(i);
        }
        m_predictionErrorEstimate = std::sqrt(std::fabs(m_predictionErrorEstimate));

        // Local error std
        m_localErrorEstimate = 0.0;
        for(int i = 0; i < m_N; i++) {
            m_localErrorEstimate +=
                    m_weight(i)*m_weight(i)
                    *(outputData()[i]-m_lastEstimate)
                    *(outputData()[i]-m_lastEstimate);
        }
        m_localErrorEstimate = std::sqrt(m_localErrorEstimate);

        // Kriging std
        m_krigingStdEstimate = 0.0;
        for(int i = 0; i < m_N; i++) {
            m_krigingStdEstimate += m_weight(i)*m_vStar(i);
        }
        m_krigingStdEstimate += m_weight(m_N);
        m_krigingStdEstimate = std::sqrt(m_krigingStdEstimate);
    }
    return m_lastEstimate;
}

double OrdinaryKriging::localErrorStd()
{
    return m_localErrorEstimate;
}

double OrdinaryKriging::krigingErrorStd()
{
    return m_krigingStdEstimate;
}

double OrdinaryKriging::combinedErrorStd()
{
    return std::sqrt(m_localErrorEstimate*m_krigingStdEstimate);
}

double OrdinaryKriging::errorEstimate()
{
    return combinedErrorStd();
}

TVector<TVector<double>> OrdinaryKriging::inputData() const
{
    return m_variogram->inputData();
}

TVector<double> OrdinaryKriging::outputData() const
{
    return m_variogram->outputData();
}

KrigingVariogram* OrdinaryKriging::variogram() const
{
    return m_variogram;
}

void OrdinaryKriging::configureKriging(const TVector<double> &err)
{
    m_N = inputData().size();
    m_xDim = inputData()[0].size();
    defineEstimateErrors(false);

    m_dStar.resize(m_N+1);
    m_vStar.resize(m_N+1);
    m_V.resize(m_N+1, m_N+1);

    m_weight.resize(m_N+1);

    for(int i = 0; i < m_N; i++) {
        for(int j = i; j < m_N; j++) {
            double dist = l2dist(inputData()[i], inputData()[j]);
            m_V(i, j) = m_variogram->value(dist);
            m_V(j, i) = m_V(i, j);
        }
        m_V(i, m_N) = 1.0;
        m_V(m_N, i) = 1.0;
    }

    m_V(m_N, m_N) = 0.0;
    if(err.size()>0) {
        for(int i = 0; i < m_N; i++) {
            m_V(i, i) -= std::pow(err[i], 2);
        }
    }

    m_factor = m_V.colPivHouseholderQr();
}

} // namespace Tigon
