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
#include <tigon/Operators/Composites/ConstrainedSurrogateBasedOptimizer.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>

#include <tigon/Utils/Kriging/DACE.h>
#include <tigon/Utils/Kriging/OrdinaryKriging.h>
#include <tigon/Utils/Kriging/PowerVariogram.h>

#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/DominanceUtils.h>

#include <tigon/Representation/Functions/SurrogateModelling/FeasibleExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/FeasibleRandExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/ConstrainedExpectedImprovement.h>
#include <tigon/Operators/Initialisation/FitnessBasedInit.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Functions/SurrogateModelling/ProbabilityFeasibility.h>

#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Algorithms/NSGAII.h>
#include <tigon/Algorithms/ACROMUSE.h>

#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Properties/ElementProperties.h>

#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

ConstrainedSurrogateBasedOptimizer::ConstrainedSurrogateBasedOptimizer()
{
    initialise();
}

ConstrainedSurrogateBasedOptimizer::
ConstrainedSurrogateBasedOptimizer(Representation::IPSet* ipset)
    : SurrogateBasedOptimizer(ipset)
{
    initialise();
}

ConstrainedSurrogateBasedOptimizer::~ConstrainedSurrogateBasedOptimizer()
{

}

void ConstrainedSurrogateBasedOptimizer::initialise()
{
    addProperty("DisableConstraintHandling"
                , TString("Indicates whether or not constraint handling operates  "
                          "disabled or not.\n"
                          "Default is false, meaning that constraint handling "
                          "is enabled.")
                , getTType(bool));
    addProperty("NumberIterations"
                , TString("Number of Iterations to run the multi-objective "
                          "optimizer to search the surrogate.\n"
                          "Default is 150.")
                , getTType(int));

    addProperty("MultiObjectivization"
                , TString("Indicates whether a multi-objective optimizer or a "
                          "single-objective optimizer is used to optimize the "
                          "surrogate model. The multi-objective approach poses "
                          "as objectives the individual functions required for "
                          "the calculation of the constrained expected improvement. "
                          "The functions correspond to the expected improvement "
                          "and probability of feasibility.\n"
                          "Default is false, meaning that a single-objective "
                          "optimizer is used.")
                , getTType(bool));

    TP_defineDisableConstraintHandling(false);
    TP_defineNumberIterations(150);
    TP_defineMultiObjectivization(false);
}

ProblemSPtr ConstrainedSurrogateBasedOptimizer::
setupSingleObjective_CEIProblem(IFunctionSPtr ei,
                                const TVector<IFunctionSPtr>& vpf)
{
    TVector<ProbabilityFeasibility*> vpfs;
    vpfs.reserve(vpf.size());
    for(auto funcC : vpf) {
        ProbabilityFeasibility* pf =
                static_cast<ProbabilityFeasibility*>(funcC.get());
        vpfs.push_back(pf);
    }

    IFunctionSPtr funEIxPF;
    switch (TP_errorMethod())
    {
    default:
    case ErrConfidenceIntervalBased:
    {
        FeasibleExpectedImprovement* fbs =
                static_cast<FeasibleExpectedImprovement*>(ei.get());
//        ExpectedImprovement* fbs = static_cast<ExpectedImprovement*>(ei.get());
        funEIxPF = IFunctionSPtr(new ConstrainedExpectedImprovement(fbs, vpfs));
        break;
    }
    case ErrDensityBased:
    {
        FeasibleRandExpectedImprovement* rsv =
                static_cast<FeasibleRandExpectedImprovement*>(ei.get());
        funEIxPF = IFunctionSPtr(new ConstrainedExpectedImprovement(rsv, vpfs));
    }
    }
    funEIxPF->defineInputPrpts(decisionVecProperties(), boxConstraints());

    ProblemSPtr   prob(new Problem);
    prob->appendFunction(funEIxPF, TVector<int>(), TVector<int>(), TVector<int>{0});
    prob->processProblemDefinition();
    return prob;
}

ProblemSPtr ConstrainedSurrogateBasedOptimizer::
setupMultiObjective_CEIProblem(IFunctionSPtr ei,
                               const TVector<IFunctionSPtr>& vpf)
{
    // Define problem and append EI function
    ProblemSPtr prob(new Problem);
    prob->appendFunction(ei);

    int i=1;
    for(auto funcC : vpf) {

        // Change the ElementProperties of probability of feasibility functions.
        // This ensures that they are treated as separate outputs when added to
        // the problem class.
        TString name = "PF" + convertToString(i);
        TString description = "PF function for the constraint number " + convertToString(i);
        ElementPropertiesFactory* elemFact = ElementPropertiesFactory::instance();
        ElementProperties elem = elemFact->createElementProperties(i, name, description, RealType, Maximization);
        // Assumption that there is only one output in probability of feasibility function
        TVector<ElementProperties> elemVector{elem};
        funcC->defineOutputPrpts(elemVector);

        prob->appendFunction(funcC);
        i++;
    }

    prob->processProblemDefinition();
    return prob;
}

