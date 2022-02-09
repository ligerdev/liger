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
#include <tigon/Utils/HypervolumeUtils.h>
#include <tigon/Utils/Hypervolume/hv.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>

namespace Tigon {

double hypervolume(ISet* set)
{
    int n = set->size();
    if(n==0) {
        return 0.0;
    }
    TVector<TVector<double> > vectorsSet;
    for(int i=0; i<n; i++) {
        TVector<double> vec = set->at(i)->doubleObjectiveVec();
        vectorsSet.push_back(vec);
    }

    return hypervolume(vectorsSet);
}

double hypervolume(ISet* set, const TVector<double> ref)
{
    int n = set->size();
    if(n==0) {
        return 0.0;
    }
    TVector<TVector<double> > vectorsSet;
    for(int i=0; i<n; i++) {
        TVector<double> vec = set->at(i)->doubleObjectiveVec();
        vectorsSet.push_back(vec);
    }

    return hypervolume(vectorsSet, ref);
}

double hypervolume(TVector<TVector<double > > set)
{
    // reference point is the maximum in every dimension
    int n = set.size();
    if(n==0) {
        return 0.0;
    }
    int d = set[0].size();
    TVector<double> refPoint(d, Tigon::Lowest);
    for(int i=0; i<n; i++) {
        TVector<double> vec = set[i];
        for(int j=0; j<d; j++) {
            refPoint[j] = std::max(refPoint[j], vec[j]);
        }
    }

    return hypervolume(set, refPoint);
}

double hypervolume(TVector<TVector<double > > set, const TVector<double> ref)
{
    int n = set.size();
    if(n==0) {
        return 0.0;
    }
    int d = set[0].size();
    TVector<double> allVectors;
    for(int i=0; i<n; i++) {
        allVectors << set[i];
    }

    double* allVecPtr = allVectors.data();
    const double* refPtr = ref.data();

    double ret = fpli_hv(allVecPtr, d, n, refPtr);

    allVectors.clear();
    allVecPtr = 0;
    refPtr = 0;

    return ret;
}

} // namespace Tigon
