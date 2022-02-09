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
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>

#include <tigon/Utils/Kriging/DACE.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/PowerVariogram.h>
#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/RandExpectedImprovement.h>

#include <tigon/Operators/Initialisation/FitnessBasedInit.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Algorithms/ACROMUSE.h>

#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

SurrogateBasedOptimizer::SurrogateBasedOptimizer()
{
    initialise();
}

SurrogateBasedOptimizer::SurrogateBasedOptimizer(Representation::IPSet* ipset)
    : IComposite(ipset)
{
    initialise();
}

SurrogateBasedOptimizer::~SurrogateBasedOptimizer()
{

}

void SurrogateBasedOptimizer::initialise()
{
    addProperty("OptimizationSearchQuality"
                , TString("Sets different parameters that influence the tasks of "
                          "training and searching the surrogate. Two options are "
                          "available:\n"
                          "1: produces better quality results (e.g. better "
                          "convergence) at the expense of having to wait longer "
                          "for the optimization task to complete.\n"
                          "0: produces lower quality results (e.g. weaker "
                          "convergence) but the optimization task runs faster.\n"
                          "Default is 1.")
                , getTType(int));

    TP_defineOptimizationSearchQuality(1);

    TString bpvSS(std::to_string(TP_budgetPerVariableSS()));
    addProperty("BudgetPerVariableSS"
                , TString("Budget per variable used by the optimizer to search "
                          "the surrogate.\n"
                          "Default is " + bpvSS + ".")
                , getTType(int));

    TString ippvSS(std::to_string(TP_initialPopsizePerVariableSS()));
    addProperty("InitialPopsizePerVariableSS"
                , TString("Initial population size per variable used by the "
                          "optimizer to search the surrogate.\n"
                          "Default is " + ippvSS + ".")
                , getTType(int));

    TString siSS(std::to_string(TP_stallIterationsSS()));
    addProperty("StallIterationsSS"
                , TString("The number of consecutive iterations used by the "
                          "termination criterion of the optimizer to search the "
                          "surrogate.\n"
                          "Default is " + siSS + ".")
                , getTType(int));

    TString bpvTS(std::to_string(TP_budgetPerVariableTS()));
    addProperty("BudgetPerVariableTS"
                , TString("Budget per variable used by the optimizer to train "
                          "the surrogate.\n"
                          "Default is " + bpvSS + ".")
                , getTType(int));

    TString ippvTS(std::to_string(TP_initialPopsizePerVariableTS()));
    addProperty("InitialPopsizePerVariableTS"
                , TString("Initial population size per variable used by the "
                          "optimizer to train the surrogate.\n"
                          "Default is " + ippvTS + ".")
                , getTType(int));

    TString siTS(std::to_string(TP_stallIterationsTS()));
    addProperty("StallIterationsTS"
                , TString("The number of consecutive iterations used by the "
                          "termination criterion of the optimizer to train the "
                          "surrogate.\n"
                          "Default is " + siTS + ".")
                , getTType(int));


    addProperty("DensityEstimationBandwidth"
                , TString("Bandwidth for the density estimation approach used by "
                          "RandExpectedImprovement. When this value is negative "
                          "the bandwidth is chosen by RandExpectedImprovement.\n"
                          "Default is -1.")
                , getTType(double));

    TP_defineDensityEstimationBandwidth(-1.0);
    TP_defineErrorMethod(Tigon::ErrConfidenceIntervalBased);

    addOutputTag(Tigon::TForEvaluation);
    defineInputTags(TStringList({Tigon::TFiltration}));
}