IMappingSPtr ConstrainedSurrogateBasedOptimizer::
runMultiObjOptimizer(ProblemSPtr prob)
{
    // Generate an initial set from a mixture of perturbations of best existing
    // solutions and random LHS
    FitnessBasedInit* fInit = new FitnessBasedInit(this);
    fInit->defineInputTags(inputTags());
    fInit->TP_defineSetSize(TP_initialPopsizePerVariableSS() * decisionVecSize());
    fInit->evaluateOnlyThisNode();

    // Create the inside multi-objective optimizer workflow
    PSetBase*              base = new PSetBase();
    IFormulation*          form = new IFormulation(base);
    UserDefinedInit*       init = new UserDefinedInit(form);
    Evaluator*             eval = new Evaluator(init);
    Algorithms::NSGAII*    alg  = new Algorithms::NSGAII(eval);

    form->defineProblem(prob);
    init->defineInitialDVecs(fInit->outputSet(0));
    alg->defineMaxIteration(TP_numberIterations());

    while(alg->remainingIterations() > 0) {
        alg->evaluate();
        alg->incrementIteration();
    }

    TStringList tags;
    tags << Tigon::TForNextIteration << Tigon::TForSelection;
    ISet* npop = alg->setWithTags(tags);

    // Determine EIxPF of all solutions
    TVector<double> fitness;
    fitness.reserve(npop->size());
    for(auto sol : npop->all()) {

        TVector<double> objs = sol->doubleObjectiveVec();
        double prod = 1.0;
        for(auto f : objs) {
            prod *= std::fabs(f);
        }
        fitness.push_back(prod);
    }

    // Find the solution with the highest EIxPF value
    auto maxElementIndex = std::max_element(fitness.begin(),fitness.end()) -
                           fitness.begin();
    IMappingSPtr bestSolutionNSGAII = npop->at(maxElementIndex);

    // Free allocated memory
    delete alg;
    delete eval;
    delete init;
    delete form;
    delete base;
    delete fInit;

    return bestSolutionNSGAII;
}

void ConstrainedSurrogateBasedOptimizer::evaluateNode()
{
    int nConstraints = constraintVecSize();
    if( (nConstraints==0) || TP_disableConstraintHandling() ) {
        SurrogateBasedOptimizer::evaluateNode();
        return;
    }

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
    } else {                               //Threshold indicator
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
        std::cout << "** Decomposition Failed for objective **" << std::endl;
        PowerVariogram* powerV = new PowerVariogram(xx, yy);
        km = KrigingSPtr(new OrdinaryKriging(powerV));
    }

