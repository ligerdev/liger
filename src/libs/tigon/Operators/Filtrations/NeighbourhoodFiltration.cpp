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
#include <tigon/Operators/Filtrations/NeighbourhoodFiltration.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

#include <boost/range/adaptor/sliced.hpp>
using boost::adaptors::sliced;

namespace Tigon {
namespace Operators {

NeighbourhoodFiltration::NeighbourhoodFiltration()
{
    initialise();
}

NeighbourhoodFiltration::NeighbourhoodFiltration(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

NeighbourhoodFiltration::~NeighbourhoodFiltration()
{

}

void NeighbourhoodFiltration::initialise()
{
    m_name = TString("Neighbourhood Filtration");
    m_description  = TString("For every solution in the input set, "
                             "creates a set with this solution and other solutions "
                             "within a predefined distance in normalised decision space.\n"
                             "The distance is controlled by changing the neighbourhood radius property");

    addProperty("NeighbourhoodRadius"
                , TString("Maximum distance between solutions to be "
                          "considered as neighbours.\nTo account for "
                          "different scales, the distance is multiplied "
                          "by sqrt(d), where d is the number of decision "
                          "variables.\nThe default is 0.1.")
                , getTType(double));
    addProperty("MaxSolutions"
                , TString("Maximum number of solutions to be used for "
                          "surrogate modelling. Default of the entire "
                          "input set is used when the value is "
                          "non-positive.")
                , getTType(int));
    addProperty("NeighbourhoodSize"
                , TString("Number of neighbours attributed to each solution.\n")
                , getTType(int));
    addProperty("IsClearOutputSets"
                , TString("This property flag, if set to true clears the existing "
                          "Outputsets, otherwise the outputsets are not clear.\n"
                          "The default is true.\n")
                , getTType(bool));

    TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodRadius);
    TP_defineNeighbourhoodRadius(0.1);
    TP_defineMaxSolutions(0);
    TP_defineNeighbourhoodSize(Tigon::DefaultNeighbourhoodSize);
    TP_defineIsClearOutputSets(true);
    TP_defineDistanceMeasure(Tigon::EuclideanDistance);

    addInputTag(Tigon::TForNeighbourhoods);
    addOutputTag(Tigon::TNeighbourhoods);
}

void NeighbourhoodFiltration::evaluateNode()
{
    if(TP_isClearOutputSets()) {
        clearOutputSets();  //overrides the data from previous iteration
    }

    if(!hasNextOutputSet()) {

        // Init
        ISet* iSet = inputSet(0);
        int sSize  = iSet->size();
        TVector<TVector<int>> neighbours(sSize);

        switch (m_neighbourhoodCriterion) {

        case (Tigon::NeighbourhoodRadius):
        {
            // Normalise the decision variables
            BoxConstraintsDataSPtr box = boxConstraints();

            // Calculate the distance (e.g. Euclidean) of every solution from each other
            for(int i=0; i<sSize-1; i++) {
                TVector<IElementSPtr> m = iSet->at(i)->decisionVec();
                for(int h=i+1; h<sSize; h++) {
                    double dist;
                    switch(TP_distanceMeasure()) {
                    case Tigon::ManhattanDistance:
                        dist = normalisedDistanceP(m, iSet->at(h)->decisionVec(),
                                                   box, 1.0);
                        break;
                    case Tigon::EuclideanDistance:
                    default:
                        dist = normalisedDistanceP(m, iSet->at(h)->decisionVec(),
                                                   box, 2.0);
                        break;
                    }

                    if(dist < m_neighbourhoodRadius * std::sqrt(decisionVecSize())) {
                        (neighbours[i]).push_back(h);
                        (neighbours[h]).push_back(i);
                    }
                }
            }

            break;
        }
        case (Tigon::NeighbourhoodSize):
        {
            // MOEA/D uses the weight vectors associated with the solutions
            // to determine the neighbourhoods
            TVector<TVector<double>> weights;
            for(auto solution : iSet->all()) {
                weights.push_back(solution->weightingVec());
            }

            // Calculate the distance (e.g. Euclidean) between every weight
            for(int i=0; i<sSize; i++) {

                TVector<double> wdist;
                wdist.reserve(sSize);

                for(auto wv : weights) {

                    double dist;
                    switch(TP_distanceMeasure()) {
                    case (Tigon::ManhattanDistance):
                        dist = lpdist(weights[i], wv, 1.0);
                        break;
                    case (Tigon::EuclideanDistance):
                        dist = l2dist(weights[i], wv);
                        break;
                    case (Tigon::AngleDistance):
                        dist = angleDistance(weights[i], wv);
                        break;
                    default:
                        throw TException(this->className(),
                                         UnrecognisedDistanceMeasure);
                    }
                    wdist.push_back(dist);
                }
                TVector<int> idx = Tigon::indSort(wdist);

                int smin = std::min(m_neighbourhoodSize+1,
                                    static_cast<int>(idx.size()));

                for(auto e : idx | sliced(1, smin)) {
                    (neighbours[i]).push_back(e);
                }
            }
            break;
        }
        default:
            throw TException(this->className(), UnrecognisedOption);
        }

        // Create sets of neighbourhoods
        int nSets = sSize;
        if((m_maxSolutions > 0) && (m_maxSolutions < nSets)) {
            nSets = m_maxSolutions;
        }

        for(int i=0; i<nSets; i++) {
            ISet* oSet = appendOutputSet();
            oSet->append(iSet->at(i));
            TVector<int> inds = neighbours.at(i);
            for(auto idx : inds) {
                oSet->append(iSet->at(idx));
            }
        }
    }
}

// property: selects the criterion for determining a neighbourhood
void NeighbourhoodFiltration::TP_defineNeighbourhoodCriterion(
        NeighbourhoodType criterion)
{
    switch(criterion) {
    case (Tigon::NeighbourhoodRadius):
        m_neighbourhoodCriterion = Tigon::NeighbourhoodRadius;
        break;
    case (Tigon::NeighbourhoodSize):
        m_neighbourhoodCriterion = Tigon::NeighbourhoodSize;
        break;
    default:
        m_neighbourhoodCriterion = Tigon::NeighbourhoodRadius;
        break;
    }
}

NeighbourhoodType NeighbourhoodFiltration::TP_neighbourhoodCriterion() const
{
    return m_neighbourhoodCriterion;
}

void NeighbourhoodFiltration::TP_defineNeighbourhoodRadius(double r)
{
    if(r > 0.0 && r <= 1.0) {
        m_neighbourhoodRadius = r;
    }
}

double NeighbourhoodFiltration::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

void NeighbourhoodFiltration::TP_defineMaxSolutions(int n)
{
    m_maxSolutions = n;
}

int NeighbourhoodFiltration::TP_maxSolutions() const
{
    return m_maxSolutions;
}

void NeighbourhoodFiltration::TP_defineNeighbourhoodSize(int n)
{
    m_neighbourhoodSize = n;
}

int NeighbourhoodFiltration::TP_neighbourhoodSize() const
{
    return m_neighbourhoodSize;
}

void NeighbourhoodFiltration::TP_defineIsClearOutputSets(bool b)
{
    m_isClearOutputSets = b;
}

bool NeighbourhoodFiltration::TP_isClearOutputSets() const
{
    return m_isClearOutputSets;
}

void NeighbourhoodFiltration::TP_defineDistanceMeasure(DistanceMeasure distance)
{
    m_distanceMeasure = distance;
}

DistanceMeasure NeighbourhoodFiltration::TP_distanceMeasure() const
{
    return m_distanceMeasure;
}


TString NeighbourhoodFiltration::name()
{
    return m_name;
}

TString NeighbourhoodFiltration::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
