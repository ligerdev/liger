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
#include <tigon/Operators/Fitness/SharedFitness.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Representation/Elements/IElementOperations.h>

namespace Tigon {
namespace Operators {

SharedFitness::SharedFitness()
{
    initialise();
}

SharedFitness::SharedFitness(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{
    initialise();
}

SharedFitness::~SharedFitness()
{

}

void SharedFitness::initialise()
{
    addProperty("SharingFunctionAlpha"
                , TString("Controls the linearity of the sharing function.\n"
                          "Defult is 1.0, meaning that the sharing function "
                          "linearly reduces from one to zero.")
                , getType(double));

    clearOutputTags();
    clearAdditionalOutputTags();
    addOutputTag(Tigon::TFitness);
    defineKeepArchive(true);  // to enable calculation of the nadir vector

    TP_defineSharingFunctionAlpha(Tigon::DefaultSharingFunctionAlpha);
}

double SharedFitness::sigmaShareEstimation(int N, int M)
{
    if(N==2) {
        return std::sqrt(M);
    }

    if(M==2) {
        return 2.0/(N-1.0);
    }

    // Function for finding the root
    // (1+sigma)^M - N*(sigma)^M - 1 = 0
    auto sigmaShareFunc = [&](double sigma)
    {
        return pow((1.0+sigma),M) - (N*pow(sigma,M)) - 1.0;
    };

    double a = 0.000001; // sigma > 0
    double b = M;

    double fa = sigmaShareFunc(a);
    double fb = sigmaShareFunc(b);

    if (fa * fb >= 0){
        cout << "Error: change the interval guess for sigma share calculation";
    }

    double currentSigma = a;
    while ((b-a) >= Tigon::DefaultErrorSigmaEstimation) {
        currentSigma = (a+b)/2.0;
        double fc = sigmaShareFunc(currentSigma);
        if ( areDoublesEqual(fc, 0.0) ) {
            break;
        }
        else if ((fc * fa) < 0.0) {
            b = currentSigma;
        }
        else {
            a = currentSigma;
        }
    }

    return currentSigma;
}

void SharedFitness::evaluateNode()
{
    ISet* allSolutions = setsUnion(outputSets());
    int N = allSolutions->size(); // number of solutions

    double sigma = sigmaShareEstimation(N, objectiveVecSize());
    double alpha = TP_sharingFunctionAlpha();

    while(hasNextOutputSet()) {

        ISet* rank = nextOutputSet();
        int rankSize = rank->size();

        TVector<IElement> ideal = copyIElementVecFromPointers(idealVec());
        TVector<IElement> nadir = copyIElementVecFromPointers(nadirVec());

        BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
        box->defineLowerBounds(ideal);
        box->defineUpperBounds(nadir);

        TVector<double> sharedfitness(rankSize);

        for(int i=0; i<rankSize; i++) {
            TVector<IElementSPtr> solA = rank->at(i)->objectiveVec();

            // Determine the distance b/w solution A and all other solutions (including A)
            TVector<double> dist(rankSize);
            for(int j=0; j<rankSize; j++) {
                if(i==j) {
                    dist[j] = 0.0;
                }
                else {
                    TVector<IElementSPtr> solB = rank->at(j)->objectiveVec();
                    dist[j] = normalisedDistanceP(solA, solB, box);
                }
            }

            // niche count of solution i
            double nicheCount = 0.0;
            std::for_each(dist.begin(), dist.end(), [&](double v) {
                nicheCount += (v<=sigma) ? 1.0 - std::pow(v/sigma, alpha) : 0.0;
            });

            double averageFitness = rank->at(i)->doubleCost();
            sharedfitness[i] = averageFitness / nicheCount;
        }

        double sumSharedFitness = std::accumulate(sharedfitness.begin(),
                                                  sharedfitness.end(), 0.0);

        for(int i=0; i<rankSize; i++) {
            double averageFitness = rank->at(i)->doubleCost();
            double scalingFactor = averageFitness*rankSize/sumSharedFitness;
            double scaledFitness = sharedfitness[i]*scalingFactor;
            rank->at(i)->defineCost(scaledFitness);
        }
    }
}

double SharedFitness::TP_sharingFunctionAlpha() const
{
    return m_alpha;
}

void SharedFitness::TP_defineSharingFunctionAlpha(double alpha)
{
    m_alpha = alpha;
}

TString SharedFitness::name()
{
    return TString("Shared Fitness");
}


TString SharedFitness::description()
{
    TString desc("Determines the shared fitness of each solution in the output "
                 "set. This is part of the fitness assignment procedure used by "
                 "the MOGA algorithm.\n"
                 "Pre-condition: the operator assumes that the fitness score of "
                 "the solutions corresponds to their average fitness (e.g. as "
                 "determined by the AverageFitness operator).");
    return desc;
}

} // namespace Operators
} // namespace Tigon


