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
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Fitness/UncertainMonteCarloRobustness.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Indicators/IRobustnessIndicator.h>
#include <tigon/Representation/Indicators/ConfidenceIndicator.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Representation/Distributions/ChiSquaredDistribution.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>

namespace Tigon {
namespace Operators {

UncertainMonteCarloRobustness::UncertainMonteCarloRobustness()
{
    initialise();
}

UncertainMonteCarloRobustness::UncertainMonteCarloRobustness(Tigon::Representation::IPSet* ipset)
    : RobustnessAssignment(ipset)
{
    initialise();
}

UncertainMonteCarloRobustness::~UncertainMonteCarloRobustness()
{

}

void UncertainMonteCarloRobustness::initialise()
{
//    m_propertyNames.push_back("Percentile");
//    m_propertyDescriptions.push_back("Controls the degree of optimism in computing "
//                                  "the robustness value for the uncertain random "
//                                  "variate. Bothe mean and variance of the distribution "
//                                  "are considered as random vairables, and the "
//                                  "values for them are chosen according to the "
//                                  "percentile.\n Default is 0.05 (optimistic). "
//                                  "Maximum is 0.999");
//    m_propertyNames.push_back("Sample Size");
//    m_propertyDescriptions.push_back("Number of samples of the robustness "
//                                  "indicator.\nDefault is 1000.");

    TP_definePercentile(1 - Tigon::DefaultConfidence);
    TP_defineSampleSize(Tigon::DefaultMonteCarloSample);
}

void UncertainMonteCarloRobustness::evaluateNode()
{
    ISet* oSet = outputSet(0);

    if((oSet == nullptr) || (oSet->size() == 0) || !multiPackExists(SampleSizes)) {
        // ERROR? can't operate
        return;
    }

    TVector<double> neighbourhoodSizes = multiPack(SampleSizes);
    if(neighbourhoodSizes.size() != oSet->size()) {
        //ERROR
        return;
    }

    for(int i=0; i<oSet->size(); i++) {
        IDistribution* dist = oSet->at(i)->cost()->dist();
        if(dist==nullptr) {
            // ERROR
            return;
        }
        double u   = dist->mean();
        double s   = dist->std();
        double N   = neighbourhoodSizes[i];
        double dof = std::max(N-1.0, 1.0);

        // Create distributions for the mean and std
        double uStd = s / std::sqrt(N);
        NormalDistribution mean(u, uStd);
        ChiSquaredDistribution cs(dof);

        // Repeatedly sample the distributions, construct a normal
        // distribution and estimate its indicator
        NormalDistribution nDist;
        SampledDistribution indicatorDist;

        for(int samp=0; samp<m_sampleSize; samp++) {
            double uSamp   = mean.sample();
            double csSamp  = cs.sample();
            double stdSamp = s * std::sqrt(dof/csSamp);
            nDist.defineMean(uSamp);
            nDist.defineStd(stdSamp);
            double r = robustnessOf(&nDist);
            indicatorDist.addSample(r);
        }

        double c = indicatorDist.percentile(m_percentile);

        oSet->at(i)->defineCost(c);
    }
}

double UncertainMonteCarloRobustness::TP_percentile() const
{
    return m_percentile;
}

void UncertainMonteCarloRobustness::TP_definePercentile(double p)
{
    if((p>0) && (p<=0.999)) {
        m_percentile = p;
    }
}

int UncertainMonteCarloRobustness::TP_sampleSize() const
{
    return m_sampleSize;
}

void UncertainMonteCarloRobustness::TP_defineSampleSize(int n)
{
    if(n > 0) {
        m_sampleSize = n;
    }
}

TString UncertainMonteCarloRobustness::name()
{
    return TString("Uncertain Robustness");
}

TString UncertainMonteCarloRobustness::description()
{
    return TString("Assigns a robustness indicator for a random variate "
                   "with estimated parameters.\n"
                   "The random variate is assumed to follow a normal "
                   "distribution. It is described by the expected mean, "
                   "expected std and the sample size.\n"
                   "The operator computes the distribution of the indicator "
                   "using Monte-Carlo sampling, and assigns a fitness "
                   "value according to a defined percentile.\n"
                   "The default is the 5th percintile (optimistic"
                   " estimation for the indicator value).\n"
                   "The default robustness indicator is the value for "
                   "95% confidence.");
}

} // namespace Operators
} // namespace Tigon
