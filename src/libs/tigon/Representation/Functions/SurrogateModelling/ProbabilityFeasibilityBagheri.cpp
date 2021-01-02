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
#include <tigon/Representation/Functions/SurrogateModelling/ProbabilityFeasibilityBagheri.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/PSets/PSetBase.h>

#include <iostream>
#include <algorithm>

namespace Tigon {
namespace Representation {

ProbabilityFeasibilityBagheri::ProbabilityFeasibilityBagheri()
{
    TP_defineNInputs(1);
    TP_defineNOutputs(1);
    m_standardNormal = new NormalDistribution(0.0, 1.0);
}

ProbabilityFeasibilityBagheri::ProbabilityFeasibilityBagheri(const KrigingSPtr &model)
    : ProbabilityFeasibilityBagheri()
{
    defineModel(model);
}


ProbabilityFeasibilityBagheri::~ProbabilityFeasibilityBagheri()
{
    if(m_standardNormal != nullptr) {
        delete m_standardNormal;
    }
}

TString ProbabilityFeasibilityBagheri::name()
{
    return("Probability of feasibility using Bagheri's modification");
}

TString ProbabilityFeasibilityBagheri::description()
{
    return("A function that estimates the solution's probability of being feasible "
           "regarding the contraints, based on Kriging estimation.\n"
           "Probability of feasibility is calculated by considering the value for the "
           "input as a random number, following a normal distribution with the "
           "mean and standard deviation estimated by the Kriging model. This modifed function"
           "is proposed by Bagheri et al. (GECCO 2017) function that allows solutions arround "
           "the boundary a higher chance to be selected .");
}

void ProbabilityFeasibilityBagheri::evaluate(const TVector<IElementSPtr> &inputs,
                                        const TVector<IElementSPtr> &outputs)
{
    if(model()) {
        double val = value(inputs);
        double err = error(inputs);
        double PF = 0.0;
        if(err > 0) {
            PF = 2*m_standardNormal->cdf(-(val/err));
        };
        PF = std::min(PF, 1.0);
        outputs[0]->defineValue(PF);
    }
}


void ProbabilityFeasibilityBagheri::defineOutputPrpts()
{
    if(TP_nOutputs() != 1) {
        return;
    }
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;

    varNames << "Probability of feasibility";
    varDescriptions << "Determines the probability of the prediction being "
                       "greater than the constraint limit, i.e. the probability "
                       "that the constraint is met.\n"
                       "A Kriging model is fit to the constraint violation "
                       "values of the solutions, where a positive value "
                       "indicates that the solution violates the constraint, "
                       "otherwise the value is zero.\n"
                       "The probability values are inside the range [0 1].";

    typeVec << RealType;
    varUnits << "";
    TVector<OptimizationType> optTypes({Maximization});
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

void ProbabilityFeasibilityBagheri::defineModel(KrigingSPtr model)
{
    KrigingSurrogate::defineModel(model);
}


double ProbabilityFeasibilityBagheri::value(const TVector<IElementSPtr> &inputs)
{
    TVector<double> iReal = IElementVecToRealVec(inputs);

    return model()->interpolate(iReal);
}

double ProbabilityFeasibilityBagheri::error(const TVector<IElementSPtr> &inputs)
{
    // The model error, is defined by the last vector that was evaluated.
    T_UNUSED(inputs);
    return model()->errorEstimate();
}

} // namespace Representation
} // namespace Tigon
