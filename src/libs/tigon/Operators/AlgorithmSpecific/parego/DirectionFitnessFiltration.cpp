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
#include <tigon/Operators/AlgorithmSpecific/parego/DirectionFitnessFiltration.h>
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

DirectionFitnessFiltration::DirectionFitnessFiltration(Tigon::Representation::IPSet* ipset)
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

    if((m_maxSolutions > sSize) || (m_maxSolutions <= 0)) {
        oSet->append(iSet);
        return;
    }

    /// sort half of the required solution according to fitness    
    TVector<double> fitnessVec;
    for(int i=0; i<sSize; i++) {
        fitnessVec.push_back(iSet->at(i)->doubleCost());
    }
    TVector<int> idx = indSort(fitnessVec);

    int j = 0;
    for( ; j<(m_maxSolutions/2); j++) {
        oSet->append(iSet->at(idx[j]));
    }

    // keep the rest of the solutions
    ISet* tempSet = new ISet();
    for( ; j<sSize; j++) {
        tempSet->append(iSet->at(idx[j]));
    }

    /// Sort the rest of the solutions according to their direction vector
    // Normalise the objective vectors
    // Only operates on the main Optimization set!
    TVector<TVector<double>> normalObjectives;
    TVector<double> ideal     = IElementVecToRealVec(idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());
    for (int i=0; i<tempSet->size(); i++) {
        TVector<double> normVec = tempSet->at(i)->doubleObjectiveVec();
        normaliseToUnitBoxInMemory(normVec, ideal, antiIdeal);
        // project onto the k-1 simplex
        toUnitVec(normVec, 1.0);
        normalObjectives.push_back(normVec);
    }

    // Calculate the Euclidean distance of every solution from the current direction
    TVector<double> distances(tempSet->size(), 0.0);
    TVector<double> dirvec = dirVec();
    for(int i=0; i<tempSet->size(); i++) {
        distances[i] = l2dist(normalObjectives[i], dirvec);
    }

    // append the closest IMappings to the output set
    TVector<int> distInd = indSort(distances);
    int remaining = m_maxSolutions - oSet->size();
    for(int i = 0; i < remaining; i++) {
        oSet->append(tempSet->at(distInd[i]));
    }
    delete tempSet;
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
                ,TString("Maximum number of solutions to be used for "
                         "surrogate modelling. Default of the entire "
                         "input set is used when the value is "
                         "non-positive.")
                , getType(int));

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
