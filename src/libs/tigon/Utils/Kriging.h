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
#ifndef KRIGING_H
#define KRIGING_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/KrigingVariogram.h>
DISABLE_WARNINGS
#include <eigen/Eigen>
ENABLE_WARNINGS

namespace Tigon {
class KrigingVariogram;

class LIGER_TIGON_EXPORT Kriging
{
public:
    Kriging();
    Kriging(KrigingVariogram* vargram, const TVector<double> &err = TVector<double>());
    ~Kriging();

    /// Variogram and interpolation
    void  assignVariogram(KrigingVariogram* vargram, const TVector<double> &err = TVector<double>());
    KrigingVariogram* variogram() const;
    double interpolate(const TVector<double> & xstar);

    /// Error standard deviations
    void  estimateErrors(bool est);

    // Square root of weighted average of the differences
    // between the estimate and data values, f(x*) and f(x_i),
    // capturing the local variation of the data values
    double localErrorStd();

    // Square root of weighted average of the variograms
    // between the target and other data point,
    // giving distance/location information from surrounding data to the target,
    // from x_i to x*
    double krigingErrorStd();

    // Square root of the localErrorStd*krigingErrorStd,
    // capturing both local variation of data values and the spatial data
    // configuration around the target.
    double combinedErrorStd();

    // By dafault, combined error std is returned.
    double errorEstimate();

private:
    void configureKriging(const TVector<double> &err);

    int  m_xDim;
    int  m_N;

    bool  m_estimateErrors;
    double m_lastEstimate;
    double m_predictionErrorEstimate;
    double m_localErrorEstimate;
    double m_krigingStdEstimate;

    TCVectorReal m_dStar;
    TCVectorReal m_vStar;
    TCVectorReal m_weight;
    TMatrixReal  m_V;
    Eigen::ColPivHouseholderQR<TMatrixReal> m_factor;

    KrigingVariogram* m_variogram;
};

} // namespace Tigon

#endif // KRIGING_H
