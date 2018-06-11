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
#include <tigon/Operators/Directions/IDirection.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Operators {

IDirection::IDirection()
{
    initialise();
}

IDirection::IDirection(Tigon::Representation::IPSet* ipset)
    : IOperator(ipset)
{
    initialise();
}

IDirection::~IDirection()
{

}

// Solution Probability
double IDirection::TP_solutionCrossoverProbability() const
{
    return m_solutionCrossoverProbability;
}

void IDirection::TP_defineSolutionCrossoverProbability(double p)
{
    m_solutionCrossoverProbability = p;
}

// Variable Probability
double IDirection::TP_variableCrossoverProbability() const
{
    return m_variableCrossoverProbability;
}

void IDirection::TP_defineVariableCrossoverProbability (double p)
{
    m_variableCrossoverProbability = p;
}

// Variable Swap Probability
double IDirection::TP_variableSwapCrossoverProbability() const
{
    return m_variableSwapCrossoverProbability;
}

void IDirection::TP_defineVariableSwapCrossoverProbability (double p)
{
    m_variableSwapCrossoverProbability = p;
}

bool IDirection::doSolutionCrossover()
{
    if(m_solutionCrossoverProbability >= 1.0) {
        return true;
    } else if(m_solutionCrossoverProbability <= 0.0) {
        return false;
    } else {
        return (m_solutionCrossoverProbability > TRAND.randUni());
    }
}

bool IDirection::doVariableCrossover()
{
    if(m_variableCrossoverProbability >= 1.0) {
        return true;
    } else if(m_variableCrossoverProbability <= 0.0) {
        return false;
    } else {
        return (m_variableCrossoverProbability > TRAND.randUni());
    }
}

bool IDirection::doVariableSwap()
{
    if(m_variableSwapCrossoverProbability >= 1.0) {
        return true;
    } else if(m_variableSwapCrossoverProbability <= 0.0) {
        return false;
    } else {
        return (m_variableSwapCrossoverProbability > TRAND.randUni());
    }
}

void IDirection::initialise()
{
    addProperty("SolutionCrossoverProbability"
                , TString("Probability of applying crossover to a solution. "
                          "Default is 0.9.")
                , typeid(double).hash_code());

    addProperty("VariableCrossoverProbability"
                , TString("Probability of applying crossover to a variable in a solution. "
                          "Default is 0.5.")
                , typeid(double).hash_code());

    addProperty("VariableSwapCrossoverProbability"
                , TString("Probability of swapping a variable between two created child solutions. "
                          "Default is 0.5.")
                , typeid(double).hash_code());

    TP_defineSolutionCrossoverProbability(Tigon::DefaultCrossoverSolutionProbability);
    TP_defineVariableCrossoverProbability(Tigon::DefaultCrossoverVariableProbability);
    TP_defineVariableSwapCrossoverProbability(Tigon::DefaultCrossoverVariableSwapProbability);

    addOutputTag(Tigon::TForDirection);
}

} // namespace Operators
} // namespace Tigon
