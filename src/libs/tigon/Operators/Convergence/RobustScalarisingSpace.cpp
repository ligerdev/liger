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
#include <tigon/Operators/Convergence/RobustScalarisingSpace.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/ScalarisingSpaceUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/SimplexLattice.h>
#include <cmath>

namespace Tigon {
namespace Operators {

RobustScalarisingSpace::RobustScalarisingSpace()
{
    initialise();
}

RobustScalarisingSpace::RobustScalarisingSpace(Tigon::Representation::IPSet* ipset)
    : IConvergence(ipset)
{
    initialise();
}

RobustScalarisingSpace::~RobustScalarisingSpace()
{
    delete m_indicator;
}

void RobustScalarisingSpace::initialise()
{
    addProperty("neighbourhoodRadius"
                , TString("Maximum Euclidean distance in normalised decision "
                          "space for two solutions to be considered as neighbours")
                , getType(double));

    addProperty("ReferenceSetSize",
                TString("The size of the reference set.\n"
                        "When a certain size is requested,\n"
                        "the actual size is equal or larger. Default is 1."),
                getType(int));

    TP_defineNeighbourhoodRadius(Tigon::DefaultNeighbourhoodRadius);
    TP_defineReferenceSetSizeRobustScalarisingSpace(1);

    m_factory = &RobustnessIndicatorFactory::instance();
    m_indicatorType = Tigon::ConfidenceType;
    m_indicatorParameter = 1.0 - Tigon::DefaultConfidence;
    m_indicator = m_factory->create(m_indicatorType, m_indicatorParameter);

    addInputTag(Tigon::TValidation);
}

void RobustScalarisingSpace::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    ISet* iSet = setsUnion(inputSets());

    int nSolutions = 0;
    for(int i=0; i<iSet->size(); i++) {
        if(iSet->at(i)->isEvaluated() == true) {
            nSolutions++;            
        }
    }

    if(nSolutions == 0) {
        // ERROR?
        return;
    }

    if(currentIteration() == 0) {
        m_ideal = IElementVecToRealVec(idealVec());
        m_antiIdeal = IElementVecToRealVec(antiIdealVec());
        createReferenceSet();
    }

    // Used to normalise the decision variables
    BoxConstraintsDataSPtr box = boxConstraints();

    // Check if the solutions are validated using Monte-Calro sampling
    bool isSampled(false);
    if(iSet->at(0)->isValidated() == SampledValidation) {
        isSampled = true;
    }

    if(isSampled) {
        int nSamps = (static_cast<SampledDistribution*>
                (iSet->at(0)->objectiveVar(0)->dist()))->nSamples();

        TVector<TVector<TVector<double> > > sampledSet;

        sampledSet.resize(nSolutions);
        for(int i = 0; i < nSolutions; i++) {
            sampledSet[i].resize(objectiveVecSize());
            for(int j = 0; j < objectiveVecSize(); j++) {
                sampledSet[i][j].resize(nSamps);
                for(int k = 0; k < nSamps; k++) {
                    SampledDistribution* dist= static_cast<SampledDistribution*>
                            (iSet->at(i)->objectiveVar(j)->dist());
                    sampledSet[i][j][k] =  dist->sampledVal(k);
                }
            }
        }

        m_convIndicatorValue = scalarisingSpaceRandom(sampledSet, m_initRefSet,
                                                      m_ideal, m_antiIdeal,
                                                      m_indicatorParameter);
    } else {
        // Calculate convergence metric value using neighbours
        TVector<TVector<double> > sampledObjSet(nSolutions,
                                               TVector<double>(objectiveVecSize()));
        TVector<TVector<double> > sampledDecSet(nSolutions,
                                               TVector<double>(decisionVecSize()));
        for(int j = 0; j < nSolutions; j++) {
            sampledObjSet[j] = iSet->at(j)->doubleObjectiveVec();
            sampledDecSet[j] = iSet->at(j)->doubleDecisionVec();
        }

        double maxDist = TP_neighbourhoodRadius() * std::sqrt(decisionVecSize());

        m_convIndicatorValue = scalarisingSpaceRobust(sampledObjSet,
                                                      sampledDecSet,
                                                      m_initRefSet,
                                                      m_ideal, m_antiIdeal,
                                                      box, maxDist,
                                                      m_indicatorParameter);
    }

    delete iSet;
}

void RobustScalarisingSpace::defineIndicator(const TString &indicatorType, double indParam)
{
    IRobustnessIndicator* temp = m_factory->create(indicatorType, indParam);
    if(temp != nullptr) {
        m_indicatorType = indicatorType;
        m_indicatorParameter = indParam;
        delete m_indicator;
        m_indicator = temp;
    } else {
        // ERROR - invalid indicator
    }
}

void RobustScalarisingSpace::TP_defineNeighbourhoodRadius(double r)
{
    if(r > 0.0 && r <= 1.0) {
        m_neighbourhoodRadius = r;
    }
}

double RobustScalarisingSpace::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

void RobustScalarisingSpace::createReferenceSet()
{
    // Normalise the goal vector
    TVector<double> nadir = IElementVecToRealVec(nadirVec());
    //TVector<double> normGoalVec = IElementVecToRealVec(goalVec());
    //normaliseToUnitBoxInMemory(normGoalVec, m_ideal, nadir);
    TVector<double> normGoalVec = normaliseForSimplexLattice(
                IElementVecToRealVec(goalVec()), m_ideal, nadir);

    using namespace boost::math;

    // Initial estimation of the lattice resolution
    int h = 1;
    int k = objectiveVecSize();
    double setSize = k;

    while(setSize < m_refSetSize) {
        ++h;
        setSize = factorial<double>(h+k-1) /
                (factorial<double>(k-1) * factorial<double>(h));
    }

    // Create a simplex lattice and remove vectors that are non-dominated
    // with the goal vector (normalised according to the ideal and nadir vectors)
    m_initRefSet = preferredSimplexLattice(h, k, normGoalVec);
}

void RobustScalarisingSpace::TP_defineReferenceSetSizeRobustScalarisingSpace(int s)
{
    m_refSetSize = s;
}

int RobustScalarisingSpace::TP_referenceSetSizeRobustScalarisingSpace() const
{
    return m_refSetSize;
}

TString RobustScalarisingSpace::name()
{
    return TString("RandomScalarisingSpace Convergence");
}

TString RobustScalarisingSpace::description()
{
    return TString("A measure for convergence of the solutions to a "
                   "multiobjective problem.\n"                   
                   "The metric accounts for the inferred performance of each"
                   "solution, rather than the single point estimates.\n"
                   "If solution set is sampled using Monte Carlo, this metric"
                   "calculates 90 percentile using Monte Carlo samples for "
                   "each solution; otherwise, it calculates 90 percentile "
                   "using neighbourhood information.\n"
                   "The convergence metric finds the best costs from the "
                   "current population of the algorithm, aggregated across a "
                   "set of reference directions. The metric is calculated at "
                   "each iteration of the algorithm when solution set is "
                   "updated.\n"                   
                   "In order to keep in the same metric space, reference "
                   "directions, ideal and anti-ideal vectors are obtained "
                   "only from the start of the optimisation process, and are "
                   "not updated.");
}

} // namespace Operators
} // namespace Tigon
