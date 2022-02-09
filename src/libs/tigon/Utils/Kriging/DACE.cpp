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
#include <tigon/Utils/Kriging/DACE.h>
#include <tigon/Representation/Mappings/IMapping.h>

#include <tigon/Representation/Functions/SurrogateModelling/Likelihood.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Initialisation/LHSInit.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Algorithms/ACROMUSE.h>

#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
using namespace Operators;
using Algorithms::ACROMUSE;
}

namespace Tigon {

DACE::DACE()
    : m_predictionErrorEstimate(0.0)
{
    // default values (ACROMUSE)
    defineBudgetPerVariable(DefaultBudgetPerVariableTS_H);
    defineInitialPopsizePerVariable(DefaultInitialPopsizePerVariableTS_H);
    defineStallIterations(DefaultStallIterationsTS_H);

    // default values (Model data)
    TVector<TVector<double>> x(3);
    x[0] << 0.3 << 0.1;
    x[1] << 0.4 << 0.7;
    x[2] << 0.8 << 0.3;

    TVector<double> y;
    y << 0.9 << 0.2 << 0.1;

    TVector<double> theta;
    theta << 67.023 << 75.23;

    updateData(x, y, theta);

    configureDACE();
}

void DACE::updateData(const TVector<TVector<double>>& x,
                      const TVector<double>& y,
                      const TVector<double>& theta)
{

    // There needst to be at least two points
    if(x.size() < 2) {
        return;
    }
    if(x[0].size() < 1) {
        return;
    }

    if(x.size() != y.size()) {
        return;
    }

    m_x = x;
    m_y = y;

    m_N    = m_x.size();
    m_xDim = m_x[0].size();

    if(theta.size() != m_x[0].size()) {
        m_theta.resize(m_x[0].size());
        searchTheta();
    } else {
        m_theta = theta; // user-defined theta
    }
    configureDACE();
}

DACE::~DACE()
{

}

double DACE::interpolate(const TVector<double>& xstar)
{
    if(xstar.size() != m_x[0].size()) {
        return 0.0;
    }

    TMatrixReal psi (m_N, 1);
    for(int i=0; i<m_N; i++) {
        double sum = 0.0;
        for(int k=0; k<m_xDim; k++) {
            sum += m_theta[k]*std::pow(std::fabs(m_x[i][k] - xstar[k]), 2.0);
        }
        psi(i, 0) = std::exp(-sum);
    }
    TMatrixReal psit = psi.transpose();

    TMatrixReal right_hand_side = psit * m_U_solve_Ut_solve_residual;
    double mu = m_mu + right_hand_side(0);

    // estimate errors
    if(estimateErrors()) {

        TMatrixReal Ut_solve_psi = m_Ut.colPivHouseholderQr().solve(psi);
        TMatrixReal U_solve_Ut_solve_psi = m_U.colPivHouseholderQr().solve(Ut_solve_psi);
        TMatrixReal res = psit * U_solve_Ut_solve_psi;

        // mean squared error (MSE)
        double mse = m_sigmaSqr * (1.0 - res(0));

        m_predictionErrorEstimate = std::sqrt(std::fabs(mse));
    }

    return mu;
}

double DACE::errorEstimate()
{
    return m_predictionErrorEstimate;
}

TVector<TVector<double> > DACE::inputData() const
{
    return m_x;
}

TVector<double> DACE::outputData() const
{
    return m_y;
}

// Find the hyperparameter (theta) via MLE
void DACE::searchTheta()
{
    IFunctionSPtr func = IFunctionSPtr(new Likelihood(m_x, m_y));
    TVector<int> paramInd, objInd, cnstrInd, unusedInd;
    objInd << 0;
    unusedInd << 1 << 2;
    ProblemSPtr   prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd, unusedInd);
    prob->processProblemDefinition();

    // Create a new workflow
    PSetBase*     base = new PSetBase();
    IFormulation* form = new IFormulation(base);
    LHSInit*      init = new LHSInit(form);
    Evaluator*    eval = new Evaluator(init);
    ACROMUSE*      alg = new ACROMUSE(eval);

