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
#include <tigon/Operators/Fitness/NSGAIIINiching.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/SimplexLattice.h>

namespace Tigon {
namespace Operators {

NSGAIIINiching::NSGAIIINiching()
{
    initialise();
}

NSGAIIINiching::NSGAIIINiching(Tigon::Representation::IPSet *ipset): IFitness(ipset)
{
    initialise();
}

NSGAIIINiching::~NSGAIIINiching()
{

}

void NSGAIIINiching::initialise()
{
    clearOutputTags();
    clearAdditionalOutputTags();
    addOutputTag(Tigon::TFitness);

    TP_defineEliteRatio(0.5);
}

void NSGAIIINiching::evaluateNode()
{
    TVector<ISet*> allRanks = outputSets();
    int popSize = 0;
    for(ISet* rank : allRanks) {
        popSize += rank->size();
    }
    int eliteSize = static_cast<int>(std::ceil(popSize*m_eliteRatio));

    ISet* St = new ISet();    // Contains fronts 1 to l
    int FlSize = 0;           // Number of solutions in Fl front
    bool isStEqualFl = false; // Indicates if St is equal to Fl
    for(ISet* rank : allRanks) {
        St->append(rank);
        if(St->size() >= eliteSize) {
            if(rank->size() >= eliteSize) {
                isStEqualFl = true;
            }
            FlSize = rank->size();
            break;
        }
    }

    if(St->size() == eliteSize) {
        return; // no need to change cost
    }

    /// 1. Generate reference points

    // Initial estimation of the lattice resolution
    int H=1;
    int M = objectiveVecSize();
    int numberReferencePoints = M;

    // increase the lattice resolution until it is equal to or larger than
    // the eliteSize
    while(numberReferencePoints < eliteSize) {
        H++;
        numberReferencePoints = simplexLatticeSize(H,M);
    }
    TVector<TVector<double>> refSet = simplexLatticeIterative(H, M);


    /// 2. Normalise the objective vectors and project to the k-1 simplex

    TVector<TVector<double>> nObjs;
    nObjs.reserve(St->size());
    TVector<double> ideal     = IElementVecToRealVec(idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());
    for(auto sol : St->all()) {
        TVector<double> normVec = normaliseToUnitBox(sol->doubleObjectiveVec(),
                                                     ideal, antiIdeal);
        // project onto the k-1 simplex
        toUnitVec(normVec, 1.0);
        nObjs.push_back(normVec);
    }


    /// 3. Associate each population member from St with a reference point

    int Pt1Size = (isStEqualFl) ? 0 : St->size() - FlSize;

    // Niche count for Pt+1 (it might stay empty)
    TVector<int> refCountPrev(numberReferencePoints, 0);
    // Niche count for Fl
    TVector<int> refCountNext(numberReferencePoints, 0);
    // Keeps track of solutions in Fl that are associated with reference points
    TVector<TVector<int>> refIdxNext(numberReferencePoints);

    int idxObj = 0; // the index is w.r.t St
    for(auto nObj : nObjs) {

        // distance b/w normalised solution nObj and all reference points in refSet
        TVector<double> dist;
        dist.reserve(numberReferencePoints);
        for(auto ref : refSet) {
            double d = angleDistance(nObj, ref);
            dist.push_back( d );
        }

        // index of the reference point with the minimum distance to nObj
        int idxRef = std::min_element(dist.begin(), dist.end()) - dist.begin();

        if(isStEqualFl) {

            // Solution is in Fl
            refIdxNext[idxRef].push_back(idxObj);
            refCountNext[idxRef]++;

        } else {

            if(idxObj < Pt1Size) {
                // Solution is in Pt+1
                refCountPrev[idxRef]++;
            } else {
                // Solution is in Fl
                refIdxNext[idxRef].push_back(idxObj);
                refCountNext[idxRef]++;
            }
        }

        idxObj++;
    }


    /// 4. Selection

    TVector<int> selectedSolutions; // selected solutions from Fl

    // refIndices accounts for deleted reference points
    TVector<int> refIndicesMap(numberReferencePoints);
    std::iota(refIndicesMap.begin(), refIndicesMap.end(), 0);

    int K = eliteSize - Pt1Size;    // remaining solutions
    while(static_cast<int>(selectedSolutions.size()) < K)
    {
        // index of the reference point with minimum pj w.r.t Pt+1
        int sElem = vectorMin(refCountPrev);
        TVector<size_t> matches;
        for(size_t i = 0; i<refCountPrev.size(); i++) {
            if(refCountPrev[i]==sElem) {
                matches.push_back(i);
            }
        }
        // If there are multiple reference points that are minimum, then chose at random
        int pRand = TRAND.randInt(matches.size());
        int Jmin = matches[pRand];

        // if there is one or more population members from Fl that are
        // associated with Jmin
        if(refCountNext[Jmin] > 0) {

            // No solutions from Pt+1 associated with Jmin
            if(refCountPrev[Jmin] == 0) {

                // distance b/w normalised solutions and reference point Jmin
                TVector<double> dist;
                dist.reserve(refCountNext[Jmin]);
                for(auto idxObj : refIdxNext[Jmin]) {
                    double d = angleDistance(nObjs[idxObj], refSet[refIndicesMap[Jmin]]);
                    dist.push_back( d );
                }

                // the point from Fl with the shortest distance to p_{Jmin} is
                // added to Pt+1
                int pos = std::min_element(dist.begin(), dist.end()) - dist.begin();
                int idxObj = refIdxNext[Jmin].at(pos);
                selectedSolutions.push_back(idxObj);

                // remove the solution idxObj from refIdxNext
                for(size_t i=0; i<refIdxNext.size(); i++) {
                    if(!refIdxNext[i].empty()) {
                        TVector<int> idxObjs = refIdxNext[i];
                        auto res = std::find(idxObjs.begin(), idxObjs.end(), idxObj);
                        if(res != idxObjs.end()) {
                            // this vector contains the solution, remove it
                            auto position = std::distance(idxObjs.begin(), res);
                            removeAt(refIdxNext[i], position);
                            refCountNext[i]--; // decrease the counter
                        }
                    }
                }
            }
            else {
                // select one member from Fl that is associated with Jmin
                // randomly and add it to Pt+1
                int rand = TRAND.randInt(refIdxNext[Jmin].size());
                int idxObj = refIdxNext[Jmin].at(rand);
                selectedSolutions.push_back(idxObj);

                // remove the solution idxObj from refIdxNext
                for(size_t i=0; i<refIdxNext.size(); i++) {
                    if(!refIdxNext[i].empty()) {
                        TVector<int> idxObjs = refIdxNext[i];
                        auto res = std::find(idxObjs.begin(), idxObjs.end(), idxObj);
                        if(res != idxObjs.end()) {
                            // this vector contains the solution, remove it
                            auto position = std::distance(idxObjs.begin(), res);
                            removeAt(refIdxNext[i], position);
                            refCountNext[i]--; // decrease the counter
                        }
                    }
                }
            }

            // increment p_{Jmin} by one
            refCountPrev[Jmin]++;
        }
        else {
            // exclude reference point Jmin from current generation

            removeAt(refIndicesMap, Jmin);
            removeAt(refCountPrev, Jmin);
            removeAt(refIdxNext, Jmin);
            removeAt(refCountNext, Jmin);
        }
    }

    /// 5. Assignment

    // Subtract 0.1 (meaning better score) to selected solutions from rank Fl
    for(auto idx : selectedSolutions) {
        double prevCost = St->at(idx)->doubleCost();
        St->at(idx)->defineCost(prevCost - 0.1);
    }

    delete St;
}

double NSGAIIINiching::TP_eliteRatio() const
{
    return m_eliteRatio;
}

void NSGAIIINiching::TP_defineEliteRatio(double r)
{
    if(isInInterval(r,0,1)) {
        m_eliteRatio = r;
    }
}

TString NSGAIIINiching::name()
{
    return TString("NSGA-III Niching");
}

TString NSGAIIINiching::description()
{
    return TString("Niching operator based on original NSGA-III paper: "
                   "An evolutionary many-objective optimization algorithm using "
                   "reference-point-based nondominated sorting approach, Part I: "
                   "Solving problems with box constraints, K. Deb and H. Jain, "
                   "IEEE Transactions on Evolutionary Computation, vol. 18, "
                   "no. 4, 577–601, 2014.");
}

} // namespace Operators
} // namespace Tigon
