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
#ifndef KRIGINGVARIOGRAM_H
#define KRIGINGVARIOGRAM_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {

class LIGER_TIGON_EXPORT KrigingVariogram
{
public:
    KrigingVariogram();
    KrigingVariogram(double nug, double cparam = 1, double alpha = 1);
    virtual ~KrigingVariogram();

    /// Virtual functions
    virtual double value(const double h) const = 0;
    virtual void  fitVariogram() = 0;

    /// Power variogram related
    virtual void  assignEta(double val);
    virtual double eta()  const;
    virtual double beta() const;

    /// Empirical variograms
    void calculateEmpiricalVariogram(TCVectorReal dist, TCVectorReal Zx);
    void calculateEmpiricalVariogram(TVector<TVector<double> > x, TVector<double> y);
    TCVectorReal empiricalVariograms() const;
    TCVectorReal empiricalLags() const;

    /// General functions
    void assignAlpha(double val);
    void assignNugget(double val);
    void assignC(double val);
    void assignVariogramData(TVector<TVector<double> > x, TVector<double> y);
    double alpha()   const;
    double paramC()  const;
    double nugget()  const;
    double sill()    const;
    int   nInputs() const;
    TVector<TVector<double> > inputData()  const;
    TVector<double>           outputData() const;

    /// Utilities
    TCVectorReal interiorPointSolver(TMatrixReal Q, TCVectorReal c,
                                       TMatrixReal A, TCVectorReal b);
private:
    double m_alpha;
    double m_c0;
    double m_c;
    TCVectorReal m_empiricalVariograms;
    TCVectorReal m_empiricalLags;
    TVector<TVector<double> > m_x;
    TVector<double>           m_y;
};

} // namespace Tigon

#endif // KRIGINGVARIOGRAM_H
