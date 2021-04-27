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
#include <tigon/tigonconstants.h>
#include <tigon/Operators/Fitness/UncertainConfidenceRobustness.h>
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

UncertainConfidenceRobustness::UncertainConfidenceRobustness()
{
    initialise();
}

UncertainConfidenceRobustness::UncertainConfidenceRobustness(Tigon::Representation::IPSet* ipset)
    : RobustnessAssignment(ipset)
{
    initialise();
}

UncertainConfidenceRobustness::~UncertainConfidenceRobustness()
{

}

void UncertainConfidenceRobustness::initialise()
{
    addProperty("Percentile"
                , TString("Controls the degree of optimism in computing "
                          "the robustness value for the uncertain random "
                          "variate. Bothe mean and variance of the distribution "
                          "are considered as random vairables, and the "
                          "values for them are chosen according to the "
                          "percentile.\n Default is 0.05 (optimistic). "
                          "Maximum is 0.999")
                , getTType(double));

    TP_definePercentile(1.0 - Tigon::DefaultConfidence);
}

void UncertainConfidenceRobustness::evaluateNode()
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

        // use the value of the defined percentile for the mean and std
        // the sample size
        double mu   = mean.percentile(m_percentile);
        double csPerc  = cs.percentile(1.0 - m_percentile);
        double sigma = s * std::sqrt(dof/csPerc);

        NormalDistribution nDist(mu, sigma);
        double c = robustnessOf(&nDist);

        oSet->at(i)->defineCost(c);
    }
}

double UncertainConfidenceRobustness::TP_percentile() const
{
    return m_percentile;
}

void UncertainConfidenceRobustness::TP_definePercentile(double p)
{
    if((p>0) && (p<=0.999)) {
        m_percentile = p;
    }
}

TString UncertainConfidenceRobustness::name()
{
    return TString("Uncertain Confidence-based Robustness");
}

TString UncertainConfidenceRobustness::description()
{
    return TString("Assigns a robustness indicator for a random variate "
                   "with estimated parameters.\n"
                   "The random variate is assumed to follow a normal "
                   "distribution. It is described by the expected mean, "
                   "expected std and the sample size.\n"
                   "The operator considers the mean and variance as "
                   "random variables following a normal and chi-squared "
                   "distributions, respectively.\n"
                   "The robustness indicator is computed for the values "
                   "of the mean and variance for a given percentile.\n"
                   "The default robustness indicator is the value for "
                   "95% confidence, and the default percentile is 5% "
                   "(optimistic).");
}

} // namespace Operators
} // namespace Tigon
