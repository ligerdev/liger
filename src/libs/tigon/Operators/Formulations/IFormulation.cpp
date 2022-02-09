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
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Representation/Problems/Problem.h>

namespace Tigon {
namespace Operators {

IFormulation::IFormulation()
{
    initialise();
}

IFormulation::IFormulation(Representation::IPSet *ipset)
    : IOperator(ipset)
{
    initialise();
}

IFormulation::~IFormulation()
{

}

void IFormulation::initialise()
{

}

void IFormulation::evaluateNode()
{
    updateProblem();
}

TString IFormulation::name()
{
    return TString("Problem Formulation");
}

TString IFormulation::description()
{
    return TString("An interface to define the optimization problem:\n"
                   "Decision variables, objectives, parameters, constraints, "
                   "evaluation functions,\n"
                   "uncertainties for decision variables and function outputs,\n"
                   "preferences, known decision and objective space boundaries.");
}

void IFormulation::defineProblem(ProblemSPtr prob)
{
    IPSet::defineProblem(prob);
}

void IFormulation::appendFunction(IFunctionSPtr func)
{
    IPSet::appendFunction(func);
}

} // namespace Operators
} // namespace Tigon