    int nDvec = m_xDim;
    form->defineProblem(prob);
    init->TP_defineSetSize(initialPopsizePerVariable() * nDvec);
    eval->TP_defineSingleObjective(true);
    alg->defineMaxIteration(budgetPerVariable() * nDvec);

    int stallIter = stallIterations();
    TVector<double> scoresDiff;
    double prevScore = 0.0;
    while(alg->remainingIterations() > 0) {
        alg->evaluate();

        IMappingSPtr sol = alg->bestMapping();
        double currentScore = -sol->doubleCost();
        if(alg->currentIteration()==0) {
            prevScore = currentScore;
        }
        else {
            double scoreDiff = std::fabs(prevScore - currentScore);
            scoresDiff.push_back(scoreDiff);
        }

        if(scoresDiff.size() > stallIter) {
            scoresDiff.erase(scoresDiff.begin()); // delete first element
        }

        if(alg->currentIteration() > stallIter) {
            double diffMean = vectorMean(scoresDiff);
            if(diffMean < Tigon::Epsilon) {
                break;
            }
        }

        prevScore = currentScore;
        alg->incrementIteration();
    }

    TVector<double> sol = alg->bestMapping()->doubleDecisionVec();
    for(size_t i=0; i<sol.size(); i++) {
        m_theta[i] = std::pow(10.0, sol[i]);
    }

//    std::cout << "LSP iterations: " << alg->currentIteration() << std::endl;
//    std::cout << "MLE parameters: " << m_theta << std::endl;

    delete alg;
    delete eval;
    delete init;
    delete form;
    delete base;
}

void DACE::updateGramMatrix()
{
    m_Gram.resize(m_N, m_N);
    for(int i=0; i<m_N-1; i++) {
        for(int j=i+1; j<m_N; j++) {

            double sum = 0.0;
            for(int k=0; k<m_xDim; k++) {
                sum += m_theta[k]*std::pow(std::fabs(m_x[i][k] - m_x[j][k]), 2.0);
            }
            // Add upper and lower halves
            m_Gram(i, j) = std::exp(-sum);
            m_Gram(j, i) = m_Gram(i, j);
        }
        // Add diagonal of ones plus a small number to reduce ill conditioning
        m_Gram(i, i) = 1.0 + std::numeric_limits<double>::epsilon();
    }
    m_Gram(m_N-1, m_N-1) = 1.0 + std::numeric_limits<double>::epsilon();
}

void DACE::configureDACE()
{
    updateGramMatrix();

    // Standard Cholesky decomposition (LL^T) of a matrix
    Eigen::LLT<TMatrixReal> cholesky(m_N);
    cholesky.compute(m_Gram);

    // Check if decomposition succeed, if not, it means that the matrix is not
    // positive definite (perhaps the points are densily packed)
    if(cholesky.info() == Eigen::Success) {
        m_isDecompositionSuccessfull = true;
    } else {
        m_isDecompositionSuccessfull = false;
    }

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

    // Matrices or vectors
    m_U = U;
    m_Ut = Ut;
    m_U_solve_Ut_solve_residual = U_solve_Ut_solve_residual;

    // scalars
    m_mu = mu;
    m_sigmaSqr = sigmaSqr(0);
}

void DACE::defineBudgetPerVariable(int n)
{
    if(n>0) {
        m_budgetPerVar = n;
    }
}

int DACE::budgetPerVariable() const
{
    return m_budgetPerVar;
}

void DACE::defineInitialPopsizePerVariable(int n)
{
    if(n>0) {
        m_initialPopsizePerVariable = n;
    }
}

int DACE::initialPopsizePerVariable() const
{
    return m_initialPopsizePerVariable;
}


void DACE::defineStallIterations(int iter)
{
    if(iter>0) {
        m_stallIterations = iter;
    }
}

int DACE::stallIterations() const
{
    return m_stallIterations;
}

TVector<double> DACE::theta() const
{
    return m_theta;
}

bool DACE::isDecompositionSuccessfull() const
{
    return m_isDecompositionSuccessfull;
}


} // namespace Tigon
