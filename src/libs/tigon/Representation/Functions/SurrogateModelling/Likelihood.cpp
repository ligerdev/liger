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
#include <tigon/Representation/Functions/SurrogateModelling/Likelihood.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Representation {

Likelihood::Likelihood()
{
    TP_defineNInputs(1);
    TP_defineNOutputs(3);
}

Likelihood::Likelihood(const TVector<TVector<double>>& x,
                       const TVector<double>& y)
    : Likelihood()
{
    defineData(x,y);
}

Likelihood::~Likelihood()
{

}

TString Likelihood::name()
{
    return("Likelihood");
}

TString Likelihood::description()
{
    return("Concentrated ln-likelihood function");
}

void Likelihood::evaluate(const TVector<IElementSPtr> &inputs,
                          const TVector<IElementSPtr> &outputs)
{
    int N = inputs.size();
    if(N != m_xDim) {
        throw TException(className(), IncorrectNumberArguments);
    }

    TVector<double> theta; theta.reserve(N);
    for(auto elem : inputs) {
        theta.push_back(std::pow(10.0, elem->value<double>()));
    }

    TMatrixReal Gram(m_N, m_N); // Gram matrix
    for(int i=0; i<m_N-1; i++) {
        for(int j=i+1; j<m_N; j++) {

            double sum = 0.0;
            for(int k=0; k<m_xDim; k++) {
                sum += theta[k]*std::pow(std::fabs(m_x[i][k] - m_x[j][k]), 2.0);
            }
            // Add upper and lower halves
            Gram(i, j) = std::exp(-sum);
            Gram(j, i) = Gram(i, j);
        }
        // Add diagonal of ones plus a small number to reduce ill conditioning
        Gram(i, i) = 1.0 + std::numeric_limits<double>::epsilon();
    }
    Gram(m_N-1, m_N-1) = 1.0 + std::numeric_limits<double>::epsilon();

    // Standard Cholesky decomposition (LL^T) of a matrix
    Eigen::LLT<TMatrixReal> cholesky(m_N);
    cholesky.compute(Gram);

    // Check if decomposition succeed, if not, it means that the matrix is not
    // positive definite (perhaps the points are densily packed)
    if(cholesky.info() == Eigen::Success) {

        TMatrixReal U = cholesky.matrixU();
        TMatrixReal Ut = U.transpose();

        TCVectorReal y(m_y.size());
        for(size_t i=0; i<m_y.size(); i++){
            y(i) = m_y[i];
        }

        TMatrixReal one = TMatrixReal::Ones(m_N, 1);
        TMatrixReal  ot = one.transpose();

        TMatrixReal Ut_solve_y = Ut.colPivHouseholderQr().solve(y);
        TMatrixReal U_solve_Ut_solve_y = U.colPivHouseholderQr().solve(Ut_solve_y);
        TMatrixReal mu_top = ot * U_solve_Ut_solve_y;

        TMatrixReal Ut_solve_one = Ut.colPivHouseholderQr().solve(one);
        TMatrixReal U_solve_Ut_solve_one = U.colPivHouseholderQr().solve(Ut_solve_one);
        TMatrixReal mu_bottom = ot * U_solve_Ut_solve_one;

        double mu = mu_top(0) / mu_bottom(0);

        TMatrixReal residual = y - one*mu;
        TMatrixReal Ut_solve_residual = Ut.colPivHouseholderQr().solve(residual);
        TMatrixReal U_solve_Ut_solve_residual = U.colPivHouseholderQr().solve(Ut_solve_residual);
        TMatrixReal sigmaSqr = residual.transpose() * U_solve_Ut_solve_residual / m_N;

        double LnDetPsi = 0.0;
        for(int i=0; i<m_N; i++) {
            LnDetPsi += std::log(std::fabs(U(i,i)));
        }
        LnDetPsi = 2.0 * LnDetPsi;

        double LnLike = -0.5*m_N*std::log(sigmaSqr(0)) - 0.5*LnDetPsi;

        outputs[0]->defineValue(LnLike);
        outputs[1]->defineValue(mu);
        outputs[2]->defineValue(sigmaSqr(0));
        return;
    }
    else {
        outputs[0]->defineValue(Tigon::Lowest);
        outputs[1]->defineValue(0.0);
        outputs[2]->defineValue(1.0);
        return;
    }
}

void Likelihood::defineInputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec(TP_nInputs(), RealType);
    TStringList          varUnits(TP_nInputs(), TString(""));
    TVector<IElement>    lowerBounds(TP_nInputs(), IElement(RealType, -3.0));
    TVector<IElement>    upperBounds(TP_nInputs(), IElement(RealType, 2.0));

    for(int i=0; i<TP_nInputs(); i++) {
        varNames.push_back("Theta_" + std::to_string(i));
        varDescriptions.push_back("Theta_" + std::to_string(i));
    }

    createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                          lowerBounds, upperBounds);
}

void Likelihood::defineOutputPrpts()
{
    TStringList          varNames;
    TStringList          varDescriptions;
    TVector<ElementType> typeVec;
    TStringList          varUnits;
    TVector<OptimizationType> optTypes;

    varNames.push_back("LnLike");
    varDescriptions.push_back("Concentrated ln-likelihood");
    typeVec.push_back(RealType);
    varUnits.push_back("");
    optTypes.push_back(Maximization);

    varNames.push_back("MeanMLE");
    varDescriptions.push_back("MLE estimate for mean");
    typeVec.push_back(RealType);
    varUnits.push_back("");
    optTypes.push_back(NonOptimization);

    varNames.push_back("SigmaSqrMLE");
    varDescriptions.push_back("MLE estimate for sigma square");
    typeVec.push_back(RealType);
    varUnits.push_back("");
    optTypes.push_back(NonOptimization);

    createOutputProperties(varNames, varDescriptions, typeVec, varUnits, optTypes);
}

void Likelihood::defineData(const TVector<TVector<double>>& x,
                            const TVector<double>& y)
{
    if( (x.size()==0) || (y.size()==0) ) {
        return;
    }
    if(x.size() != y.size()) {
        return;
    }

    m_N    = x.size();
    m_xDim = x[0].size();

    m_x.clear();
    m_y.clear();
    m_x = x;
    m_y = y;

    TP_defineNInputs(m_xDim);
}

TVector<TVector<double>> Likelihood::xValues() const
{
    return m_x;
}

TVector<double> Likelihood::yValues() const
{
    return m_y;
}

} // namespace Representation
} // namespace Tigon
