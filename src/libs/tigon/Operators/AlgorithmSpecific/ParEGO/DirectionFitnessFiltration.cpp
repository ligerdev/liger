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
#include <tigon/Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

DirectionFitnessFiltration::DirectionFitnessFiltration()
{
    initialise();
}

DirectionFitnessFiltration::
DirectionFitnessFiltration(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

DirectionFitnessFiltration::~DirectionFitnessFiltration()
{

}

void DirectionFitnessFiltration::evaluateNode()
{
    // Init
    ISet* iSet  = inputSet(0);
    if(iSet == nullptr) {
        // ERROR
        return;
    }
    int sSize = iSet->size();

    ISet* oSet = outputSet(0);
    if(oSet == nullptr) {
        oSet = appendOutputSet();
    } else {
        oSet->clearMappings();
    }    

    if((TP_maxSolutions() >= sSize) || (TP_maxSolutions() <= 0)) {
        oSet->append(iSet);
        return;
    }

    TVector<int> selIdx = selection(iSet, TP_maxSolutions());
    for(auto idx : selIdx) {
        oSet->append(iSet->at(idx));
    }
}

TVector<int> DirectionFitnessFiltration::selection(ISet* set, int maxSolutions)
{
    TVector<int> out;
    int halfMax = static_cast<int>(std::ceil(maxSolutions/2.0));

    /// 1. Select half of the solutions based on their current fitness

    // Sort half of the required solutions according to fitness
    TVector<double> fitnessVec;
    fitnessVec.reserve(set->size());
    for(auto sol : set->all()) {
        fitnessVec.push_back(sol->doubleCost());
    }
    TVector<int> sortIdx = indSort(fitnessVec);

    for(auto idx : mid(sortIdx, 0, halfMax)) {
        out.push_back(idx);
    }

    // keep the rest of the solutions
    TVector<int> tempIdx;
    ISet* tempSet = new ISet();
    for(auto idx : mid(sortIdx, halfMax)) {
        tempSet->append(set->at(idx));
        tempIdx.push_back(idx); // copy the indices of the remaining solutions
    }


    /// 2. Select the other half based on their proximity to the direction vector

    // Normalise the objective vectors with respect to the ideal and anti-ideal
    TVector<TVector<double>> normalObjectives;
    TVector<double> ideal     = IElementVecToRealVec(idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());
    for(auto sol : tempSet->all()) {
        TVector<double> normVec = normaliseToUnitBox(sol->doubleObjectiveVec(),
                                                     ideal, antiIdeal);
        // project onto the k-1 simplex
        toUnitVec(normVec, 1.0);
        normalObjectives.push_back(normVec);
    }

    // Calculate the Euclidean distance of every solution from the current direction
    TVector<double> distances;
    distances.reserve(tempSet->size());
    for(auto objVec : normalObjectives) {
        distances.push_back(l2dist(objVec, dirVec()));
    }
    delete tempSet;

    // Sort the indices based on their distance
    TVector<int> distInd = indSort(distances);

    // Save the indices (the number of indices is equal to remaining)
    int remaining = maxSolutions - out.size();
    for(auto idx : mid(distInd, 0, remaining)) {
        out.push_back(tempIdx[idx]);
    }

    return out;
}

void DirectionFitnessFiltration::TP_defineMaxSolutions(int n)
{
    m_maxSolutions = n;
}

int DirectionFitnessFiltration::TP_maxSolutions() const
{
    return m_maxSolutions;
}

void DirectionFitnessFiltration::initialise()
{
    addProperty("MaxSolutions"
                ,TString("Maximum number of solutions used to construct the "
                         "surrogate model. If the value of this parameter is "
                         "less than, or equal to zero, then the entire input set "
                         "is used.\n"
                         "Default is to use the entire input set.")
                , getTType(int));

    defineInputTags(TStringList({Tigon::TFitness}));

    /*!
     * \todo remove this configuration. It needs to be defined in IFiltration.
     */
    addOutputTag(Tigon::TFiltration);

    TP_defineMaxSolutions(0);
}

TString DirectionFitnessFiltration::name()
{
    return TString("Direction Fitness Filtration");
}

TString DirectionFitnessFiltration::description()
{
    return TString("Sorts the output set according to fitness and "
                   "direction of the solutions.\n Half of the "
                   "requested size is sorted according to fitness. "
                   "the remaining solutions are sorted according to "
                   "closeness of their direction vector to the current "
                   "direction vector.");
}

} // namespace Operators
} // namespace Tigon
