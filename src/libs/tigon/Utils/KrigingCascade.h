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
#ifndef KRIGINGCASCADE_H
#define KRIGINGCASCADE_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
DISABLE_WARNINGS
#include <eigen/Eigen>
ENABLE_WARNINGS


namespace Tigon {
class Kriging;
class KrigingVariogram;

class LIGER_TIGON_EXPORT KrigingCascade
{
public:
    KrigingCascade();
    KrigingCascade(TVector<TVector<double> > XX,
                           TVector<TVector<double> > YY,
                           TString basis);
    KrigingCascade(TVector<KrigingVariogram*> variograms);
    ~KrigingCascade();

    TVector<double> interpolate(TVector<double> x);
    TVector<double> errors();
    TVector<Kriging*> interpolators();
    void append(Kriging* interpolator);
    void clear();
    void fitVariogram();

    void enableErrorEstimates();
    void disableErrorEstimates();

private:
    int  m_xDim;
    bool m_produceErrorEstimates;
    TVector<double>    m_errors;
    TVector<Kriging*> m_interpolators;
};

} // namespace Tigon

#endif // KRIGINGCASCADE_H
