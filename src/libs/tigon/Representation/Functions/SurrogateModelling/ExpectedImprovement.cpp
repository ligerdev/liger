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
#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>

namespace Tigon {
namespace Representation {

ExpectedImprovement::ExpectedImprovement()
    : m_best(0.0), m_worst(0.0)
{
    TP_defineNInputs(1);
    TP_defineNOutputs(1);

    m_standardNormal = new NormalDistribution(0.0, 1.0);
}

ExpectedImprovement::ExpectedImprovement(KrigingSPtr model)
    : ExpectedImprovement()
{
    defineModel(model);
}

ExpectedImprovement::~ExpectedImprovement()
{
    if(m_standardNormal != nullptr) {
        delete m_standardNormal;
    }
}

TString ExpectedImprovement::name()
{
    return("Expected Improvement");
}

TString ExpectedImprovement::description()
{
    return("A function that estimates the solution's expected improvement over "
           "the current best known solution, based on Kriging estimation.\n"
           "Expected improvement is calculated by considering the value for the "
           "input as a random number, following a normal distribution with the "
           "mean and standard deviation estimated by the Kriging model.");
}

void ExpectedImprovement::evaluate(
        const TVector<IElementSPtr> &inputs,
        const TVector<IElementSPtr> &outputs)
{
    if(model()) {
        double val = value(inputs);
        double err = error(inputs);
        double diff = best() - val;
        double expImprove = 0.0;
        if(err > 0) {
            expImprove = diff * m_standardNormal->cdf(diff/err) +
                         err *  m_standardNormal->pdf(diff/err);

            // normalization
            //double maxPDF = 1.0/sqrt(2.0*boost::math::constants::pi<double>());
            //expImprove = expImprove/(diff + std::fabs(worst()-best())*maxPDF);
        }

        outputs[0]->defineValue(expImprove);
//        outputs[0]->defineValue(std::log10(expImprove + std::numeric_limits<double>::epsilon()));
    }
}

void ExpectedImprovement::updateBest()
{
    TVector<double> yy(model()->outputData());
    m_best  = vectorMin(yy);
    m_worst = vectorMax(yy);
}

void ExpectedImprovement::defineOutputPrpts()
{
    if(TP_nOutputs() != 1) {
        return;
    }
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;

    varNames << "Expected improvement";
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

void ExpectedImprovement::defineModel(KrigingSPtr model)
{
    KrigingSurrogate::defineModel(model);
    updateBest();
}

double ExpectedImprovement::best() const
{
    return m_best;
}

double ExpectedImprovement::worst() const
{
    return m_worst;
}


double ExpectedImprovement::value(const TVector<IElementSPtr>& inputs)
{
    TVector<double> iReal = IElementVecToRealVec(inputs);

    return model()->interpolate(iReal);
}

double ExpectedImprovement::error(const TVector<IElementSPtr>& inputs)
{
    // The model error, is defined by the last vector that was evaluated.
    T_UNUSED(inputs);
    return model()->errorEstimate();
}

void ExpectedImprovement::defineBest(double best)
{
    m_best = best;
}

void ExpectedImprovement::defineWorst(double worst)
{
    m_worst = worst;
}

} // namespace Representation
} // namespace Tigon
