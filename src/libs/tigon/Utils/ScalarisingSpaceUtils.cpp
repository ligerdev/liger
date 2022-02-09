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
#include <tigon/Utils/ScalarisingSpaceUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>

namespace Tigon {

double scalarisingSpace(const TVector<TVector<double > >& objectiveSet,
                       const TVector<TVector<double > >& refSet,
                       const TVector<double>& ideal,
                       const TVector<double>& antiIdeal)
{
    int n = objectiveSet.size();
    if(n==0) {
        return Tigon::Highest;
    }

    TVector<double> normObj;
    TVector<double> cost(n);
    double minCost = 0;
    double ret = 0;
    // Loop through all directions
    for(int nDir=0; nDir<refSet.size(); nDir++) {
        minCost = Tigon::Highest;
        for (int i=0; i<n; i++) {
            // Normalise objective vectors according to ideal and anti-ideal vectors
            normObj = normaliseToUnitBox(objectiveSet[i], ideal, antiIdeal);
            cost[i] = weightedChebyshev(refSet[nDir], normObj);
        }
        sort(cost.begin(),cost.end());
        ret += cost[0];
    }
    ret = ret / refSet.size();
    return ret;
}

double scalarisingSpaceRobust(const TVector<TVector<double> >& objectiveSet,
                             const TVector<TVector<double> >& decisionSet,
                             const TVector<TVector<double> >& refSet,
                             const TVector<double>& ideal,
                             const TVector<double>& antiIdeal,
                             const TVector<TVector<double> >& decBound,
                             double maxDist, double confidenceLevel)
{
    int n = objectiveSet.size();
    if(n==0) {
        return Tigon::Highest;
    }

    // Find neighbours and weights for each sln
    TVector<TVector<int> > m_neighbours(n);
    TVector<TVector<double> > m_weights(n);
    for(int i=0; i<n-1; i++) {
        for(int h = i+1; h < n; h++) {
            TVector<double> diff(decisionSet[0].size(), 0.0);
            for(int j = 0; j < decisionSet[0].size(); j++) {
                double interval = decBound[j][1] - decBound[j][0];
                diff[j] = abs(decisionSet[i][j] - decisionSet[h][j]) / interval;
            }
            double distance = magnitudeAndDirectionP(diff, 2);
            if(distance < maxDist) {
                double weight = 1.0 - distance/maxDist;
                (m_neighbours[i]).push_back(h);
                (m_neighbours[h]).push_back(i);
                (m_weights[i]).push_back(weight);
                (m_weights[h]).push_back(weight);
            }
        }
    }

    // Normalise all objectives
    TVector<TVector<double> > normObj(n);
    for (int i=0; i<n; i++) {
        normObj[i] = normaliseToUnitBox(objectiveSet[i], ideal, antiIdeal);
    }

    TVector<double> cost_real(n);
    TVector<double> cost_inferred(n);
    double minCost = 0;
    double ret = 0;
    // Loop through all directions
    for(int nDir=0; nDir<refSet.size(); nDir++) {
        minCost = Tigon::Highest;
        for (int i=0; i<n; i++) {
            cost_real[i] = weightedChebyshev(refSet[nDir], normObj[i]);
        }

        SampledDistribution samp;
        for(int i=0; i<n; i++) {
            TVector<int>    inds = m_neighbours[i];
            TVector<double> wVec = m_weights[i];
            // include the solution in its neighbourhood
            inds.insert(inds.begin(), i);
            wVec.insert(wVec.begin(), 1.0);

            samp.clear();
            for(int h = 0; h < inds.size(); h++) {
                samp.addSample(cost_real[inds[h]], wVec[h]);
            }

            if (samp.nSamples() == 1) {
                double mean = samp.mean();
                cost_inferred[i] = mean;
            } else {
                double mean = samp.mean();
                double std  = samp.std();
                NormalDistribution dist(mean, std);
                cost_inferred[i] = dist.percentile(confidenceLevel);
            }
        }
        sort(cost_inferred.begin(),cost_inferred.end());
        ret += cost_inferred[0];
    }
    ret = ret / refSet.size();
    return ret;
}

double scalarisingSpaceRobust(const TVector<TVector<double> >& objectiveSet,
                             const TVector<TVector<double> >& decisionSet,
                             const TVector<TVector<double> >& refSet,
                             const TVector<double>& ideal,
                             const TVector<double>& antiIdeal,
                             BoxConstraintsDataSPtr box,
                             double maxDist, double confidenceLevel)
{
    int n = objectiveSet.size();
    if(n==0) {
        return Tigon::Highest;
    }

    // Find neighbours and weights for each sln
    TVector<TVector<int> > m_neighbours(n);
    TVector<TVector<double> > m_weights(n);
    for(int i=0; i<n-1; i++) {
        for(int h = i+1; h < n; h++) {
            double distance = normalisedDistanceVecP(decisionSet[i], decisionSet[h], box);
            if(distance < maxDist) {
                double weight = 1.0 - distance/maxDist;
                (m_neighbours[i]).push_back(h);
                (m_neighbours[h]).push_back(i);
                (m_weights[i]).push_back(weight);
                (m_weights[h]).push_back(weight);
            }
        }
    }

    // Normalise all objectives
    TVector<TVector<double> > normObj(n);
    for (int i=0; i<n; i++) {
        normObj[i] = normaliseToUnitBox(objectiveSet[i], ideal, antiIdeal);
    }

    TVector<double> cost_real(n);
    TVector<double> cost_inferred(n);
    double minCost = 0;
    double ret = 0;
    // Loop through all directions
    for(int nDir=0; nDir<refSet.size(); nDir++) {
        minCost = Tigon::Highest;
        for (int i=0; i<n; i++) {
            cost_real[i] = weightedChebyshev(refSet[nDir], normObj[i]);
        }

        SampledDistribution samp;
        for(int i=0; i<n; i++) {
            TVector<int>   inds = m_neighbours[i];
            TVector<double> wVec = m_weights[i];
            // include the solution in its neighbourhood
            inds.insert(inds.begin(), i);
            wVec.insert(wVec.begin(), 1.0);

            samp.clear();
            for(int h = 0; h < inds.size(); h++) {
                samp.addSample(cost_real[inds[h]], wVec[h]);
            }

            if (samp.nSamples() == 1) {
                double mean = samp.mean();
                cost_inferred[i] = mean;
            } else {
                double mean = samp.mean();
                double std  = samp.std();
                NormalDistribution dist(mean, std);
                cost_inferred[i] = dist.percentile(confidenceLevel);
            }
        }
        sort(cost_inferred.begin(),cost_inferred.end());
        ret += cost_inferred[0];
    }
    ret = ret / refSet.size();
    return ret;
}

double scalarisingSpaceRandom(const TVector<TVector<TVector<double> > >& objectiveSet,
                             const TVector<TVector<double> >& refSet,
                             const TVector<double>& ideal,
                             const TVector<double>& antiIdeal,
                             double confidenceLevel)
{
    int n = objectiveSet.size();
    if(n==0) {
        return Tigon::Highest;
    }

    int oVecSize = objectiveSet[0].size();
    int nSampSize = objectiveSet[0][0].size();
    double cost_real = 0;
    TVector<double> cost_inferred(n);
    double minCost = 0;
    double ret = 0;
    // Loop through all directions
    for(int nDir=0; nDir<refSet.size(); nDir++) {
        minCost = Tigon::Highest;
        // Loop through all solutions
        SampledDistribution samp;
        for (int i=0; i<n; i++) {            
            // Loop through all Monte Carlo samples
            samp.clear();
            for(int k=0; k<nSampSize; k++){
                TVector<double> sampObj(oVecSize);
                // Obtain objective vector
                for(int j=0; j<oVecSize; j++) {
                    sampObj[j] = objectiveSet[i][j][k];
                }
                TVector<double> normObj = normaliseToUnitBox(sampObj, ideal, antiIdeal);
                cost_real = weightedChebyshev(refSet[nDir], normObj);
                samp.addSample(cost_real);
            }
            double mean = samp.mean();
            double std  = samp.std();
            if(std == INFINITY) {
                cost_inferred[i] = mean;
            } else {
                NormalDistribution dist(mean, std);
                cost_inferred[i] = dist.percentile(confidenceLevel);
            }            
        }
        sort(cost_inferred.begin(),cost_inferred.end());
        ret += cost_inferred[0];
    }
    ret = ret / refSet.size();
    return ret;
}

} // namespace Tigon
