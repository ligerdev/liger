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
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/Kriging.h>
#include <tigon/Utils/PowerVariogram.h>
#include <tigon/Representation/Functions/ExpectedImprovement.h>
#include <tigon/Representation/Functions/RandExpectedImprovement.h>
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Operators/Formulations/IFormulation.h>
#include <tigon/Operators/Initialisation/FitnessBasedInit.h>
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Operators/EmptyOperator.h>
#include <tigon/Operators/Perturbations/BoundedPerturbation.h>
#include <tigon/Algorithms/ACROMUSE.h>
#include <tigon/Representation/Properties/ElementProperties.h>

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
    addProperty("BudgetPerVariable"
                , TString("Budget Per Variable")
                , getType(int));
    addProperty("NeighbourhoodRadius"
                , TString("Neighbourhood Radius")
                , getType(double));
    //addProperty("ErrorMethod", , getType());

    TP_defineBudgetPerVariable(100);

    // an invalid radius that keeps the RandExpectedImprovement default value:
    TP_defineNeighbourhoodRadius(Tigon::UseDefaultNeighbourhoodRadius);
    TP_defineErrorMethod(ErrDensityBased);

    //defineErrorMethod(Tigon::ErrDensityBased);

    addOutputTag(Tigon::TForEvaluation);
    defineInputTags(TStringList({Tigon::TFiltration}));
}

void SurrogateBasedOptimizer::evaluateNode()
{
    ISet* oSet = outputSet(0);
    ISet* iSet = inputSet(0);

    // Fit a Kriging model to the input set
    TVector<TVector<double> > xx;
    TVector<double> yy;
    if(iSet->at(0)->doubleCost() > 0) {    // Confidence indicator
        for(int i=0; i<iSet->size(); i++) {
            // decision vector
            xx.push_back(iSet->at(i)->doubleDecisionVec());
            // function value
            yy.push_back(iSet->at(i)->doubleCost());
        }
    } else {                               //Threshold indicator
        for(int i=0; i<iSet->size(); i++) {
            // decision vector
            xx.push_back(iSet->at(i)->doubleDecisionVec());
            // function value
            yy.push_back(-iSet->at(i)->doubleCost());
        }
    }

    PowerVariogram* powerV = new PowerVariogram(xx, yy);
    KrigingSPtr km = KrigingSPtr(new Kriging(powerV));
    km->estimateErrors(true);

    // Create an IFunction from the Kriging model
    IFunctionSPtr func;
    switch (TP_errorMethod())
    {
    case ErrConfidenceIntervalBased:
    {
        func = IFunctionSPtr(new ExpectedImprovement(km));
        break;
    }
    case ErrDensityBased:
    default:
    {
        TVector<TVector<double> > samps;
        if(oSet->size() == iSet->size()) {
            samps = xx;
        } else {
            for(int i=0; i<oSet->size(); i++) {
                samps.push_back(oSet->at(i)->doubleDecisionVec());
            }
        }
        RandExpectedImprovement* fun = new RandExpectedImprovement(km, samps);
        fun->defineBandwith(Tigon::KDEBandwithRatio * TP_neighbourhoodRadius());
        func = IFunctionSPtr(fun);
        break;
    }
    }

    func->defineInputPrpts(decisionVecProperties(), boxConstraints());

    // Generate an initial set from a mixture of perturbations of best existing
    // solutions and random LHS
    FitnessBasedInit* fInit = new FitnessBasedInit(this);
    fInit->defineInputTags(inputTags());
    fInit->TP_defineSetSize(10 * decisionVecSize());
    fInit->evaluateOnlyThisNode();

    // Create a new workflow
    PSetBase*              base = new PSetBase();
    IFormulation*          prob = new IFormulation(base);
    UserDefinedInit*       init = new UserDefinedInit(prob);
    Evaluator*             eval = new Evaluator(init);
    Algorithms::ACROMUSE*  alg  = new Algorithms::ACROMUSE(eval);

    prob->appendFunction(func);
    init->defineInitialDVecs(fInit->outputSet(0));

    eval->TP_defineSingleObjective(true);

    alg->defineBudget(TP_budgetPerVariable() * decisionVecSize());

    while(alg->remainingBudget() > 0) {
        alg->evaluate();
        alg->incrementIteration();
    }

    // create a new IMapping with the same decision vector as the best solution
    IMappingSPtr imap = createMapping();
    TVector<IElementSPtr> newDVec = alg->bestMapping()->decisionVec();
    imap->defineDecisionVec(newDVec);

    delete alg;
    delete eval;
    delete init;
    delete prob;
    delete base;
    delete fInit;
    km.reset();
}

void SurrogateBasedOptimizer::TP_defineBudgetPerVariable(int n)
{
    if(n>0) {
        m_budgetPerVar = n;
    }
}

int SurrogateBasedOptimizer::TP_budgetPerVariable() const
{
    return m_budgetPerVar;
}

void SurrogateBasedOptimizer::TP_defineNeighbourhoodRadius(double r)
{
    m_neighbourhoodRadius = r;
}

double SurrogateBasedOptimizer::TP_neighbourhoodRadius() const
{
    return m_neighbourhoodRadius;
}

void SurrogateBasedOptimizer::TP_defineErrorMethod(ErrorHandlingMethod err)
{
    m_errMethod = err;
}

ErrorHandlingMethod SurrogateBasedOptimizer::TP_errorMethod() const
{
    return m_errMethod;
}


TString SurrogateBasedOptimizer::name()
{
    return TString("Surrogate-Based Optimization");
}

TString SurrogateBasedOptimizer::description()
{
    return TString("Creates a surrogate model (Kriging) from the "
                   "existing solutions and optimizes over this model.\n"
                   "The optimal solution is appended to the current "
                   "set.\n");
}

} // namespace Operators
} // namespace Tigon
