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
#include <tigon/Representation/Functions/SurrogateModelling/FeasibleExpectedImprovement.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Representation {

FeasibleExpectedImprovement::FeasibleExpectedImprovement()
    : ExpectedImprovement()
{
    TP_defineNOutputs(1);
}

FeasibleExpectedImprovement::FeasibleExpectedImprovement(KrigingSPtr model)
    : ExpectedImprovement(model)
{
    TP_defineNOutputs(1);
}

FeasibleExpectedImprovement::FeasibleExpectedImprovement(
        KrigingSPtr model,
        const TVector<double>& violation)
    : ExpectedImprovement()
{
    TP_defineNOutputs(1);
    defineModelAndViolation(model, violation);
}

FeasibleExpectedImprovement::~FeasibleExpectedImprovement()
{

}

TString FeasibleExpectedImprovement::name()
{
    return("Expected Improvement with feasible (or close to feasible) best known "
           "solution");
}

TString FeasibleExpectedImprovement::description()
{
    return("This function extends the Expected Improvement function by making "
           "use of feasibility information about the solutions, to estimate the "
           "solution's expected improvement.\n"
           "For this, a violation score is associated with each solution, then "
           "the solution is said to be feasible if the violation score is "
           "smaller than or equal to zero, otherwise the solution is infeasible.\n"
           "To determine the best 'known' solution only feasible solutions are "
           "considered. In case all solutions are infeasible then the least "
           "infeasible solution is chosen. Notably, the higher the violation "
           "score, the greater the infeasibility of the solution is.\n"
           "The violation score is given by the vector m_violation.");
}

void FeasibleExpectedImprovement::defineModelAndViolation(
        KrigingSPtr model, const TVector<double> &violation)
{
    KrigingSurrogate::defineModel(model);
    defineViolation(violation);
    updateBest();
}

void FeasibleExpectedImprovement::defineViolation(const TVector<double>& violation)
{
    // check that the size of the violation vector is equal to
    // the number of solutions in the Kriging model
    if(violation.size() == model()->outputData().size()) {
        m_violation = violation;
    }
}

void FeasibleExpectedImprovement::updateBest()
{
    if(m_violation.empty()) {
        ExpectedImprovement::updateBest();
        return;
    }

    TVector<double> yy(model()->outputData());
    defineWorst(vectorMax(yy));

    TVector<double> yy_feasible;

    for(size_t i=0; i<yy.size(); i++) {
        if( m_violation[i] <= 0.0 ) { // only add feasible solutions
            yy_feasible.push_back(yy[i]);
        }
    }

    if(yy_feasible.empty()) {
        // select the least infeasible solution
        auto index = std::min_element(m_violation.begin(), m_violation.end()) -
                     m_violation.begin();
        auto besty = yy[index];
        defineBest(besty);
    } else {
        auto besty = vectorMin(yy_feasible);
        defineBest(besty);
    }
}

void FeasibleExpectedImprovement::defineOutputPrpts()
{
    if(TP_nOutputs() != 1) {
        return;
    }
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;

    varNames << "Feasible expected improvement";
    varDescriptions << "The expected improvement over the best 'known' point "
                       "to the Kriging model. Expected improvement is calculated "
                       "by considering the value for the input as a random "
                       "number, following a normal distribution with the mean "
                       "and standard deviation estimated by the Kriging model.";
    typeVec << RealType;
    varUnits << "";
    TVector<OptimizationType> optTypes({Maximization});
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

} // namespace Representation
} // namespace Tigon
