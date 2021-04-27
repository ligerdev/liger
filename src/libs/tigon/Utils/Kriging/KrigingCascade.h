/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef KRIGINGCASCADE_H
#define KRIGINGCASCADE_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <eigen/Eigen>

namespace Tigon {
class OrdinaryKriging;
class KrigingVariogram;

class LIGER_TIGON_EXPORT KrigingCascade
{
public:
    KrigingCascade();
    KrigingCascade(const TVector<TVector<double>>& XX,
                   const TVector<TVector<double>>& YY, TString basis);
    KrigingCascade(const TVector<KrigingVariogram*>& variograms);
    ~KrigingCascade();

    TVector<double> interpolate(TVector<double> x);
    TVector<double> errors();
    TVector<OrdinaryKriging*> interpolators();
    void append(OrdinaryKriging* interpolator);
    void clear();
    void fitVariogram();

    void enableErrorEstimates();
    void disableErrorEstimates();

private:
    int  m_xDim;
    bool m_produceErrorEstimates;
    TVector<double>    m_errors;
    TVector<OrdinaryKriging*> m_interpolators;
};

} // namespace Tigon

#endif // KRIGINGCASCADE_H