//    PowerVariogram* powerV = new PowerVariogram(xx, yy);
//    KrigingSPtr km = KrigingSPtr(new OrdinaryKriging(powerV));
    km->defineEstimateErrors(true);


    /// 2. Calculate constraint violation of every solution to use in
    /// calculation of feasible f_min in EI

    TVector<double> cViolation;
    cViolation.reserve(iSet->size());
    TVector<double> threshVec  = IElementVecToRealVec(thresholdVec());
    for(auto sol : iSet->all()) {
        double violation = solutionConstraintViolation(sol, threshVec);
        cViolation.push_back(violation);
    }


    /// 3. Expected Improvement function

    IFunctionSPtr func;
    switch (TP_errorMethod())
    {
    case ErrConfidenceIntervalBased:
    default:
    {
        func = IFunctionSPtr(new FeasibleExpectedImprovement(km, cViolation));
//        func = IFunctionSPtr(new ExpectedImprovement(km));
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
        FeasibleRandExpectedImprovement* fun =
                new FeasibleRandExpectedImprovement(km, samps, cViolation);
        fun->defineBandwidth(TP_densityEstimationBandwidth());
        func = IFunctionSPtr(fun);
        break;
    }
    }

    func->defineInputPrpts(decisionVecProperties(), boxConstraints());


    /// 4. Build GP models of all constraints and
    /// generate the probability of feasibility function

    TVector<KrigingSPtr> ck;
    ck.reserve(nConstraints);
    TVector<IFunctionSPtr> vpf;
    vpf.reserve(nConstraints);
    for(int i=0; i<nConstraints; i++) {

        TVector<double> cy;
        cy.reserve(iSet->size());
        for(auto sol : iSet->all()) {
            cy.push_back(sol->doubleConstraintVar(i) - threshVec[i]);
        }

        // Construct the Kriging model for the constraint
        KrigingSPtr kmC;
        DACE* kinterp2 = new DACE();
        kinterp2->defineBudgetPerVariable(TP_budgetPerVariableTS());
        kinterp2->defineInitialPopsizePerVariable(TP_initialPopsizePerVariableTS());
        kinterp2->defineStallIterations(TP_stallIterationsTS());
        kinterp2->updateData(xx, cy);
        if(kinterp2->isDecompositionSuccessfull()) {
            kmC = KrigingSPtr(kinterp2);
        } else {
            delete kinterp2;
            std::cout << "** Decomposition Failed for the "
                      << std::to_string(i+1) << "th constraint **" << std::endl;
            PowerVariogram* powerVC = new PowerVariogram(xx, cy);
            kmC = KrigingSPtr(new OrdinaryKriging(powerVC));
        }

//        PowerVariogram* powerVC = new PowerVariogram(xx, cy);
//        KrigingSPtr kmC = KrigingSPtr(new OrdinaryKriging(powerVC));
        kmC->defineEstimateErrors(true);
        ck.push_back(kmC); // save the Kriging model to free memory later

        // Construct the probability of feasibility function
        ProbabilityFeasibility* pf = new ProbabilityFeasibility(kmC);
        IFunctionSPtr funcC = IFunctionSPtr(pf);
        funcC->defineInputPrpts(decisionVecProperties(), boxConstraints());
        vpf.push_back(funcC);
    }


    /// 5. Setup the problem and run an optimizer to search the surrogates

    IMappingSPtr bestSolution;
    if(TP_multiObjectivization()) {
        ProblemSPtr probMultiObj = setupMultiObjective_CEIProblem(func, vpf);
        bestSolution = runMultiObjOptimizer(probMultiObj);
//        TVector<double> objsNSGAII = bestSolution->doubleObjectiveVec();
//        double prod = 1.0;
//        for(auto elem : objsNSGAII) {
//            prod *= std::fabs(elem);
//        }
//        std::cout << "prod(NSGAII) = " << prod << std::endl;

    } else {
        ProblemSPtr probSingleObj = setupSingleObjective_CEIProblem(func, vpf);
        bestSolution = runOptimizer_AIBased(probSingleObj);
//        double prod = std::fabs(bestSolution->doubleObjectiveVar(0));
//        std::cout << "prod(ACROMUSE) = " << prod << std::endl;
    }

    /// 6. Create a new solution

    IMappingSPtr imap = createMapping();
    imap->defineDecisionVec(bestSolution->decisionVec());
    imap->defineWeightingVec(dirVec());


    /// 7. Free memory associated with GP models

    km.reset(); // free Kriging model of expected improvement function
    for(auto c : ck) {
        c.reset(); // free Kriging model of probability of feasibility function
    }
}

bool ConstrainedSurrogateBasedOptimizer::TP_disableConstraintHandling() const
{
    return m_disableConstraintHandling;
}

void ConstrainedSurrogateBasedOptimizer::TP_defineDisableConstraintHandling(bool flag)
{
    m_disableConstraintHandling = flag;
}

int ConstrainedSurrogateBasedOptimizer::TP_numberIterations() const
{
    return m_numberIterations;
}

void ConstrainedSurrogateBasedOptimizer::TP_defineNumberIterations(int iter)
{
    if(iter>0)  {
        m_numberIterations = iter;
    }
}

bool ConstrainedSurrogateBasedOptimizer::TP_multiObjectivization() const
{
    return m_multiObjectivization;
}

void ConstrainedSurrogateBasedOptimizer::
TP_defineMultiObjectivization(bool isMultiObjectivizationUsedOrNot)
{
    m_multiObjectivization = isMultiObjectivizationUsedOrNot;
}

TString ConstrainedSurrogateBasedOptimizer::name()
{
    return("Constrained Surrogate-Based Optimization");
}

TString ConstrainedSurrogateBasedOptimizer::description()
{
    return("Creates a surrogate model (Kriging) from the existing solutions and "
           "uses this model to construct the constrained expected improvement "
           "function. An optimizer searches for a solution that attains the "
           "maximum constrained expected improvement.\n"
           "The optimal solution is appended to the current set.");
}

} // namespace Operators
} // namespace Tigon

