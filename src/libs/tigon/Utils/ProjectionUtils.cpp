/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Utils/ProjectionUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
ProjectedVec gDChebyshevProject(TVector<double> dir,
                                const TVector<double> f)
{
    ProjectedVec ret;
    int k = f.size();
    if(dir.size() == k) {

        // avoid division by zero, and make sure the direction is normalised
        for(int i=0; i<k; i++) {
            dir[i] += Tigon::Epsilon;
        }
        toUnitVec(dir, 1.0);

        // gD optimal weighting vector
        TVector<double> w(k);
        for(int i=0; i<k; i++) {
            w[i] = 1.0 / dir[i];
        }
        toUnitVec(w, 1.0);

        // calculate scalarising function
        ret.magnitude = weightedChebyshev(w, f);
        ret.magnitude /= weightedChebyshev(w, dir);

        // scale projected vector
        ret.vec = dir;
        scale(ret.vec,ret.magnitude);
    }
    return ret;
}

ProjectedVec gDWeightedSumProject(TVector<double> dir,
                                  const TVector<double> f)
{
    ProjectedVec ret;
    int k = f.size();
    if(dir.size() == k) {

        // avoid division by zero, and make sure the direction is normalised
        for(int i=0; i<k; i++) {
            dir[i] += Tigon::Epsilon;
        }
        toUnitVec(dir, 1.0);

        // gD optimal weighting vector
        TVector<double> w(k);
        for(int i=0; i<k; i++) {
            w[i] = 1.0 / dir[i];
        }
        toUnitVec(w, 1.0);

        // calculate scalarising function
        ret.magnitude = weightedSum(w, f);
        ret.magnitude /= weightedSum(w, dir);

        // scale projected vector
        ret.vec = dir;
        scale(ret.vec,ret.magnitude);
    }
    return ret;
}

ProjectedVec gDWeightedLpProject(TVector<double> dir,
                                 const TVector<double> f,
                                 double p)
{
    ProjectedVec ret;
    int k = f.size();
    if(dir.size() == k) {

        // avoid division by zero, and make sure the direction is normalised
        for(int i=0; i<k; i++) {
            dir[i] += Tigon::Epsilon;
        }
        toUnitVec(dir, 1.0);

        // gD optimal weighting vector
        TVector<double> w(k);
        for(int i=0; i<k; i++) {
            w[i] = 1.0 / dir[i];
        }
        toUnitVec(w, 1.0);

        // calculate scalarising function
        ret.magnitude = weightedLp(w, f, p);
        ret.magnitude /= weightedLp(w, dir, p);

        // scale projected vector
        ret.vec = dir;
        scale(ret.vec,ret.magnitude);
    }
    return ret;
}

} // namespace Tigon
