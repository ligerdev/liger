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
#include <tigon/Utils/Kriging.h>
#include <tigon/Utils/TigonUtils.h>


namespace Tigon {

Kriging::Kriging()
{
    // Default constructor
    m_estimateErrors = false;
    m_xDim = 0;
    m_N = 0;
    m_lastEstimate = 0;
    m_predictionErrorEstimate = 0;
    m_localErrorEstimate = 0;
    m_krigingStdEstimate = 0;
    m_variogram = 0;
}

Kriging::Kriging(KrigingVariogram* vargram, const TVector<double> &err)
{
    m_estimateErrors = false;
    m_xDim = 0;
    m_N = 0;
    m_lastEstimate = 0;
    m_predictionErrorEstimate = 0;
    m_localErrorEstimate = 0;
    m_krigingStdEstimate = 0;
    m_variogram = 0;

    assignVariogram(vargram, err);
}

Kriging::~Kriging()
{
    if(m_variogram) {
        delete m_variogram;
    }
}

void Kriging::assignVariogram(KrigingVariogram* vargram, const TVector<double> &err)
{
    if(m_variogram) {
        delete m_variogram;
    }
    m_variogram = vargram;
    configureKriging(err);
}

double Kriging::interpolate(const TVector<double> &xstar) {
    for(int i = 0; i < m_N; i++) {
        m_vStar(i) = m_variogram->value(l2dist(m_variogram->inputData()[i], xstar));
    }

    m_vStar(m_N) = 1.0;
    m_weight = m_factor.solve(m_vStar);

    m_lastEstimate = 0.0;
    for (int i = 0; i < m_N; i++) {
        m_lastEstimate += m_weight(i)*m_variogram->outputData()[i];
    }

    if(m_estimateErrors) {
        // Prediction error std
        m_predictionErrorEstimate = 0.0;
        for(int i = 0; i < m_N; i++) {
            m_predictionErrorEstimate += m_weight(i)*m_vStar(i);
        }
        m_predictionErrorEstimate = std::sqrt(std::abs(m_predictionErrorEstimate));

        // Local error std
        m_localErrorEstimate = 0.0;
        for(int i = 0; i < m_N; i++) {
            m_localErrorEstimate +=
                    m_weight(i)*m_weight(i)
                    *(m_variogram->outputData()[i]-m_lastEstimate)
                    *(m_variogram->outputData()[i]-m_lastEstimate);
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

void  Kriging::estimateErrors(bool est)
{
    m_estimateErrors = est;
}

double Kriging::localErrorStd()
{
    return m_localErrorEstimate;
}

double Kriging::krigingErrorStd()
{
    return m_krigingStdEstimate;
}

double Kriging::combinedErrorStd()
{
    return std::sqrt(m_localErrorEstimate*m_krigingStdEstimate);
}

double Kriging::errorEstimate()
{
    return combinedErrorStd();
}

KrigingVariogram* Kriging::variogram() const
{
    return m_variogram;
}

void Kriging::configureKriging(const TVector<double> &err)
{
    m_N = m_variogram->inputData().size();
    m_xDim = m_variogram->inputData()[0].size();
    m_estimateErrors = false;

    m_dStar.resize(m_N+1);
    m_vStar.resize(m_N+1);
    m_V.resize(m_N+1, m_N+1);

    m_weight.resize(m_N+1);

    for(int i = 0; i < m_N; i++) {
        for(int j = i; j < m_N; j++) {
            m_V(i, j) = m_variogram->value(l2dist(m_variogram->inputData()[i],
                                                  m_variogram->inputData()[j]));
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
