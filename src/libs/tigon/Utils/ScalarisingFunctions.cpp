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
#include <tigon/Utils/ScalarisingFunctions.h>

namespace Tigon {
double weightedChebyshev(const TVector<double> &w, const TVector<double> &f)
{
    double m = 0.0;
    size_t sw = w.size();
    size_t sf = f.size();
    if(sw == sf) {
        int counter = 0;
        for(double wi : w) {
            double feval = wi;
            if(feval<Tigon::Epsilon) {
                feval = Tigon::Epsilon;
            }
            m = std::max(m, feval * f[counter]);
            ++counter;
        }
    }
    return m;
}

double weightedChebyshevAugmented(const TVector<double> &w, const TVector<double> &f)
{
    if(w.size() != f.size()) {
        return 0.0;
    }

    double m = 0.0;

    int wsize = w.size();
    for(int i=0; i<wsize; i++) {
        double feval = w[i]; // used to address weakly dominated points due to zero weights
        if(feval<Tigon::Epsilon) {
            feval = Tigon::Epsilon;
        }
        m = std::max(m, feval * f[i]);
        //m = std::max(m, w[i] * f[i]);  // uncomment this line to use original
    }

    double term=0.0;
    for(int i=0; i<wsize; i++) {

        double feval = w[i]; // used to address weakly dominated points due to zero weights
        if(feval < Tigon::Epsilon) {
            feval = Tigon::Epsilon;
        }
        term += feval * f[i];
    }

    m = m + Tigon::AugmentedConstant*term;

    return m;
}

double weightedSum(const TVector<double> &w, const TVector<double> &f)
{
    double sum = 0.0;
    if(w.size() == f.size()) {
        for(size_t i=0; i<w.size(); i++) {
            double feval = w[i]; // used to address weakly dominated points due to zero weights
            if(feval<Tigon::Epsilon) {
                feval = Tigon::Epsilon;
            }
            sum += feval * f[i];
            //sum += w[i] * f[i];   // uncomment this line to use original
        }
    }
    return sum;
}

double weightedLp(const TVector<double> &w, const TVector<double> &f, double p)
{
    double sum = 0.0;
    if(w.size() == f.size() && p > 0.0) {
        for(size_t i=0; i<w.size(); i++) {
            double feval = w[i]; // used to address weakly dominated points due to zero weights
            if(feval<Tigon::Epsilon) {
                feval = Tigon::Epsilon;
            }
            sum += std::pow(feval * f[i], p);
            //sum += std::pow(w[i] * f[i], p);   // uncomment this line to use original
        }
        sum = std::pow(sum, 1.0/p);
    }
    return sum;
}

} // namespace Tigon
