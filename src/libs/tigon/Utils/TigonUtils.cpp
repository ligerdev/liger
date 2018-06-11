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
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Mappings/IMappingOperations.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Utils/NormalisationUtils.h>

#include <algorithm>

using namespace Tigon::Representation;
namespace Tigon {

double truncateValue(double value, ElementType type,
                     const IElement& ub, const IElement& lb)
{
    switch(type) {
    case NominalType:
    {
        int dvalue = (int) value;
        int ubd = ub.value();
        int lbd = lb.value();
        if( (dvalue < lbd) || (dvalue > ubd) ) {
            return TRAND.randInt(ubd - lbd + 1, lbd);
        }
        break;
    }
    case RealType: case IntegerType: case OrdinalType:
    {
        double ubr = ub.value();
        double lbr = lb.value();
        if(value < lbr) {
            return lbr;
        } else if(value > ubr) {
            return ubr;
        }
        break;
    }
    default:
        throw TException("TigonUtils", InvalidTypeException);
    }

    // no need to truncate
    return value;
}

ISet* setsUnion(TVector<ISet*> sets)
{
    TVector<IMappingSPtr> imaps;
    for(int i=0; i<sets.size(); i++) {
        ISet* s = sets[i];
        for(int j=0; j < s->size(); j++) {
            if( !(vectorContains(imaps, s->at(j))) ) {
                imaps.push_back(s->at(j));
            }
        }
    }
    ISet* u = new ISet(imaps);
    return u;
}

TVector<IMappingSPtr> uniqueSet(TVector<IMappingSPtr> set)
{
    TVector<IMappingSPtr> u;
    for(int i=0; i<set.size(); i++) {
        bool duplicate = false;
        for(int j=0; j<u.size(); j++) {
            tribool IMapEqComp = *set[i] == *u[j];
            if(IMapEqComp.value == tribool::true_value) {
                duplicate = true;
                break;
            }
        }
        if(!duplicate) {
            u.push_back(set[i]);
        }
    }
    return u;
}

ISet* uniqueSet(ISet* set)
{
    TVector<IMappingSPtr> u = uniqueSet(set->all());
    ISet* ret = new ISet(u);
    return ret;
}

TVector<TVector<int> > latinHypercubeSampling(int nDims, int nSamps)
{
    // Based on the algorithm in Numerical Recipes
    TVector<int> indVec(nSamps);
    for(int i=0; i<nSamps; i++) {
        indVec[i] = i;
    }
    TVector<TVector<int> > indices(nDims, indVec);

    TVector<TVector<int> > LHS(nSamps, TVector<int>(nDims));
    for(int samp=0; samp<nSamps; samp++) {
        for(int dim=0; dim<nDims; dim++) {
            int idx = TRAND.randInt(nSamps - samp);
            LHS[samp][dim] = indices[dim].at(idx);
            indices[dim].erase(indices[dim].begin()+idx);
        }
    }
    return LHS;
}

TVector <int> randomPermutationSampling(int nSamps)
{
    TVector<int> indVec(nSamps);
    int current = 0;
    std::generate(indVec.begin(), indVec.end(), [&current](){return current++;});
    std::random_shuffle(indVec.begin(), indVec.end(),
                        [](int i) { return TRAND.randInt(i);});
    return indVec;
}

bool isGoalDefined(IElementSPtr goal)
{
    return (*goal > goal->minValue());
}

bool areDoublesEqual(double a, double b)
{
    double c = std::max(std::abs(a),std::abs(b));
    double relDif = (c==0.0) ? 0.0 : std::abs(a-b) / c;
    return relDif < Epsilon;
}

bool isThresholdDefined(IElementSPtr threshold)
{
    return *threshold < threshold->maxValue();
}

std::ostream &operator<<(std::ostream &os, ISet * const pop)
{
    os << "[ ";
    for(int i=0; i< pop->size(); i++) {
        TVector<IElementSPtr> vec = pop->at(i)->objectiveVec();
        for(int j=0; j<vec.size(); j++) {
            os << vec[j]->value<double>() << " ";
        }
        if(i < pop->size() - 1) {
            os << "; ";
        } else {
            os << " ";
        }
    }

    os << "];" << std::endl;
    return os;
}

/*!
 * \brief timePreferenceWeights: the set of weights generated correspond
 * to a time based discount factor used in inter-temporal valuation.
 * Check: D.S.G. POLLOCK: ENVIRONMENTAL ECONOMICS, Lecture 7, Utility,
 * Time Preference and Myopia.
 * \param n: time difference between the current generation and the
 * previous n generations.
 * \param alpha: change in importance between consecutive generations.
 * The higher alpha is, the more importance is given to one generation
 * over the previous generations. The less alpha is, the importance between
 * generations becomes more homogeneous.
 * \return
 */
TVector<double> timePreferenceWeights(int n, double alpha)
{
    TVector<double> res(n);

    int p=0;
    for(int i=n;i>0; i--) {
        res[p] = 1.0/pow(1.0+alpha,i);
        p++;
    }

    return res;
}

int generateRandomNumberTimePreference(int nRuns, double alpha) {

    if(nRuns==1) {
        return 0;
    }

    TVector<double> S = timePreferenceWeights(nRuns, alpha);
    // calculate the sum
    double Sum = std::accumulate(S.begin(), S.end(), 0.0);

    // normalise by dividing by the sum
    std::transform(S.begin(), S.end(), S.begin(),
                   [Sum](double elem){ return elem/Sum; });

    TVector<double> CDF(nRuns);
    // calculate the CDF
    std::partial_sum(S.begin(), S.end(), CDF.begin());

    double R = TRAND.randUni();

    // Select the generation according to the random number
    TVector<double>::const_iterator it;
    it = std::find_if(CDF.begin(), CDF.end(),
                      [R](double elem){ return (R<elem); });
    int runIndex = it - CDF.begin(); // the selected run

    return runIndex;
}

int TTP_hash(const TVector<int> &variables)
{
    TVector<int> tour(variables.size());

    std::iota(tour.begin(), tour.end(), 1);
    std::sort(tour.begin(), tour.end());

    int tour_id=0;

    do {
        tour_id++;

        if(tour == variables) {
            break;
        }
    } while(std::next_permutation(tour.begin(), tour.end()));

    return tour_id;
}

bool areVectorsEqual(const TVector<double>& a, const TVector<double>& b)
{
    if(a.size()!=b.size()) {
        return false;
    }

    for(auto t : boost::combine(a,b)) {
        if(!areDoublesEqual(t.get<0>(), t.get<1>()))
            return false;
    }

    return true;
}

TVector<double> crowdingDist(const TVector<TVector<double>> &set)
{
    int N = set.size();
    int M = set[0].size();
    TVector <double> cDist(N,0.0);
    TVector <double> objVector(N);

    for(int k=0; k<M; k++) {

        objVector[0] = set[0][k];
        double fmax = objVector[0];
        double fmin = objVector[0];
        for(int i=1; i<N; i++) {
            objVector[i] = set[i][k];
            if(objVector[i]>fmax) fmax = objVector[i];
            if(objVector[i]<fmin) fmin = objVector[i];
        }

        // sort the objective values
        TVector<int> rsInd = Tigon::indSort(objVector);

        // set crowding for extreme solutions
        cDist[rsInd[0]] = 0.0;
        cDist[rsInd[N-1]] = 0.0;

        // set crowding for remaining solutions
        for(int i=1; i<N-1; i++) {
            double currrentCost;
            if( (fmax-fmin)<Tigon::Epsilon ) {
                currrentCost = 0.0;
            } else {
                // normalise b/w 0:1
                currrentCost = set[rsInd[i+1]][k] - set[rsInd[i-1]][k];
                currrentCost = 1.0-currrentCost/(fmax-fmin);
            }
            cDist[rsInd[i]] += currrentCost;
        }
    }

    std::transform(cDist.begin(), cDist.end(), cDist.begin(),
                   std::bind1st(std::multiplies<double>(), 1.0/M));

    return cDist;
}

TVector<int> centroidDistanceOrder(const TVector<TVector<double>>& dataVec,
                                   const TVector<IMappingSPtr>& population,
                                   Tigon::DistanceMeasure distMeasure)
{
    TVector<int> solMap(dataVec.size());

    /// a) determine the centroid of the data
    // vector with the sum the of columns
    TVector<double> centroid = sumColumnsDoubleVector(dataVec);
    // normalise
    std::transform(centroid.begin(), centroid.end(),
                   centroid.begin(),
                   std::bind1st(std::multiplies<double>(),
                                1.0/dataVec.size()));

    /// b) determine the distance between the centroid and each data vector
    TVector<double> distance;
    distance.reserve(dataVec.size());
    for(auto row: dataVec) {
        double dist;
        switch(distMeasure) {
        case (Tigon::EuclideanDistance):
            dist = l2dist(centroid, row);
            break;
        case (Tigon::ManhattanDistance):
            dist = lpdist(centroid, row, 1.0);
            break;
        case (Tigon::AngleDistance):
            dist = angleDistance(centroid, row);
            break;
        default:
            throw TException("TigonUtils", UnrecognisedDistanceMeasure);
        }
        distance.push_back(dist);
    }
    centroid.clear();

    /// c) sort the data vectors based on their distance to the centroid
    TVector<int> sortedInd = indSort(distance);
    distance.clear();

    /// d) Build a map between solutions and data vectors.
    /// Find the shortest distance between a solution and a data vector.

    // solSet: a vector that stores the indices of the solution set.
    // Once a solution is paired with a data vector the
    // corresponding element is removed from solSet.
    TVector<int> solSet(dataVec.size());
    std::iota(solSet.begin(),solSet.end(),0);

    for(auto isort : sortedInd) {

        TVector<double> d;
        d.reserve(solSet.size());
        for(auto pos : solSet) {
            TVector<double> solution = population[pos]->doubleObjectiveVec();
            double dist;
            switch(distMeasure) {
            case (Tigon::EuclideanDistance):
                dist = l2dist(dataVec[isort], solution);
                break;
            case (Tigon::ManhattanDistance):
                dist = lpdist(dataVec[isort], solution, 1.0);
                break;
            case (Tigon::AngleDistance):
                dist = angleDistance(dataVec[isort], solution);
                break;
            default:
                throw TException("TigonUtils", UnrecognisedDistanceMeasure);
            }
            d.push_back(dist);
        }
        int pos_min = min_element(d.begin(), d.end()) - d.begin();

        solMap[solSet[pos_min]]=isort;
        solSet.erase(solSet.begin()+pos_min);
        d.clear();
    }
    return solMap;
}

int uniqueCellIdentificationNumber(const TVector<double> &sol, int nb)
{
    // PRE: the elements of the vector sol have to exist in the interval [0,1]

    for(auto elem: sol) {
        if( (elem<0.0) || (elem>1.0) ) {
            return -1;
        }
    }

    TVector<double> B(nb+1);

    // B={0/nb,1/nb,...,nb/nb} with size nb+1
    for(int i=0; i<nb; i++) {
        B[i] = (double)i/(double)nb;
    }
    B[nb]=1.0;

    int M = sol.size();
    TVector<int> k(M);
    TVector<size_t> indices(nb);
    std::iota(indices.begin(), indices.end(), 0);

    for(int j=0; j<M; j++) {

        double s = sol[j];
        // finds the index i in indices where B[i]<=s<=B[i+1]
        k[j] = *std::find_if(indices.begin(), indices.end(),
                     [s,&B](size_t i){ return ((s>=B[i]) && (s<=B[i+1])); });
    }

    int c=0.0;
    for(int j=1; j<=M; j++) {
        c += k[j-1]*std::pow(nb,j-1);
    }

    return c;
}

TVector<int> binaryRandVector(int n)
{
    TVector<int> b(n);
    std::generate(b.begin(), b.end(), [&]{return TRAND.randInt(2,0); });

    return b;
}

TVector<int> binaryRandVectorConstrained(const TVector<double> &w, double W)
{
    int n = w.size();
    TVector<int> b(n,0);

    TVector<int> positions(n);
    std::iota(positions.begin(), positions.end(), 0);

    // check feasibility
    double sum=0.0;
    for(auto p : positions) {
        sum += w[p];
    }
    bool isFeasible = (sum<W) ? true : false;

    // while is not feasible delete items
    while(!isFeasible) {
        // select a random position
        int R = TRAND.randInt(positions.size(),0);
        // delete the position
        positions.erase(positions.begin()+R);

        sum=0.0;
        for(auto p : positions) {
            sum += w[p];
        }
        isFeasible = (sum<W) ? true : false;
    }
    for(auto p: positions) {
        b[p]=1;
    }

    return b;
}

} // namespace Tigon

