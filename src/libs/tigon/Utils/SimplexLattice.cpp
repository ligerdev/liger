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
#include <tigon/Utils/SimplexLattice.h>
#include <boost/math/special_functions/factorials.hpp>
#include <numeric>

using namespace boost::math;

namespace Tigon {

int simplexLatticeSize(int h, int k) {

    if(k<2) {
        return h;
    }

    if(k==2) {
        return h+1;
    }

    int ratio=h+1;

    TVector <int> p(ratio);

    for(int i=0; i<ratio; i++) {
        p[i] = i+1;
    }

    for(int j=0; j<(k-3); j++) {
        for(int i=0; i<(ratio-1); i++) {
            p[i+1] += p[i];
        }
    }

    int res = 0;
    res = std::accumulate(p.begin(), p.end(), 0);

    return res;
}

TVector<TVector<double> > simplexLatticeRecursive(int h, int k, double s)
{
    TVector<TVector<double> > wSet;
    //terminating criterion
    if(k==1) {
        wSet = TVector<TVector<double> >(1, TVector<double>(1, s));
    } else {
        int nW = std::round(factorial<double>(h+k-1) /
                factorial<double>(k-1) / factorial<double>(h));
        wSet = TVector<TVector<double> >(nW,TVector<double>(k, 0.0));
        int idx = 0;
        double w=0.0;
        for(int i=0; i<h+1; i++) {
            TVector<TVector<double> > kMinusOneSimplex
                    = simplexLatticeRecursive(h-i, k-1, s-w);
            int sSize = kMinusOneSimplex.size();
            for(int j=0; j<sSize; j++) {
                TVector<double> ww(1, w);
                ww << kMinusOneSimplex[j];
                wSet[idx++].swap(ww);
            }
            w += s/h;
        }
    }
    return wSet;
}

TVector<TVector<double>> simplexLatticeIterative(int h, int k)
{
    TVector <int> counter(k-1,0);
    TVector <int> limit(k-1);

    int npoints = simplexLatticeSize(h,k);
    TVector<TVector<double> > res(npoints); // npoints x k

    for(int i=0; i<npoints; i++) {
        int position = h+1;

        res[i].reserve(k);
        for(int j=0; j<(k-1); j++) {
            // append the weight vector for dimensions m=1..M-1
            res[i].push_back((double)counter[j]/(double)h);

            // determine the last position
            position -= counter[j];

            // recalculate the limits
            int sum = std::accumulate(counter.cbegin(), counter.cbegin()+j, 0);
            limit[j] = h - sum;
        }
        // determine the weight vector for dimension m=M
        res[i].push_back(((double)position-1.0)/(double)h);

        // move to next weight vector
        for(int j=(k-1)-1; j>=0; j--) {
            if(counter[j] < limit[j]) {
                counter[j] += 1;
                break;
            } else {
                counter[j] = 0;
            }
        }
    }

    return res;
}

TVector<TVector<double> > constrainedSimplexLattice(
        int h, int k, const TVector<double>& upperBound)
{
    TVector <int> counter(k-1,0);
    TVector <int> limit(k-1);
    TVector <double> temp(k);
    bool valid = true;

    int npoints = simplexLatticeSize(h,k);
    TVector<TVector<double> > res; // requiredNumber x k
    res.reserve(npoints);

    int cnt=0;
    for(int i=0; i<npoints; i++) {
        int position = h+1;
        valid = true;

        for(int j=0; j<(k-1); j++) {
            // append the weight vector for dimensions m=1..M-1
            temp[j] = (double)counter[j]/(double)h;
            if(temp[j]>upperBound[j]) {
                valid=false;
                break;
            }

            // determine the last position
            position -= counter[j];

            // recalculate the limits
            int sum = std::accumulate(counter.cbegin(), counter.cbegin()+j, 0);
            limit[j] = h - sum;
        }
        // determine the weight vector for dimension m=M
        if(valid==true) {
            temp[k-1] = ((double)position-1.0)/(double)h;
            if(temp[k-1]>upperBound[k-1]) {
                valid=false;
            }

            if(valid==true) {
                res.push_back(temp);
                cnt++;
            }
        }

        // move to next weight vector
        for(int j=(k-1)-1; j>=0; j--) {
            if(counter[j] < limit[j]) {
                counter[j] += 1;
                break;
            } else {
                counter[j] = 0;
            }
        }
    }

    return res;
}

TVector<TVector<double>>
preferredSimplexLattice(int h, int k, const TVector<double> &goalVec)
{
    TVector<TVector<double>> wPref;
    int completeLatticeSetSize = simplexLatticeSize(h, k);

    double goalVecSum = std::accumulate(goalVec.begin(), goalVec.end(), 0.0);

    if(areDoublesEqual(goalVecSum, 1.0)) {
        wPref.push_back(goalVec);

    } else if(goalVecSum < 1.0) {

        // goal vector below the simplex
        wPref = simplexLatticeIterative(h, k);
        simplexLatticeLinearTransformation(wPref, goalVec);

    } else {

        // goal vector on top of the simplex
        TVector<double> upperBound = goalVec;
        std::transform(upperBound.begin(), upperBound.end(),
                       upperBound.begin(),
                       std::bind1st(std::multiplies<double>(),
                                    1.0/(goalVecSum-1.0)));
        int currentSetSize = 0;
        int originalH=h;

        do {

            wPref = constrainedSimplexLattice(h++, k, upperBound);
            simplexLatticeLinearTransformation(wPref, goalVec);
            currentSetSize = wPref.size();

            /// Prevents a never ending increase of h.
            /// Such situation can arise if the goal vector is over the
            /// simplex lattice or over a very narrow region. Implying that
            /// the currentSetSize is never "equal to" or "bigger than" the
            /// completeLatticeSetSize.
            if(simplexLatticeSize(h,k)>
                    CONSTANTS::MAX_COMPLETE_SIMPLEX_LATTICE_SIZE) {
                //qDebug() << "Reset the weight vectors!!!";
                return simplexLatticeIterative(originalH, k);
            }


        } while (currentSetSize < completeLatticeSetSize);
    }

    return wPref;
}

TVector<TVector<double>>
preferredSimplexLatticeCrowding(int h, int k, const TVector<double> &goalVec,
                                int maxNumberWeightVector)
{
    TVector<TVector<double>> wPref;
    int completeLatticeSetSize = simplexLatticeSize(h, k);

    double goalVecSum = std::accumulate(goalVec.begin(), goalVec.end(), 0.0);

    if(areDoublesEqual(goalVecSum, 1.0)) {
        wPref.push_back(goalVec);

    } else if(goalVecSum < 1.0) {

        // goal vector below the simplex
        wPref = simplexLatticeIterative(h, k);
        simplexLatticeLinearTransformation(wPref, goalVec);

    } else {

        // goal vector on top of the simplex
        TVector<double> upperBound = goalVec;
        std::transform(upperBound.begin(), upperBound.end(),
                       upperBound.begin(),
                       std::bind1st(std::multiplies<double>(),
                                    1.0/(goalVecSum-1.0)));
        int currentSetSize = 0;
        int originalH=h;

        do {

            wPref = constrainedSimplexLattice(h++, k, upperBound);
            simplexLatticeLinearTransformation(wPref, goalVec);
            currentSetSize = wPref.size();

            /// Prevents a never ending increase of h.
            /// Such situation can arise if the goal vector is over the
            /// simplex lattice or over a very narrow region. Implying that
            /// the currentSetSize is never "equal to" or "bigger than" the
            /// completeLatticeSetSize.
            if(simplexLatticeSize(h,k)>
                    CONSTANTS::MAX_COMPLETE_SIMPLEX_LATTICE_SIZE) {
                //qDebug() << "Reset the weight vectors!!!";
                wPref = simplexLatticeIterative(originalH, k);
                break;
            }


        } while (currentSetSize < completeLatticeSetSize);
    }

    if(wPref.size() > maxNumberWeightVector) {

        int currentsize = wPref.size();

        do {

            TVector<double> cd = crowdingDist(wPref);
            TVector<int> indices = Tigon::indSort(cd);

            wPref.erase(wPref.begin() + indices[indices.size()-1]);
            currentsize--;

        } while( currentsize > maxNumberWeightVector );

    }

    return wPref;
}

TVector<TVector<double>>
preferredSimplexLatticeRedundantComponents(
        int h, int k, const TVector<double> &goalVec,
        const TVector<bool> &componentsSubset)
{
    // construct goalSubset based on a reduced set of components in
    // componentsSubset
    TVector<double> goalSubset;
    for(int i=0; i<k; i++) {
        if(componentsSubset[i]) {
            goalSubset.push_back(goalVec[i]);
        }
    }

    // generate a simplex lattice for the reduced set of components based on
    // the goal vector subset
    int nSelectedComponents = vectorCount(componentsSubset, true);
    TVector<TVector<double>> wSubset =
            preferredSimplexLattice(h, nSelectedComponents, goalSubset);
    int newSize = wSubset.size();

    // construct the complete simplex lattice where the redundant components
    // have value 0.
    TVector<TVector<double>> w(newSize, TVector<double>(k, 0.0));
    for(int i=0; i<newSize; i++) {
        int p=0;
        for(int j=0; j<k; j++) {
            if(componentsSubset[j]) {
                w[i][j] = wSubset[i][p];
                p++;
            }
        }
    }
    return w;
}

void simplexLatticeLinearTransformation(TVector<TVector<double> > &set,
                                        const TVector<double> &goalVec)
{
    double sum = std::accumulate(goalVec.begin(), goalVec.end(), 0.0);

    double scaleFactor = 1.0 - sum;
    for(int i=0; i<set.size(); i++) {
        for(int j=0; j<goalVec.size(); j++) {
            set[i][j] *= scaleFactor;
            set[i][j] += goalVec[j];
        }
    }

    return;
}

} // namespace Tigon
