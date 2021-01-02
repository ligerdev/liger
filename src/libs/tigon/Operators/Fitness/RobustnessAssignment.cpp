/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Fitness/RobustnessAssignment.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Indicators/IRobustnessIndicator.h>
#include <tigon/Factories/RobustnessIndicatorFactory.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/ScalarisingFunctions.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

RobustnessAssignment::RobustnessAssignment()
{
    initialise();
}

RobustnessAssignment::RobustnessAssignment(Tigon::Representation::IPSet* ipset)
    : Scalarization(ipset)
{
    initialise();
}

RobustnessAssignment::~RobustnessAssignment()
{
    delete m_indicator;
}

void RobustnessAssignment::initialise()
{
    m_factory = &RobustnessIndicatorFactory::instance();
    m_indicatorType = Tigon::ConfidenceType;
    m_indicatorParameter = Tigon::DefaultConfidence;
    m_indicator = m_factory->create(m_indicatorType, m_indicatorParameter);

    addProperty("IndicatorType"
                , TString("The indicator used to quantify the random variates.\n"
                          "Default is confidence indicator.")
                , typeid(TString).hash_code());

    addProperty("IndicatorParameter"
                , TString("Default is 95% (for the confidence "
                          "indicator type).")
                , getTType(double));

    clearAdditionalOutputTags();
    TStringList tags;
    tags << Tigon::TValidation << Tigon::TFitness;
    defineOutputTags(tags);
}

void RobustnessAssignment::evaluateNode()
{
    ISet* oSet = outputSet(0);

    if((oSet == nullptr) || (oSet->size() == 0)) {
        // ERROR?
        return;
    }

    if(TP_indicatorType() == ConfidenceType) {
        for(int i=0; i<oSet->size(); i++) {
            double r = robustnessOf(oSet->at(i)->cost()->dist());
            oSet->at(i)->defineCost(r);
        }
    } else {
        // Normalise the goal vector
        TVector<double> ideal = IElementVecToRealVec(idealVec());
        TVector<double> antiIdeal = IElementVecToRealVec(antiIdealVec());
        TVector<double> normGoalVec = IElementVecToRealVec(goalVec());
        normaliseToUnitBoxInMemory(normGoalVec, ideal, antiIdeal);

        // Calculate current weight vector
        int M=objectiveVecSize();
        TVector<double> weight(M);
        for(int i=0; i<M; i++) {
            weight[i] = 1.0 / dirVec()[i];
        }
        magnitudeAndDirectionP(weight, 1.0);

        // Scalarise goal vector
        double cost;
        switch(TP_scalarisingFunction()) {
        case (Tigon::WeightedSum):
            cost = weightedSum(weight, normGoalVec);
            break;
        case (Tigon::WeightedChebyshev):
            cost = weightedChebyshev(weight, normGoalVec);
            break;
        case (Tigon::WeightedChebyshevAugmented):
            cost = weightedChebyshevAugmented(weight, normGoalVec);
            break;
        case (Tigon::WeightedLp):
            cost = weightedLp(weight, normGoalVec, pNorm());
            break;
        default:
            throw TException(this->className(),
                             UnrecognisedScalarisationFunction);
        }
        TP_defineIndicatorParameter(cost);        

        for(int i=0; i<oSet->size(); i++) {
            // Multiply the Iq values by -1 to avoid complications in other operators
            double r = -robustnessOf(oSet->at(i)->cost()->dist());            
            oSet->at(i)->defineCost(r);
        }
    }
}

TString RobustnessAssignment::TP_indicatorType() const
{
    return m_indicatorType;
}

void RobustnessAssignment::TP_defineIndicatorType(const TString& t)
{
    IRobustnessIndicator* temp = m_factory->create(t, GenericRobustnessParameter);
    if(temp != nullptr) {
        m_indicatorType = t;
        m_indicatorParameter = Tigon::GenericRobustnessParameter;
        delete m_indicator;
        m_indicator = temp;
        temp = nullptr;
    } else {
        // ERROR - undefined type
    }
}

double RobustnessAssignment::TP_indicatorParameter() const
{
    return m_indicatorParameter;
}

void RobustnessAssignment::TP_defineIndicatorParameter(double p)
{
    IRobustnessIndicator* temp = m_factory->create(m_indicatorType, p);
    if(temp != nullptr) {
        m_indicatorParameter = p;
        delete m_indicator;
        m_indicator = temp;
        temp = nullptr;
    } else {
        // ERROR - invalid parameter
    }
}

void RobustnessAssignment::defineIndicator(TString indicatorType, double indParam)
{
    IRobustnessIndicator* temp = m_factory->create(indicatorType, indParam);
    if(temp != nullptr) {
        m_indicatorType = indicatorType;
        m_indicatorParameter = indParam;
        delete m_indicator;
        m_indicator = temp;
        temp = nullptr;
    } else {
        // ERROR - invalid indicator
    }
}

double RobustnessAssignment::robustnessOf(IDistribution* dist) const
{
    return m_indicator->evaluate(dist);
}

TString RobustnessAssignment::name()
{
    return TString("Robustness Assignment");
}

TString RobustnessAssignment::description()
{
    return TString("Sets the cost of all IMappings in its output set according "
                   "to a robustness indicator.\n"
                   "The robustness is computed for the first element that is "
                   "a random variate.");
}

} // namespace Operators
} // namespace Tigon
