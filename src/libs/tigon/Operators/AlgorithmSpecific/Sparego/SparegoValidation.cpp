/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/AlgorithmSpecific/Sparego/SparegoValidation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Distributions/IDistribution.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Representation/Distributions/ChiSquaredDistribution.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Utils/Kriging.h>
#include <tigon/Utils/PowerVariogram.h>

#include <cmath>

namespace Tigon {
namespace Operators {

SparegoValidation::SparegoValidation()
{    
    initialise();
}

SparegoValidation::SparegoValidation(Tigon::Representation::IPSet *ipset)
    : IEvaluator(ipset)
{
    initialise();
}

SparegoValidation::~SparegoValidation() {

}

void SparegoValidation::evaluateNode()
{
    /// Init
    ISet* oSet = outputSet(0);
    if(oSet == nullptr) {
        throw TException(this->className(), TagsNotFoundException);
    }

    int nPrev = m_neighbours.size();
    int sSize = oSet->size();

    double maxDist = m_neighbourhoodRadius * std::sqrt(decisionVecSize());

    m_neighbours.resize(sSize);
    m_weights.resize(sSize);
    m_sampleSizes.resize(sSize);

    /// Find the neighbours of every solution
    /// only to add the newly evaluated solutions by keeping the distances matrix
    /// from previous iterations.

    // Normalise the new decision variables
    BoxConstraintsDataSPtr box = boxConstraints();

    // Calculate the Euclidean distance of every solution from each other
    // and assign a correlation weight according to distance
    for(int h = nPrev; h < sSize; h++) {
        // The contribution of the solution itself to its sample size
        // is 1.0
        m_sampleSizes[h] = 1.0;
    }

    for(int i=0; i<sSize-1; i++) {
        TVector<IElementSPtr> m = oSet->at(i)->decisionVec();
        for(int h = std::max(nPrev, i+1); h < sSize; h++) {

            double dist = normalisedDistanceP(m,oSet->at(h)->decisionVec(),box);
            if(dist < maxDist) { // solutions are neighbours

                double weight = 1.0 - dist/maxDist;
                (m_neighbours[i]).push_back(h);
                (m_neighbours[h]).push_back(i);
                (m_weights[i]).push_back(weight);
                (m_weights[h]).push_back(weight);

                // Add the weight to both solutions' sample size
                m_sampleSizes[i] += weight;
                m_sampleSizes[h] += weight;
            }
        }
    }

    /// Create a normal distribution for every IElement
    /// (objectives, constraints, cost) based on neighbourhood information

    SampledDistribution samp;
    IDistribution* dist;

    // for each solution
    for(int i = 0; i < sSize; i++) {

        TVector<int> inds = m_neighbours[i];
        if(inds.empty()) {
            // solutions without neighbours, not currently supported
            return;
        }
        else {
            // solutions with neighbours
            TVector<double> wVec = m_weights[i];

            // include the solution in its neighbourhood
            inds.insert(inds.begin(), i);
            wVec.insert(wVec.begin(), 1.0);

            // solution
            IMappingSPtr imap = oSet->at(i);

            /// Compute the distributions of objectives and constraints only if the
            /// solution was not validated in other methods (i.e., Monte-Carlo)
            if((imap->isValidated() == ParametricValidation) ||
                    (imap->isValidated() == NotValidated)) {

                // **********
                // Objectives
                // **********

                for(int j=0; j<objectiveVecSize(); j++) {
                    // Calculate the weighted mean and std in the neighbourhood
                    samp.clear();

                    for(int h = 0; h < inds.size(); h++) {
                        double val = oSet->at(inds[h])->doubleObjectiveVar(j);
                        samp.addSample(val, wVec[h]);
                    }
                    double mean = samp.mean();
                    double std  = samp.std();

                    dist = modifyDistParams(mean, std, m_sampleSizes[i]);

                    imap->objectiveVar(j)->defineDist(dist);
                }

                // ***********
                // Constraints
                // ***********

                for(int j=0; j<constraintVecSize(); j++) {
                    samp.clear();
                    for(int h=0; h<inds.size(); h++) {
                        double val = oSet->at(inds[h])->doubleConstraintVar(j);
                        samp.addSample(val, wVec[h]);
                    }
                    double mean = samp.mean();
                    double std  = samp.std();

                    dist = modifyDistParams(mean, std, m_sampleSizes[i]);

                    imap->constraintVar(j)->defineDist(dist);
                }
            }

            /// The cost distribution is required for sParEGO

            // ****
            // Cost
            // ****

            samp.clear();
            for(int h=0; h<inds.size(); h++) {
                double val = oSet->at(inds[h])->doubleCost();
                samp.addSample(val, wVec[h]);
            }
            double mean = samp.mean();
            double std  = samp.std();

            dist = modifyDistParams(mean, std, m_sampleSizes[i]);

            imap->cost()->defineDist(dist);

            /// Mark the solution as validated only if it wasn't already validated
            /// by another operator
            if(imap->isValidated() == NotValidated) {
                imap->defineValidated(ParametricValidation);
            }
        }
    }
}

double SparegoValidation::sampleSize(int idx) const
{
    if(isInRange(idx, m_sampleSizes.size())) {
        return m_sampleSizes[idx];
    } else {
        throw TException(this->className(), RangeException);
        return PROTOTYPE_REAL_ERROR_CODE;
    }
}

TVector<int> SparegoValidation::neighboursOf(int idx) const
{
    if(isInRange(idx, m_neighbours.size())) {
        return m_neighbours[idx];
    } else {
        throw TException(this->className(), RangeException);
        return TVector<int>();
    }
}

double SparegoValidation::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

void SparegoValidation::TP_defineNeighbourhoodRadius(double r)
{
    if(r > 0.0 && r <= 1.0) {
        m_neighbourhoodRadius = r;
    } else {
        throw TException(this->className(), DomainException);
    }

}

double SparegoValidation::TP_percentile() const
{
    return m_percentile;
}

void SparegoValidation::TP_definePercentile(double p)
{
    if((p>0) && (p<=0.999)) {
        m_percentile = p;
    } else {
        throw TException(this->className(), DomainException);
    }
}

ConfidenceInSampleType SparegoValidation::TP_confidenceInSample() const
{
    return m_confType;
}

void SparegoValidation::TP_defineConfidenceInSample(ConfidenceInSampleType t)
{
    m_confType = t;
}

TString SparegoValidation::name()
{
    return TString("SparegoValidation");
}

TString SparegoValidation::description()
{
    return TString("Exposes the distribution of every solution, as perceived "
                   "by sParEGO.\n"
                   "A normal distribution is added to every output (objective "
                   "and constraint) and to the cost according to the "
                   "information form the neighbours");
}

void SparegoValidation::initialise()
{
    addProperty("NeighbourhoodRadius"
                , TString("Maximum distance between solutions to be "
                          "cosidered as neighbours.\nTo account for "
                          "different scales, the distance is multiplied "
                          "by sqrt(d), where d is the number of decision "
                          "variables.\nThe default is 0.1")
                , getType(double));
    addProperty("Percentile"
                , TString("Controls the degree of optimism in computing "
                          "the robustness value for the uncertain random "
                          "variate. Bothe mean and variance of the distribution "
                          "are considered as random vairables, and the "
                          "values for them are chosen according to the "
                          "percentile.\n Default is 0.05 (optimistic). "
                          "Maximum is 0.999")
                , getType(double));

    /// \bug What is this?? Should be ConfidenceInSampleType
    //m_propertyNames.push_back("Sample Size Effect on Distribution");
    //m_propertyDescriptions.push_back("TODO");

    TP_defineNeighbourhoodRadius(Tigon::DefaultNeighbourhoodRadius);
    TP_defineConfidenceInSample(Tigon::UnchangedDistribution);
    TP_definePercentile(1 - Tigon::DefaultConfidence);

    TP_defineCountEvaluations(false);

    addOutputTag(Tigon::TFitness);
    addAdditionalOutputTag(Tigon::TValidation);
}

// Private method
IDistribution* SparegoValidation::modifyDistParams(double nomMean,
                                                   double nomStd,
                                                   double sampSize) const
{
    if(nomStd <= 0.0 || std::isinf(nomStd)) {
        // Change the variance to epsilon
        nomStd = Tigon::Epsilon;
        throw TException(this->className(), DomainException);
    }

    IDistribution* dist(nullptr);
    if(m_confType == UnchangedDistribution) {
        dist = new NormalDistribution(nomMean, nomStd);
        return dist;
    }

    if(sampSize <= 1) {
        throw TException(this->className(), DomainException);
    }

    switch(m_confType)
    {
    case UnchangedDistribution:
    default:
    {
        dist = new NormalDistribution(nomMean, nomStd);
        break;
    }
    }

    return dist;
}

} // namespace Operators
} // namespace Tigon