void SurrogateBasedOptimizer::evaluateNode()
{
    ISet* oSet = outputSet(0);
    ISet* iSet = inputSet(0);

    /// 1. Build GP model for the Expected Improvement

    // Fit a Kriging model to the input set
    TVector<TVector<double> > xx;
    TVector<double> yy;
    if(iSet->at(0)->doubleCost() > 0) {    // Confidence indicator
        for(auto sol : iSet->all()) {
            // decision vector
            xx.push_back(sol->doubleDecisionVec());
            // function value
            yy.push_back(sol->doubleCost());
        }
    } else {                               // Threshold indicator
        for(auto sol : iSet->all()) {
            // decision vector
            xx.push_back(sol->doubleDecisionVec());
            // function value
            yy.push_back(-sol->doubleCost());
        }
    }

    KrigingSPtr km;
    DACE* kinterp = new DACE();
    kinterp->defineBudgetPerVariable(TP_budgetPerVariableTS());
    kinterp->defineInitialPopsizePerVariable(TP_initialPopsizePerVariableTS());
    kinterp->defineStallIterations(TP_stallIterationsTS());
    kinterp->updateData(xx, yy);
    if(kinterp->isDecompositionSuccessfull()) {
        km = KrigingSPtr(kinterp);
    } else {
        delete kinterp;
        std::cout << "** Decomposition Failed **" << std::endl;
        PowerVariogram* powerV = new PowerVariogram(xx, yy);
        km = KrigingSPtr(new OrdinaryKriging(powerV));
    }

//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);

    /// 2. Expected Improvement function

    // Create an IFunction from the Kriging model
    IFunctionSPtr func;
    switch (TP_errorMethod())
    {
    case ErrConfidenceIntervalBased:
    default:
    {
        func = IFunctionSPtr(new ExpectedImprovement(km));
        break;
    }
    case ErrDensityBased:
    {
        TVector<TVector<double> > samps;
        if(oSet->size() == iSet->size()) {
            samps = xx;
        } else {
            for(auto sol : oSet->all()) {
                samps.push_back(sol->doubleDecisionVec());
            }
        }
        RandExpectedImprovement* fun = new RandExpectedImprovement(km, samps);
        fun->defineBandwidth(TP_densityEstimationBandwidth());
        func = IFunctionSPtr(fun);
        break;
    }
    }

    func->defineInputPrpts(decisionVecProperties(), boxConstraints());


    /// 3. Run an optimizer to search the surrogates

    ProblemSPtr   prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();
    IMappingSPtr sol = runOptimizer_AIBased(prob);


    /// 4. Create a new solution

    // create a new IMapping with the same decision vector as the best solution
    IMappingSPtr imap = createMapping();
    TVector<IElementSPtr> newDVec = sol->decisionVec();
    imap->defineDecisionVec(newDVec);
    imap->defineWeightingVec(dirVec());


    /// 5. Free memory associated with GP model
    km.reset();
}

IMappingSPtr SurrogateBasedOptimizer::runOptimizer_FEBased(ProblemSPtr prob)
{
    // Generate an initial set from a mixture of perturbations of best existing
    // solutions and random LHS
    FitnessBasedInit* fInit = new FitnessBasedInit(this);
    fInit->defineInputTags(inputTags());
    fInit->TP_defineSetSize(TP_initialPopsizePerVariableSS() *
                            decisionVecSize());
    fInit->evaluateOnlyThisNode();

    // Create a new workflow
    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    UserDefinedInit*       init = new UserDefinedInit(form);
    Evaluator*             eval = new Evaluator(init);
    Algorithms::ACROMUSE*  alg  = new Algorithms::ACROMUSE(eval);

    form->defineProblem(prob);
    init->defineInitialDVecs(fInit->outputSet(0));
    eval->TP_defineSingleObjective(true);
    alg->defineBudget(TP_budgetPerVariableSS() * decisionVecSize());

    while(alg->remainingBudget() > 0) {
        alg->evaluate();
        alg->incrementIteration();
    }

    IMappingSPtr sol = alg->bestMapping();

    delete alg;
    delete eval;
    delete init;
    delete form;
    delete base;
    delete fInit;

    return sol;
}

