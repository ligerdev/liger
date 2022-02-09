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
#include <tigon/Representation/Functions/SurrogateModelling/RandExpectedImprovement.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/KrigingVariogram.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/KernelDensityEstimation.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>

#include <boost/math/constants/constants.hpp>

namespace Tigon {
namespace Representation {

RandExpectedImprovement::RandExpectedImprovement()
{
    TP_defineNOutputs(1);
}

RandExpectedImprovement::RandExpectedImprovement(KrigingSPtr model)
    : ExpectedImprovement(model)
{
    TP_defineNOutputs(1);
    defineSamples(model->inputData());
}

RandExpectedImprovement::RandExpectedImprovement(
        KrigingSPtr model,
        const TVector<TVector<double>>& samples)
    : ExpectedImprovement(model)
{
    TP_defineNOutputs(1);
    defineSamples(samples);
}

RandExpectedImprovement::~RandExpectedImprovement()
{

}

void RandExpectedImprovement::defineSamples(const TVector<TVector<double>> &samples)
{
    m_samples = samples;
    defineDefaultBandwidth();
}

TVector<TVector<double> > RandExpectedImprovement::samples() const
{
    return m_samples;
}

void RandExpectedImprovement::evaluate(
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
        }

        outputs[0]->defineValue(expImprove);
    }
}

void RandExpectedImprovement::defineOutputPrpts()
{
    if(TP_nOutputs() != 1) {
        return;
    }
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;

    varNames << "Rand expected improvement";
    varDescriptions << "The expected improvement over the best 'known' point "
                       "to the Kriging model. Expected improvement is calculated "
                       "by considering the value for the input as a random "
                       "number, following a normal distribution. The mean is the "
                       "Kriging model estimation, and the error has an inverse "
                       "correlation to the density of the solution in decision "
                       "space.";
    typeVec << RealType;
    varUnits << "";
    TVector<OptimizationType> optTypes({Maximization});
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

double RandExpectedImprovement::value(const TVector<IElementSPtr>& inputs)
{
    TVector<double> iReal = IElementVecToRealVec(inputs);

    return model()->interpolate(iReal);
}

double RandExpectedImprovement::error(const TVector<IElementSPtr>& inputs)
{
    TVector<double> iReal = IElementVecToRealVec(inputs);

    // calculate the error
    double density = gaussianKDE(m_samples, iReal, m_bandwidth);
    double err = (std::atan(1.0 / density)) /
            (boost::math::constants::pi<double>()) * 2.0;

    // scale the error according to the function values
    err *= (worst() - best());

    return err;
}

void RandExpectedImprovement::defineBandwidth(double h)
{
    if(h > 0.0) {
        m_bandwidth = h;
    }
}

double RandExpectedImprovement::bandwidth() const
{
    return m_bandwidth;
}

void RandExpectedImprovement::defineDefaultBandwidth()
{
    int n = m_samples.size();
    if(n>0) {
        int d = m_samples[0].size();
        if(d>0) {
            // The default bandwidth is 1/100th of the mean span of the dimensions of x
            double h = 0.0;
            for(int i=0; i<d; i++) {
                TVector<double> vec(n);
                for(int j=0; j<n; j++) {
                    vec[j] = m_samples[j][i];
                }
                h += vectorMax(vec) - vectorMin(vec);
            }
            h /= d;
            h /= 100.0;

            defineBandwidth(h);
        } else {
            m_bandwidth = 0.01;
        }
    } else {
        m_bandwidth = 0.01;
    }
}

TString RandExpectedImprovement::name()
{
    return("Expected improvement with error estimated by the density of "
           "solutions in decision space");
}

TString RandExpectedImprovement::description()
{
    return("A function that estimates the solution's expected improvement over "
           "the current best known solution, based on Kriging estimation.\n"
           "Expected improvement is calculated by considering the value for the "
           "input as a random number, following a normal distribution. The mean "
           "is the Kriging model estimation, and the error has an inverse "
           "correlation to the density of the solutions in decision space.");
}

} // namespace Representation
} // namespace Tigon
