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
#include <tigon/Representation/Functions/RandExpectedImprovement.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/Kriging.h>
#include <tigon/Utils/KrigingVariogram.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Utils/KernelDensityEstimation.h>
#include <tigon/Representation/Distributions/NormalDistribution.h>

#include <boost/math/constants/constants.hpp>

namespace Tigon {
namespace Representation {

RandExpectedImprovement::RandExpectedImprovement()
{

}

RandExpectedImprovement::RandExpectedImprovement(KrigingSPtr model)
    : ExpectedImprovement(model)
    , m_samples(model->variogram()->inputData())
{
}

RandExpectedImprovement::RandExpectedImprovement(KrigingSPtr model,
                                                 TVector<TVector<double> > samples)
    : ExpectedImprovement(model)
    , m_samples(samples)
{
}

RandExpectedImprovement::~RandExpectedImprovement()
{

}

TString RandExpectedImprovement::name()
{
    return("Random Expected Improvement");
}

TString RandExpectedImprovement::description()
{
    return("A function that estimates the solutin's expected improvement over "
           "the current best known solution, based on Kriging estimation.\n"
           "Expected improvement is calculated by considering the value for the "
           "input as a random number, following a normal distribution. The mean "
           "is the Kriging model estimation, and the error has an inverse "
           "correlation to the density of the solution in decision space.");
}

void RandExpectedImprovement::evaluate(const TVector<IElementSPtr> &inputs,
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

        outputs[0]->defineValue(-expImprove);
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
    TVector<OptimizationType> optTypes(TP_nOutputs(), Minimization);

    varNames << "Expected_Improvement";
    varDescriptions << "The expected improvement over the best 'known' point "
                       "to the Kriging model. Expected improvement is calculated "
                       "by considering the value for the input as a random "
                       "number, following a normal distribution. The mean "
                       "is the Kriging model estimation, and the error has an inverse "
                       "correlation to the density of the solution in decision space.";
    typeVec << RealType;
    varUnits << "" << "";
    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

double RandExpectedImprovement::value(TVector<IElementSPtr> inputs)
{
    TVector<double> iReal = IElementVecToRealVec(inputs);

    return model()->interpolate(iReal);
}

double RandExpectedImprovement::error(TVector<IElementSPtr> inputs)
{
    TVector<double> iReal = IElementVecToRealVec(inputs);

    // calculate the error
    double density = gaussianKDE(m_samples, iReal, m_bandwidth);
    double err = (std::atan(1.0 / density)) / (boost::math::constants::pi<double>()) * 2.0;

    // scale the error according to the function values
    err *= (worst() - best());

    return err;
}

void RandExpectedImprovement::defineBandwith(double h)
{
    if(h > 0.0) {
        m_bandwidth = h;
    }
}

double RandExpectedImprovement::bandwith() const
{
    return m_bandwidth;
}

void RandExpectedImprovement::defineDefaultBandwith()
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

            defineBandwith(h);
        } else {
            m_bandwidth = 0.01;
        }
    } else {
        m_bandwidth = 0.01;
    }
}

} // namespace Representation
} // namespace Tigon
