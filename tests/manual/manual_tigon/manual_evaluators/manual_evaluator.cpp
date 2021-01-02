/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <iostream>
#include <chrono>
#include <tigon/Tigon.h>

void bmk_parallelEvaluation() {
    // Tests the parallel evaluator, benchmarking against a single evaluation thread
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;

    // Long running function (Copy of DTLZ with added delay)
    class longFunc : public DTLZ1 {
    public:
        longFunc() { defineParallelisable();}
        longFunc(const longFunc& func) : DTLZ1(func) {}
        virtual ~longFunc() {}
        void evaluate(const TVector<IElementSPtr> &inputs,
                      const TVector<IElementSPtr> &outputs) {
            DTLZ1::evaluate(inputs, outputs);
        }
    };

    int dVecSize = 7;
    int oVecSize = 2;
    int populationSize = 200;
    int maxIter = 200;

    //Function Definition
    IFunctionSPtr          func  = IFunctionSPtr(new longFunc);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    IFunctionSPtr          pfunc  = IFunctionSPtr(new longFunc);
    pfunc->TP_defineNInputs(dVecSize);
    pfunc->TP_defineNOutputs(oVecSize);

    // Parallel evaluation problem
    PSetBase*              pbase = new PSetBase();
    IFormulation* pprob = new IFormulation(pbase);
    pprob->appendFunction(pfunc);
    RandomInit* pinit = new RandomInit(pprob);
    Evaluator* pevaluator = new Evaluator(pinit);
    pevaluator->TP_defineParallel(true);

    // NSGAIIPSA remake
    NonDominanceRanking* pranking     = new NonDominanceRanking(pevaluator);
    PSACrowding* pcrowding            = new PSACrowding(pranking);
    PSAEliteSelection* pelite         = new PSAEliteSelection(pcrowding);
    TournamentFiltrationForModification* pchildren = new TournamentFiltrationForModification(pelite);
    RandFiltrationForDirection* pfiltForCrossOver  = new RandFiltrationForDirection(pchildren);
    pfiltForCrossOver->addOutputTag(Tigon::TForNextIteration);
    pfiltForCrossOver->addOutputTag(Tigon::TForPerturbation);
    SinglePointCrossOver*
            pcrossOver   = new SinglePointCrossOver(pfiltForCrossOver);
    PolynomialMutation*
            ppm          = new PolynomialMutation(pcrossOver);
    MergeForNextIteration*
            pnxt         = new MergeForNextIteration(ppm);

    pinit->TP_defineSetSize(populationSize);
    pinit->addOutputTag(Tigon::TForFitness);
    pelite->addOutputTag(Tigon::TForSelection);
    pchildren->addInputTag(Tigon::TForNextIteration);
    pchildren->removeInputTag(Tigon::TFitness);
    pchildren->TP_defineChildrenRatio(1.0);

    // Serial Evaluation problem
    PSetBase*              base = new PSetBase();
    IFormulation* prob = new IFormulation(base);
    prob->appendFunction(func);
    RandomInit* init = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    evaluator->TP_defineParallel(false);
    NonDominanceRanking*
            ranking     = new NonDominanceRanking(evaluator);
    PSACrowding*
            crowding    = new PSACrowding(ranking);
    PSAEliteSelection*
            elite       = new PSAEliteSelection(crowding);
    TournamentFiltrationForModification*
            children    = new TournamentFiltrationForModification(elite);
    RandFiltrationForDirection*
            filtForCrossOver  = new RandFiltrationForDirection(children);
    filtForCrossOver->addOutputTag(Tigon::TForNextIteration);
    filtForCrossOver->addOutputTag(Tigon::TForPerturbation);
    SinglePointCrossOver*
            crossOver   = new SinglePointCrossOver(filtForCrossOver);
    PolynomialMutation*
            pm          = new PolynomialMutation(crossOver);
    MergeForNextIteration*
            nxt         = new MergeForNextIteration(pm);


    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForFitness);
    elite->addOutputTag(Tigon::TForSelection);
    children->addInputTag(Tigon::TForNextIteration);
    children->removeInputTag(Tigon::TFitness);
    children->TP_defineChildrenRatio(1.0);

    cout << "Testing single thread evaluation" << endl;
    auto start = std::chrono::system_clock::now();
    for(int iter=0; iter<maxIter; iter++){
        //cout << "Iteration " << iter << endl;
        nxt->evaluate();
        nxt->incrementIteration();
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    cout << "Single thread:   " << elapsed_seconds.count() << "ms" << endl;

    cout << "Testing parallel evaluation" << endl;
    start = std::chrono::system_clock::now();
    for(int iter=0; iter<maxIter; iter++){
        //cout << "Iteration " << iter << endl;
        pnxt->evaluate();
        pnxt->incrementIteration();
    }
    end = std::chrono::system_clock::now();
    elapsed_seconds = end-start;

    cout << "Parallel thread: " << elapsed_seconds.count() << "ms" << endl;

}

int main()
{
    cout << "Running Benchmarks..." << endl;

    bmk_parallelEvaluation();

    return 0;
}
