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
#include <tigon/Operators/Evaluators/RandomNumberEstimator.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Utils/NormalisationUtils.h>

#include <QtMath>

namespace Tigon {
namespace Operators {

RandomNumberEstimator::RandomNumberEstimator()
{
    m_propertyNames.append("Neighbourhood Radius");
    m_propertyDescriptions.append("Maximum distance between solutions to be "
                                  "cosidered as neighbours.\nTo account for "
                                  "different scales, the distance is multiplied "
                                  "by sqrt(d), where d is the number of decision "
                                  "variables.\nThe default is 0.1");
    m_propertyNames.append("Maximum Solutions");
    m_propertyDescriptions.append("Maximum number of solutions to be used for "
                                  "surrogate modelling. Default of the entire "
                                  "input set is used when the value is "
                                  "non-positive.");

    addOutputTag(Tigon::TRandomNumbers);
    addInputTag(Tigon::TFitness);
    // TODO: consider a different type of operator (not evaluator)
    removeOutputTag(Tigon::TForEvaluation);

    TP_defineNeighbourhoodRadius(0.1);
    TP_defineMaxSolutions(0);
}

RandomNumberEstimator::RandomNumberEstimator(IPSet* ipset)
    : IEvaluator(ipset)
{
    m_propertyNames.append("Neighbourhood Radius");
    m_propertyDescriptions.append("Maximum distance between solutions to be "
                                  "cosidered as neighbours.\nTo account for "
                                  "different scales, the distance is multiplied "
                                  "by sqrt(d), where d is the number of decision "
                                  "variables.\nThe default is 0.1");
    m_propertyNames.append("Maximum Solutions");
    m_propertyDescriptions.append("Maximum number of solutions to be used for "
                                  "surrogate modelling. Default of the entire "
                                  "input set is used when the value is "
                                  "non-positive.");

    addOutputTag(Tigon::TRandomNumbers);
    addInputTag(Tigon::TFitness);
    // TODO: consider a different type of operator (not evaluator)
    removeOutputTag(Tigon::TForEvaluation);

    TP_defineNeighbourhoodRadius(0.1);
    TP_defineMaxSolutions(0);
}

RandomNumberEstimator::~RandomNumberEstimator()
{

}

void RandomNumberEstimator::evaluateNode()
{
#ifdef SPAREGO_DEBUG
    cout << "\tIn RandomNumberEstimator";
#endif
    // Init
    clearOutputSets();  //overrides the data from previous iteration
    ISet* oSet = appendOutputSet();
    ISet* iSet = inputSet(0);
    int sSize = iSet->size();

/// Find the neighbours of every solution
    //TODO - It will be more efficient only to add the newly evaluated solutions
    //       by keeping the distances matrix from previous iterations.

    // Normalise the decision variables
    QVector<QVector<IElementSPtr> > normalInputs;
    BoxConstraintsDataSPtr box = boxConstraints();
    for (int i=0; i<sSize; i++) {
        QVector<IElementSPtr> normVec(decisionVecSize());
        for(int j=0; j<decisionVecSize(); j++) {
            normVec[j] = iSet->at(i)->decisionVar(j)->clone();
        }
        normaliseToUnitBox(normVec, box);
        normalInputs.append(normVec);
    }

    // Calculate the Euclidean distance of every solution from each other
    // and assign a correlation weight according to distance
    QVector<QVector<int> > neighbours(sSize);
    QVector<QVector<qreal> > weights(sSize);
    qreal maxDist = m_neighbourhoodRadius * qSqrt(decisionVecSize());
    for(int i=0; i<sSize-1; i++) {
        for(int h=i+1; h<sSize; h++) {
            qreal dist = distanceP(normalInputs[i], normalInputs[h], 2.0);
            if(dist < maxDist) {
                (neighbours[i]).append(h);
                (neighbours[h]).append(i);
                (weights[i]).append(1.0 - dist/maxDist);
                (weights[h]).append(1.0 - dist/maxDist);
            }
        }
    }

    // Create a set of generic mappings with the statistics of every solution's
    // neighbourhood
    QVector<Tigon::ElementType> ovecTypes;
    // weighted mean, weighted std, sample size (sum of weights), random distribution
    ovecTypes << Tigon::RealType << Tigon::RealType << Tigon::RealType <<
                 Tigon::RealType;

    ProblemSPtr genericProblem = ProblemSPtr::create();
    genericProblem->defineDVecPrpts(decisionVecProperties());
    genericProblem->defineOVecTypes(ovecTypes);

    SampledDistribution samp;

    int nSets = sSize;
    if((m_maxSolutions > 0) && (m_maxSolutions < nSets)) {
        nSets = m_maxSolutions;
    }

    for(int i=0; i<nSets; i++) {
        QVector<int>   inds = neighbours[i];
        QVector<qreal> wVec = weights[i];
        // include the solution in its neighbourhood
        inds.prepend(i);
        wVec.prepend(1.0);

        IMappingSPtr imap = createGenericMapping(oSet, genericProblem);
        // Copy the decision vector
        imap->setDecisionVec(iSet->at(i)->decisionVec());

        int N = inds.size();
        if(N < 2) {
            // ERROR
//            cout << "ERROR in RandomNumberEstimator - Neighbourhood size smaller "
//                    "than 2" << endl;
            if(N==0) {
                // ERROR - this really shouldn't happen!
            } else { // N == 1
                qreal mean = iSet->at(i)->doubleCost();
                qreal std  = iSet->at(i)->doubleCost();

                imap->setObjectiveVar(0, IElement(mean));
                imap->setObjectiveVar(1, IElement(std));
                imap->setObjectiveVar(2, IElement(1.0));
                IElementSPtr el = IElementSPtr::create();
                el->defineDist(new NormalDistribution(mean, std));
                imap->setObjectiveVar(3, el);
            }

        } else {

            // Calculate the weighted mean and std in the neighbourhood
            samp.clear();
            qreal sampleSize = 0.0;
            for(int h=0; h<N; h++) {
                samp.addSample(iSet->at(inds[h])->doubleCost(), wVec[h]);
                sampleSize += wVec[h];
            }
            qreal mean = samp.mean();
            qreal std  = samp.std();

            imap->setObjectiveVar(0, IElement(mean));
            imap->setObjectiveVar(1, IElement(std));
            imap->setObjectiveVar(2, IElement(sampleSize));
            IElementSPtr el = IElementSPtr::create();
            el->defineDist(new NormalDistribution(mean, std));
            imap->setObjectiveVar(3, el);
        }
    }
    genericProblem.clear();
}

void RandomNumberEstimator::TP_defineNeighbourhoodRadius(qreal r)
{
    if(r > 0.0 && r <= 1.0)
    m_neighbourhoodRadius = r;
}

qreal RandomNumberEstimator::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

void RandomNumberEstimator::TP_defineMaxSolutions(int n)
{
    m_maxSolutions = n;
}

int RandomNumberEstimator::TP_maxSolutions() const
{
    return m_maxSolutions;
}

QString RandomNumberEstimator::name()
{
    return QString("Random Number Estimator");
}

QString RandomNumberEstimator::description()
{
    return QString("Representation of uncertain function "
                   "evaluation as a normal distribution.\n"
                   "For solutions with close neighbours, the variance "
                   "is taken as the variance of the "
                   "fitness within the neighbourhood.\n"
                   "For solutions without neighbours, the variance is estimated "
                   "from a Kriging model, based on the variance "
                   "of the other solutions.\n"
                   "The mean value is the mean of the neighbourhood.");
}

QStringList RandomNumberEstimator::propertyNames()
{
    return m_propertyNames;
}


QStringList RandomNumberEstimator::propertyDescriptions()
{
    return m_propertyDescriptions;
}

} // namespace Operators
}  // namespace Tigon
