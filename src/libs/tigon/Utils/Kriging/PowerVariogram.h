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
#ifndef POWERVARIOGRAM_H
#define POWERVARIOGRAM_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>

namespace Tigon {

class LIGER_TIGON_EXPORT PowerVariogram : public KrigingVariogram
{
public:
    PowerVariogram(const TVector<TVector<double>>& x, const TVector<double>& y);
    PowerVariogram(const TVector<TVector<double>>& x, const TVector<double>& y,
                   double eta, double nug=0.0);
    ~PowerVariogram();    

    double value(const double h) const;
    void  fitVariogram();

    void  fitVariogramLS(const TVector<TVector<double>>& x,
                         const TVector<double>& y);

    void  assignEta(double val);
    double eta()  const;
    double beta() const;

private:
    bool  dataCheckPassed() const;
    void  updateBeta();
    double m_beta;
    double m_eta;
};

}

#endif // POWERVARIOGRAM_H
