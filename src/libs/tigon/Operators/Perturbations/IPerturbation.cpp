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
#include <tigon/Operators/Perturbations/IPerturbation.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Random/RandomGenerator.h>

namespace Tigon {
namespace Operators {

IPerturbation::IPerturbation()
{
    initialise();
}

IPerturbation::IPerturbation(Representation::IPSet *ipset)
    : IOperator(ipset)
{
    initialise();
}

IPerturbation::~IPerturbation()
{

}

void IPerturbation::initialise()
{
    addProperty("SolutionMutationProbability"
                , TString("Probability of applying mutation to a solution. Default is 1.0.")
                , getTType(double));
    addProperty("VariableMutationProbability"
                , TString("Average number of mutations per "
                          "decision vector.\nThe per-variable "
                          "probability is this number, divided "
                          "by the number of decision variables\n"
                          "Default is 1.")
                , getTType(double));

    TP_defineSolutionMutationProbability(Tigon::DefaultMutationSolutionProbability);
    TP_defineVariableMutationProbability(Tigon::DefaultMutationVariableProbability);

    addOutputTag(Tigon::TForPerturbation);
}

// Solution Probability
double IPerturbation::TP_solutionMutationProbability() const
{
    return m_solutionMutationProbability;
}

void IPerturbation::TP_defineSolutionMutationProbability(double p)
{
    m_solutionMutationProbability = p;
}

// Variable Probability
double IPerturbation::TP_variableMutationProbability() const
{
    return m_variableMutationProbability;
}

void IPerturbation::TP_defineVariableMutationProbability(double p)
{
    m_variableMutationProbability = p;
}

bool IPerturbation::doSolutionMutation()
{
    double probability = m_solutionMutationProbability;

    if(probability >= 1.0) {
        return true;
    } else if(probability <= 0.0) {
        return false;
    } else {
        return (probability > TRAND.randUni());
    }
}

bool IPerturbation::doVariableMutation()
{
    //double probability = m_variableMutationProbability / decisionVecSize();
    double probability = m_variableMutationProbability; // more general

    if(probability >= 1.0) {
        return true;
    } else if(probability <= 0.0) {
        return false;
    } else {
        return (probability > TRAND.randUni());
    }
}

} // namespace Operators
} // namespace Tigon
