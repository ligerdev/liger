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
#ifndef TIGONUTILS_H
#define TIGONUTILS_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Representation/Elements/IElement.h>

#include <numeric>
#include <algorithm>

#include <boost/math/special_functions/pow.hpp>
#include <boost/range/combine.hpp>
using namespace boost::math;

namespace Tigon{
namespace Representation{
class ISet;
}
}
using namespace Tigon::Representation;

namespace Tigon {

LIGER_TIGON_EXPORT bool areDoublesEqual(double n1, double n2);

LIGER_TIGON_EXPORT
bool areVectorsEqual(const TVector<double>& a, const TVector<double>& b);

template <typename T>
void clearPointerVector(TVector<T> &c)
{
    std::for_each(c.begin(), c.end(), [](const T& elem) {
        delete elem;
    });

    c.clear();
}

// Return the sign of val
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// This function accepts negative or positive indices
// and returns a positive index within the range of 'size'.
// An index of -1 refers to the last element etc.
// NOTE this function will always return a valid index within
// the range, even if idx>size (idx mod size is the result).
inline int indexCycle(int idx, int size)
{
    return ((size + sgn<int>(idx)*(std::abs(idx) % size)) % size);
}

// Checks if the idx (positive, or negative) is within the TOROIDAL range
// of 'size'.
inline bool isInTRange(int idx, int size)
{
    return (((idx<size && idx>=0) || (std::abs(idx)<=size && idx<0)) ? true : false);
}

// Checks if the idx (positive, or negative) is within the range
// of 'size'.
inline bool isInRange(int idx, int size)
{
    return ((idx<size && idx>=0) ? true : false);
}

// Checks if val is within the interval [lb ub]
inline bool isInInterval(double val, double lb, double ub)
{
    return ((val>=lb && val<=ub) ? true : false);
}

// Sort a TVector and return the sorted indices in a separate TVector
template <typename T>
TVector<int> indSort(const TVector<T>& v, bool isAscendingOrder=true)
{
    // initialize original index locations
    TVector<int> idx(v.size());
    std::iota(idx.begin(), idx.end(), 0);

    // sort indexes based on comparing values in v
    if(isAscendingOrder) {
        std::sort(idx.begin(), idx.end(),
             [&v](int i1, int i2) {return v[i1] < v[i2];});
    }
    else {
        std::sort(idx.begin(), idx.end(),
             [&v](int i1, int i2) {return v[i1] > v[i2];});
    }

    return idx;
}

template <typename T>
TVector<T> shuffle(TVector<T> vec)
{
    TVector<T> shuffledVec;
    int s = static_cast<int>(vec.size());
    for(int i=0; i<s; i++) {
        int randInd = TRAND.randInt(s-i);
        shuffledVec.push_back(vec.at(randInd));
        vec.erase(vec.begin()+randInd);
    }
    return shuffledVec;
}

// Convolution of two vectors
template <typename T> TVector<T> conv(const TVector<T>& a, const TVector<T>& b)
{
    // size of convolved vector
    int cSize = a.size() + b.size() - 1;
    TVector<T> c(cSize,0);

    for (int i=0; i<cSize; i++) {
        int ia = std::min(i , static_cast<int>(a.size()-1));
        int ib = std::max(0 , static_cast<int>(i-a.size()+1));
        while(ia>=0 && ib<b.size()) {
            c[i] += a[ia--]*b[ib++];
        }
    }
    return c;
}

/*
 * The p-norm of the difference between two vectors
 * */
template<typename T>
inline double lpdist(const TVector<T>& x1, const TVector<T>& x2, double p)
{
    if(!(p>0.0)) {
        return Tigon::Highest;
    }

    if(x1.size() != x2.size() ) {
        return Tigon::Highest;
    }

    TVector<double> aux;
    aux.reserve(x1.size());
    std::transform(x1.begin(), x1.end(), x2.begin(), std::back_inserter(aux),
                   [p](const T& e1, const T& e2){
                    return std::pow(std::abs(e1 - e2),p);
                   });
    return std::pow(std::accumulate(aux.begin(), aux.end(), 0.0), 1.0/p);
}

/*
 * The p-norm of the difference between two vectors
 * */
template<>
inline double lpdist(const TVector<IElementSPtr>& x1,
              const TVector<IElementSPtr>& x2, double p)
{
    if(!(p>0.0)) {
        return Tigon::Highest;
    }

    if(x1.size() != x2.size() ) {
        return Tigon::Highest;
    }

    TVector<double> aux;
    aux.reserve(x1.size());
    std::transform(x1.begin(), x1.end(), x2.begin(), std::back_inserter(aux),
          [p](IElementSPtr e1, IElementSPtr e2){
           return std::pow(std::abs(e1->value<double>()-e2->value<double>()),p);
          });
    return std::pow(std::accumulate(aux.begin(), aux.end(), 0.0), 1.0/p);
}

// Euclidean distance from one vector, x1, to another, x2.
template <typename T> double l2dist(const TVector<T>& x1, const TVector<T>& x2)
{
    return lpdist(x1,x2,2.0);
}

inline double l2dist(TCVectorReal x1, TCVectorReal x2)
{
    if(x1.rows() != x2.rows()) {
        return Tigon::Highest;
    }

    int64 dim = x1.size();
    double d=0.0;
    for(int i=0; i < dim; i++) {
        d += pow<2>(x1(i)-x2(i));
    }
    return std::sqrt(d);
}

inline double l2dist(TCVectorReal x1, const TVector<double>& x2)
{
    if(x1.rows() != static_cast<int>(x2.size())) {
        return Tigon::Highest;
    }

    int64 dim = x1.size();
    double d=0.0;
    for(int i = 0; i < dim; i++) {
        d += pow<2>(x1(i)-x2[i]);
    }
    return std::sqrt(d);
}

template <typename T>
T vectorMin(const TVector<T>& v)
{
    return *std::min_element(v.begin(), v.end());
}

template <typename T>
T vectorMax(const TVector<T>& v)
{
    return *std::max_element(v.begin(), v.end());
}

template <typename T>
double vectorMean(const TVector<T>& v)
{
    double mean = std::accumulate(v.begin(),v.end(), 0.0);
    return mean/(double)v.size();
}

template <typename T>
double vectorStd(const TVector<T>& v)
{
    double mean = vectorMean(v);
    std::vector<double> diff(v.size());
    std::transform(v.begin(), v.end(), diff.begin(),
                   [mean](double x) { return x - mean; });
    double sq_sum = std::inner_product(diff.begin(), diff.end(),
                                       diff.begin(), 0.0);
    return std::sqrt(sq_sum/(double)v.size());
}


// Determines the angle (in radians) between two vectors with origin on zero.
// The inverse cosine (acos(x)) for x in [-1,1] ranges from pi to 0.
// Given that the maximum angle between two vectors is pi/2 (or 90 degrees),
// this function output ranges from pi/2 to 0
template <typename T>
double angleDistance(const TVector<T>& x1, const TVector<T>& x2)
{
    if(x1.size() != x2.size()) {
        return Tigon::Highest;
    }

    double innerProduct = std::inner_product(x1.begin(), x1.end(), x2.begin(), 0.0);
    double x1Modulus = std::accumulate(x1.begin(), x1.end(), 0.0,
                        [](double sum, const T& elem){return sum + elem*elem;});
    double x2Modulus = std::accumulate(x2.begin(), x2.end(), 0.0,
                        [](double sum, const T& elem){return sum + elem*elem;});

    x1Modulus = std::sqrt(x1Modulus);
    x2Modulus = std::sqrt(x2Modulus);

    return std::acos(innerProduct/(x1Modulus*x2Modulus));
}

template <typename T>
TVector<T> linspace(T a, T b, size_t N)
{
    T h = (b - a) / static_cast<T>(N-1);
    TVector<T> xs(N);
    typename TVector<T>::iterator x;
    T val;
    for (x = xs.begin(), val = a; x != xs.end(); ++x, val += h)
        *x = val;
    return xs;
}

template <typename T>
TVector<T> sumColumnsDoubleVector(const TVector<TVector<T>>& data)
{
    TVector<T> colsums(data[0].size());
    std::for_each(data.begin(), data.end(),
                  [&](const TVector<T>& row)
                  {
                        std::transform(row.begin(), row.end(), colsums.begin(),
                                       colsums.begin(),
                                       [](T d1, T d2){return d1+d2;});
                  });

    return colsums;
}

template <typename T>
T scaleRange(T x, T oldMin, T oldMax, T newMin, T newMax)
{
    if( (oldMin>oldMax) || (newMin>newMax)) {
        return -1; // ERROR
    }
    T res = ((x-oldMin)*(newMax-newMin))/(oldMax-oldMin) + newMin;
    return res;
}

LIGER_TIGON_EXPORT
TVector<double> timePreferenceWeights(int n, double alpha);

LIGER_TIGON_EXPORT
int generateRandomNumberTimePreference(int nRuns, double alpha);

LIGER_TIGON_EXPORT
int TTP_hash(const TVector<int>& variables);

LIGER_TIGON_EXPORT
Tigon::Representation::ISet* setsUnion(TVector<Tigon::Representation::ISet*> sets);

// An unevaluated IMapping is not considered a duplicate of an evaluated
// IMapping even if they have identical decision vector (they are imcomparable)
LIGER_TIGON_EXPORT TVector<IMappingSPtr> uniqueSet(TVector<IMappingSPtr> set);
LIGER_TIGON_EXPORT ISet* uniqueSet(ISet* set);

LIGER_TIGON_EXPORT
TVector<TVector<int> > latinHypercubeSampling(int nDims, int nSamps);
LIGER_TIGON_EXPORT TVector <int> randomPermutationSampling(int nSamps);

LIGER_TIGON_EXPORT bool isGoalDefined(IElementSPtr           goal);
LIGER_TIGON_EXPORT bool isThresholdDefined(IElementSPtr threshold);

LIGER_TIGON_EXPORT std::ostream&  operator<< (std::ostream& os, ISet * const pop);

LIGER_TIGON_EXPORT double truncateValue(double value, ElementType type,
                                        const IElement& ub, const IElement& lb);
/*
 * Estimates the crowding distance of each row vector from the input set.
 * The crowding distance formulation is taken from page 236 of the book:
 * K. Deb, Multi-Objective Optimization using Evolutionary Algorithms,
 * John Wiley \& Sons, 2001.
 * */
LIGER_TIGON_EXPORT
TVector<double> crowdingDist(const TVector<TVector<double>>& set);

/*
 * Returns an ordered set of indices that correspond to solutions of a
 * population. The indices are ordered based on the distance between the
 * solutions a set of vectors from dataVec.
 * The vectors are first ordered based on their distance to the centroid. From
 * the closest to the farthest, each vector is assigned to the closest solution,
 * without allowing the same solution to be assigned to more than one vector.
 * */
LIGER_TIGON_EXPORT
TVector<int> centroidDistanceOrder(
        const TVector<TVector<double>>& dataVec,
        const TVector<IMappingSPtr> &population,
        Tigon::DistanceMeasure distMeasure=Tigon::AngleDistance);


/**
 * @brief uniqueCellIdentificationNumber
 * @param sol: M-dimensional point
 * @param nb: number of bins used to discretize the M-dimensional space
 * @return generates an unique id (number) for a given M-dimensional point (sol),
 * according to the number of bins (nb) provided.
 * This function discretizes the n-dimensional space according to the given
 * number of bins (nb), and identifies the cell where the point (sol) exists in.
 * Each cell has an unique id (number) that ranges from 0 to nb^M-1.
 * PRE: Each dimensions of the point (sol) needs to lie on the interval [0,1],
 * otherwise the value of -1 is returned.
 */
LIGER_TIGON_EXPORT
int uniqueCellIdentificationNumber(const TVector<double> &sol, int nb);

/**
 * @brief binaryRandVector
 * @param n  vector dimensions
 * @return a binary n-dimensional vector, whose values are generated randomly
 */
LIGER_TIGON_EXPORT
TVector<int> binaryRandVector(int n);

/**
 * @brief binaryRandVectorConstrained
 *         generates a binary n-dimensional vector denoted by b=(b1,...,bn)^T
 *         the elements are binary random variables, i.e., bi\in{0,1} for all i
 *         the elements values are chosen such that
 *          1) the \sum b^Tw is maximum and
 *          2) \sum b^Tw < W
 * @param w  real-valued vector w=(w1,...wn)^T
 * @param W  constrain value
 * @return a binary n-dimensional vector whose values are generated randomly
 */
LIGER_TIGON_EXPORT
TVector<int> binaryRandVectorConstrained(const TVector<double>& w, double W);

} // namespace Tigon
#endif // TIGONUTILS_H
