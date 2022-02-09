/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Convergence/RandomHypervolume.h>
#include <tigon/Representation/Distributions/SampledDistribution.h>
#include <tigon/Representation/Indicators/IRobustnessIndicator.h>
#include <tigon/Factories/RobustnessIndicatorFactory.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/HypervolumeUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

RandomHypervolume::RandomHypervolume()
{
    initialise();
}

RandomHypervolume::RandomHypervolume(Tigon::Representation::IPSet* ipset)
    : IConvergence(ipset)
{
    initialise();
}

RandomHypervolume::~RandomHypervolume()
{
    if(m_indicator != nullptr) {
        delete m_indicator;
    }
}

void RandomHypervolume::initialise()
{
    addProperty("IndicatorType"
                , TString("The convergence metric used to "
                          "quantify the the random hypervolume "
                          "measure.\n"
                          "Default is confidence indicator.")
                , getTType(TString));
    addProperty("IndicatorParameter"
                , TString("Default is 95% (for the confidence "
                          "indicator type).")
                , getTType(double));
    addProperty("NSamples"
                , TString("Number of samples from each random "
                          "objective vector.\nIf the objectives "
                          "are sampled through a Monte-Carlo "
                          "Validation operator, this property is "
                          "ignored and the existing number of "
                          "samples is used.\nIf the vectors are "
                          "not sampled, the number of samples "
                          "must be a positive integer.\nDefault "
                          "is ")
                + std::to_string(DefaultMonteCarloSample)
                , getTType(int));
    m_isDefinedRef = false;

    m_factory = &RobustnessIndicatorFactory::instance();
    m_indicatorType = Tigon::ConfidenceType;
    m_indicatorParameter = 1.0 - Tigon::DefaultConfidence;
    m_nSamples = Tigon::DefaultMonteCarloSample;
    m_indicator = m_factory->create(m_indicatorType, m_indicatorParameter);

    addInputTag(Tigon::TValidation);
}

void RandomHypervolume::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    ISet* validatedSet = setsUnion(inputSets());
    validatedSet->shuffle();

    int nSolutions = validatedSet->size();

    // ignore non-validated solutions
    for(int i=nSolutions-1; i>=0; i--) {
        if(validatedSet->at(i)->isValidated() == NotValidated) {
            validatedSet->remove(i);
        }
    }
    nSolutions = validatedSet->size();
    if(nSolutions == 0) {
        // ERROR?
        return;
    }

    // Check if the solutions are validated using Monte-Calro sampling
    bool isSampled(false);
    if(validatedSet->at(0)->isValidated() == SampledValidation) {
        isSampled = true;
    }

    // Reference point
    if(!m_isDefinedRef) {
        m_reference = IElementVecToRealVec(antiIdealVec());
    }

    // Determine the number of samples
    int nSamps;
    if(isSampled) {
        nSamps = (static_cast<SampledDistribution*>
                  (validatedSet->at(0)->objectiveVar(0)->dist()))->nSamples();
    } else {
        if(m_nSamples > 0) {
            nSamps = m_nSamples;
        } else {
            throw TException(this->className(),
                             "Sampled distribution does not exist.");
        }
    }

    // Initialise a vector of hypervolume samples
    TVector<double> hvSamples(nSamps);

    for(int i = 0; i < nSamps; i++) {
        TVector<TVector<double> > sampledSet(nSolutions,
                                             TVector<double>(objectiveVecSize()));
        for(int j = 0; j < nSolutions; j++) {
            for(int k = 0; k < objectiveVecSize(); k++) {
                if(isSampled) {
                    SampledDistribution* dist= static_cast<SampledDistribution*>
                            (validatedSet->at(j)->objectiveVar(k)->dist());
                    sampledSet[j][k] =  dist->sampledVal(i);
                } else {
                    sampledSet[j][k] = validatedSet->at(j)->objectiveVar(k)->sample();
                }
            }
        }
        hvSamples[i] = hypervolume(sampledSet, m_reference);
    }

    // The distribution of the HV samples
    SampledDistribution randHV(hvSamples);

    m_convIndicatorValue = m_indicator->evaluate(&randHV);
    delete validatedSet;
}

TString RandomHypervolume::TP_indicatorType() const
{
    return m_indicatorType;
}

void RandomHypervolume::TP_defineIndicatorType(const TString& t)
{
    IRobustnessIndicator* temp = m_factory->create(t, GenericRobustnessParameter);
    if(temp != nullptr) {
        m_indicatorType = t;
        m_indicatorParameter = Tigon::GenericRobustnessParameter;
        delete m_indicator;
        m_indicator = temp;
        temp = nullptr;
    } else {
        throw TException(this->className(), UnrecognisedTypeException);
    }
}

double RandomHypervolume::TP_indicatorParameter() const
{
    return m_indicatorParameter;
}

void RandomHypervolume::TP_defineIndicatorParameter(double p)
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

int RandomHypervolume::TP_nSamples() const
{
    return m_nSamples;
}

void RandomHypervolume::TP_defineNSamples(int n)
{
    m_nSamples = n;
}

void RandomHypervolume::defineIndicator(TString indicatorType, double indParam)
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

void RandomHypervolume::defineRefPoint(TVector<double> ref)
{
    m_reference = ref;
    m_isDefinedRef = true;
}

TVector<double> RandomHypervolume::ref() const
{
    return m_reference;
}

TString RandomHypervolume::name()
{
    return TString("Confidence Based Convergence");
}

TString RandomHypervolume::description()
{
    return TString("A measure for convergence of the solutions to an uncertain "
                   "multiobjective problem.\n"
                   "Every solution in the input set is repeatedly evaluated to "
                   "estimate its distribution of objective vector.\n"
                   "Then the hypervolume of the set is repeatedly calcuted "
                   "using a different sample from every solution for every "
                   "calculation.\n"
                   "The convergence value is the specified robustness measure "
                   "for the sampled hypervolume.");
}

} // namespace Operators
} // namespace Tigon