IMappingSPtr SurrogateBasedOptimizer::runOptimizer_AIBased(ProblemSPtr prob)
{
    // Generate an initial set from a mixture of perturbations of best existing
    // solutions and random LHS
    FitnessBasedInit* fInit = new FitnessBasedInit(this);
    fInit->defineInputTags(inputTags());
    fInit->TP_defineSetSize(TP_initialPopsizePerVariableSS() *
                            decisionVecSize());
    fInit->evaluateOnlyThisNode();

    // Create a new workflow
    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    UserDefinedInit*       init = new UserDefinedInit(form);
    Evaluator*             eval = new Evaluator(init);
    Algorithms::ACROMUSE*  alg  = new Algorithms::ACROMUSE(eval);

    form->defineProblem(prob);
    init->defineInitialDVecs(fInit->outputSet(0));
    eval->TP_defineSingleObjective(true);
    alg->defineMaxIteration(TP_budgetPerVariableSS() * decisionVecSize());

    int stallIter = TP_stallIterationsSS();
    TVector<double> scoresDiff;
    double prevScore = 0.0;
    while(alg->remainingIterations() > 0) {
        alg->evaluate();

        double currentScore = -alg->bestMapping()->doubleObjectiveVar(0);
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
    //std::cout << "SS iterations: " << alg->currentIteration() << std::endl;

    IMappingSPtr sol = alg->bestMapping();

    delete alg;
    delete eval;
    delete init;
    delete form;
    delete base;
    delete fInit;

    return sol;
}

void SurrogateBasedOptimizer::TP_defineBudgetPerVariableSS(int n)
{
    if(n>0) {
        m_budgetPerVarSS = n;
    }
}

int SurrogateBasedOptimizer::TP_budgetPerVariableSS() const
{
    return m_budgetPerVarSS;
}

void SurrogateBasedOptimizer::TP_defineInitialPopsizePerVariableSS(int n)
{
    if(n>0) {
        m_initialPopsizePerVariableSS = n;
    }
}

int SurrogateBasedOptimizer::TP_initialPopsizePerVariableSS() const
{
    return m_initialPopsizePerVariableSS;
}

void SurrogateBasedOptimizer::TP_defineStallIterationsSS(int iter)
{
    if(iter>0) {
        m_stallIterationsSS = iter;
    }
}

int SurrogateBasedOptimizer::TP_stallIterationsSS() const
{
    return m_stallIterationsSS;
}

void SurrogateBasedOptimizer::TP_defineBudgetPerVariableTS(int n)
{
    if(n>0) {
        m_budgetPerVarTS = n;
    }
}

int SurrogateBasedOptimizer::TP_budgetPerVariableTS() const
{
    return m_budgetPerVarTS;
}

void SurrogateBasedOptimizer::TP_defineInitialPopsizePerVariableTS(int n)
{
    if(n>0) {
        m_initialPopsizePerVariableTS = n;
    }
}

int SurrogateBasedOptimizer::TP_initialPopsizePerVariableTS() const
{
    return m_initialPopsizePerVariableTS;
}

void SurrogateBasedOptimizer::TP_defineStallIterationsTS(int iter)
{
    if(iter>0) {
        m_stallIterationsTS = iter;
    }
}

int SurrogateBasedOptimizer::TP_stallIterationsTS() const
{
    return m_stallIterationsTS;
}


double SurrogateBasedOptimizer::TP_densityEstimationBandwidth() const
{
    return m_densityEstimationBandwidth;
}

void SurrogateBasedOptimizer::TP_defineDensityEstimationBandwidth(double bandwidth)
{
    m_densityEstimationBandwidth = bandwidth;
}

void SurrogateBasedOptimizer::TP_defineErrorMethod(ErrorHandlingMethod err)
{
    m_errMethod = err;
}

ErrorHandlingMethod SurrogateBasedOptimizer::TP_errorMethod() const
{
    return m_errMethod;
}

///
/// \brief SurrogateBasedOptimizer::defineOptimizationSearchQuality
/// Sets different parameters that influence the tasks of training and searching
/// the surrogate.
/// \param mode
/// mode=1 produces better quality results (e.g. better convergence) at the
/// expense of having to wait longer for the optimization task to complete.
/// mode=0: produces lower quality results (e.g. weaker convergence) but the
/// optimization task runs faster.
///
void SurrogateBasedOptimizer::TP_defineOptimizationSearchQuality(int mode)
{
    switch(mode)
    {
    case 0: // Parameters for weaker quality results
        // Search the surrogate parameters
        TP_defineBudgetPerVariableSS(Tigon::DefaultBudgetPerVariableSS_L);
        TP_defineInitialPopsizePerVariableSS(Tigon::DefaultInitialPopsizePerVariableSS_L);
        TP_defineStallIterationsSS(Tigon::DefaultStallIterationsSS_L);

        // Train the surrogate parameters
        TP_defineBudgetPerVariableTS(Tigon::DefaultBudgetPerVariableTS_L);
        TP_defineInitialPopsizePerVariableTS(Tigon::DefaultInitialPopsizePerVariableTS_L);
        TP_defineStallIterationsTS(Tigon::DefaultStallIterationsTS_L);
        break;
    case 1: // Parameters for better quality results
    default:
        // Search the surrogate parameters
        TP_defineBudgetPerVariableSS(Tigon::DefaultBudgetPerVariableSS_H);
        TP_defineInitialPopsizePerVariableSS(Tigon::DefaultInitialPopsizePerVariableSS_H);
        TP_defineStallIterationsSS(Tigon::DefaultStallIterationsSS_H);

        // Train the surrogate parameters
        TP_defineBudgetPerVariableTS(Tigon::DefaultBudgetPerVariableTS_H);
        TP_defineInitialPopsizePerVariableTS(Tigon::DefaultInitialPopsizePerVariableTS_H);
        TP_defineStallIterationsTS(Tigon::DefaultStallIterationsTS_H);
        break;
    }
    m_mode = mode;
}

int SurrogateBasedOptimizer::TP_optimizationSearchQuality() const
{
    return m_mode;
}

TString SurrogateBasedOptimizer::name()
{
    return("Surrogate-Based Optimization");
}

TString SurrogateBasedOptimizer::description()
{
    return("Creates a surrogate model (Kriging) from the existing solutions and "
           "uses this model to construct the expected improvement function. An "
           "optimizer is used to search for a solution that attains the maximum "
           "expected improvement.\n"
           "The optimal solution is appended to the current set.\n");
}

} // namespace Operators
} // namespace Tigon
