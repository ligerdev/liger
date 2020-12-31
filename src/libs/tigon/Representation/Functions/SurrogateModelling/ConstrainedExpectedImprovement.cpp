/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Functions/SurrogateModelling/ConstrainedExpectedImprovement.h>

#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>

#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/ProbabilityFeasibility.h>

namespace Tigon {
namespace Representation {


ConstrainedExpectedImprovement::ConstrainedExpectedImprovement()
{
    TP_defineNInputs(1);
    TP_defineNOutputs(3);
    defineParallelisable(false);
}

ConstrainedExpectedImprovement::
ConstrainedExpectedImprovement(ExpectedImprovement* ei,
                               const TVector<ProbabilityFeasibility*>& vpf)
    : ConstrainedExpectedImprovement()
{
    defineExpectedImprovement(ei);
    defineProbabilityFeasibility(vpf);
}


ConstrainedExpectedImprovement::~ConstrainedExpectedImprovement()
{

}

TString ConstrainedExpectedImprovement::name()
{
    return("Constrained Expected Improvement");
}

TString ConstrainedExpectedImprovement::description()
{
    return("Implements a fully probabilistic approach where constraints are "
           "added to the expected improvement. As a result, if the predicted errors "
           "in the constrained model are low and surrogate shows a constrained "
           "violation then the expected improvement will also be low, but not "
           "zero, since there is a finite possibility that a full evaluation of "
           "the constraints may actually reveal a feasible design. Conversely, "
           "if the error in the constraints are large then there will be a "
           "significant chance that the constraint predictions are wrong and "
           "that a new point will, in fact, be feasible. Thus, the expectation "
           "of improvement will be greater.");
}

void ConstrainedExpectedImprovement::
evaluate(const TVector<IElementSPtr> &inputs,
         const TVector<IElementSPtr> &outputs)
{
    // Make a copy of the vector of outputs
    TVector<IElementSPtr> outputs_temp;
    for(auto output : outputs) {
        outputs_temp.push_back(output->clone());
    }

    // Evaluate expected improvement function
    m_ei->evaluate(inputs, outputs_temp);
//    double ei = std::log10(outputs_temp[0]->value<double>()+std::numeric_limits<double>::epsilon());
    double ei = outputs_temp[0]->value<double>();
    outputs[1]->defineValue(ei); // output for expected improvement

    // Evaluate probability of feasibility function for each constraint
    double mpf = 1.0;
//    double mpf = 0.0;
    for(size_t i = 0; i<m_vpf.size(); i++) {

        m_vpf[i]->evaluate(inputs, outputs_temp);
//        double pf = std::log10(outputs_temp[0]->value<double>()+std::numeric_limits<double>::epsilon());
        double pf = outputs_temp[0]->value<double>();
        outputs[i+2]->defineValue(pf); // outputs for probability of feasibility

        mpf *= pf;
//        mpf += pf;
    }

    outputs[0]->defineValue(ei*mpf); // output for EIxPF
//    outputs[0]->defineValue(std::log10(ei*mpf + std::numeric_limits<double>::epsilon()));
//    outputs[0]->defineValue(ei+mpf); // output for EIxPF
}

void ConstrainedExpectedImprovement::
defineExpectedImprovement(ExpectedImprovement* ei)
{
    m_ei = ei;
    TP_defineNInputs(ei->model()->inputData()[0].size());
}

void ConstrainedExpectedImprovement::
defineProbabilityFeasibility(const TVector<ProbabilityFeasibility*>& vpf)
{
    m_vpf = vpf;
    TP_defineNOutputs(2+vpf.size());
}

void ConstrainedExpectedImprovement::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<IElement>    lowerBounds(0); // -inf
    TVector<IElement>    upperBounds(0); // inf

    for(int i = 0; i < TP_nInputs(); i++) {
        varNames.push_back("Input_Var_" + std::to_string(i));
        varDescriptions.push_back("Input_VarDesc_" + std::to_string(i));
        typeVec.push_back(RealType);
        varUnits.push_back("");
    }
    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void ConstrainedExpectedImprovement::defineOutputPrpts()
{
    if(TP_nOutputs() < 3) {
        return;
    }

    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    varNames << "EIxPF";
    varDescriptions << "EIxPF";
    typeVec << RealType;
    varUnits << "";
    optTypes << Maximization;

    // The following outputs are kept for information only

    varNames << "Expected_Improvement";
    varDescriptions << "The expected improvement over the best 'known' point "
                       "to the Kriging model. Expected improvement is calculated "
                       "by considering the value for the input as a random "
                       "number, following a normal distribution with the mean "
                       "and standard deviation estimated by the Kriging model.";
    typeVec << RealType;
    varUnits << "";
    optTypes << NonOptimization;

    int nConstraints = TP_nOutputs() - 2;
    for(int i=0; i<nConstraints; i++) {
        varNames << "Probability_of_Feasibility for constraint number " + convertToString(i+1);
        varDescriptions << "The probability of feasibility of the curent "
                           "solution for constraint number " + convertToString(i+1);
        typeVec << RealType;
        varUnits << "";
        optTypes << NonOptimization;
    }

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}


} // namespace Representation
} // namespace Tigon
