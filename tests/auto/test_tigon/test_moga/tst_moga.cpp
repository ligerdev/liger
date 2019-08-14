/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#include <QtTest>
#include <QDebug>

#include <iostream>
#include <tigon/Tigon.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/IElementUtils.h>
#include <tigon/Utils/NormalisationUtils.h>


#define TST_MOGA_OPERATORS
#define WORK_FLOW
#define ALGORITHM

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_moga : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_MOGA_OPERATORS
    /* Add the necessary operators */
    void test_Ranking();
    void test_AverageFitness();

    void test_RankingExample();
    void test_AverageFitnessExample();
    void test_SharedFitnessExample();

    void test_RouletteWheelSelection();
    void test_StochasticUniversalSampling();
#endif

#ifdef WORK_FLOW
    void test_MOGA_workflow();
#endif
#ifdef ALGORITHM
    void test_MOGA();
#endif
};

/*
 * Dataset corresponding to the decision variable values for optimization
 * problem Min-Ex, taken from Deb's book, Multi-Objective Optimization using
 * Evolutionary Algorithms, Table 16, page 194.
 */
TVector<TVector<double>> dataset = {{0.31, 0.89},
                                    {0.43, 1.92},
                                    {0.22, 0.56},
                                    {0.59, 3.63},
                                    {0.66, 1.41},
                                    {0.83, 2.51}};

void inline TCOMPARE(tribool actual, tribool expected)
{
    bool flag;

    if(actual.value == expected.value) {
        flag = true;
    } else {
        flag = false;
    }

    QCOMPARE(flag, true);
}

void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(size_t i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

#ifdef TST_MOGA_OPERATORS

void tst_moga::test_Ranking()
{
    int dVecSize = 6;  // number of inputs
    int oVecSize = 3;  // number of outputs
    int populationSize = 20;

    PSetBase*       base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    ProblemSPtr prob     = pg->problem();

    TString funcName   = "Tigon::Representation::DTLZ2";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    RandomInit* init = new RandomInit(pg);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);
    FreezeOperatorOutput* forDirClone = new FreezeOperatorOutput(evaluator);
    forDirClone->defineInputTags(init->outputTags());

    evaluator->evaluate();

    ISet* set = evaluator->outputSet(0);

    int N = set->size();

    TVector<int> domCount(N, 0);
    tribool dom;
    for(int i=0; i<N-1; i++) {
        IMappingSPtr a = set->at(i);
        for(int j=i+1; j<N; j++) {
            IMappingSPtr b = set->at(j);
            dom = *a <= *b;
            if(dom == true) {
                domCount[j]++;
            } else if(dom == false) {
                domCount[i]++;
            }
        }
    }

    MOGANonDominanceRanking* mogaranking = new MOGANonDominanceRanking(forDirClone);

    mogaranking->evaluate();

    TVector<ISet*> mogranks = mogaranking->outputSets();
    cout << "Number of different ranks (Number of output sets): " <<
            mogranks.size() << "" << endl;

    ISet* pop  = init->outputSet(0);

    QCOMPARE(bool(pop->size()>0), true);

    for(int i=0; i<pop->size(); i++) {

        IMappingSPtr operatorpop  = pop->at(i);
        double mogaRank = operatorpop->doubleCost();
        QCOMPARE(areDoublesEqual(domCount[i], mogaRank), true);

        cout << "Test Dominance rank: " << domCount[i] <<
                "\t MOGADominance rank: " << mogaRank <<endl;
    }
}

void tst_moga::test_AverageFitness()
{
    int dVecSize = 6;  // number of inputs
    int oVecSize = 3;  // number of outputs
    int populationSize = 20;

    PSetBase*       base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    ProblemSPtr prob     = pg->problem();

    TString funcName   = "Tigon::Representation::DTLZ2";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    RandomInit* init = new RandomInit(pg);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);
    MOGANonDominanceRanking* ranking = new MOGANonDominanceRanking(evaluator);

    // Makes a copy of the main optimization set after MOGADominanceRanking operates
    FreezeOperatorOutput* forDirClone = new FreezeOperatorOutput(ranking);
    forDirClone->defineInputTags(init->outputTags());

    AverageFitness* avgfitness = new AverageFitness(forDirClone);
    avgfitness->defineOutputTags(init->outputTags());

    avgfitness->evaluate();

    ISet* pop_before = forDirClone->outputSet(0);
    ISet* pop_after  = init->outputSet(0);

    QCOMPARE(bool(pop_after->size()>0), true);

    for(int i=0; i<populationSize; i++) {

        IMappingSPtr before = pop_before->at(i);
        IMappingSPtr after  = pop_after->at(i);

        double rank = before->doubleCost();
        double averageFitness = after->doubleCost();

        cout << "MOGA Dominance Rank:  " << rank <<
                " \t MOGA Average fitness: " << averageFitness <<endl;
    }
}

void tst_moga::test_RankingExample()
{
    PSetBase*       base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);

    TString funcName   = "Tigon::Representation::MinEx";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    UserDefinedInit* init = new UserDefinedInit(pg);
    init->defineInitialDVecs(dataset);

    Evaluator* evaluator = new Evaluator(init);

    /* Algorithm */

    MOGANonDominanceRanking* ranking = new MOGANonDominanceRanking(evaluator);

    ranking->evaluate();

    ISet* pop = ranking->outputSet(0);

    cout << "Check average fitness values with Table 16 from Deb's book." << endl;
    cout << "x1\tx2\tf1\tf2\tRank" << endl;

    for(IMappingSPtr sol : pop->all()) {
        TVector<double> dvec = sol->doubleDecisionVec();
        TVector<double> objs = sol->doubleObjectiveVec();
        double rank = sol->doubleCost();

        for(auto d : dvec) {
            cout << d << "\t";
        }
        for(auto o : objs) {
            cout << o << "\t";
        }
        cout << rank << endl;
    }

    ISet* population = init->outputSet(0);
    TVector<IElementSPtr> ranks;
    for(IMappingSPtr sol : population->all()) {
        ranks.push_back(sol->cost());
    }

    TCOMPARE(*ranks[4] == *ranks[2], true);
    TCOMPARE(*ranks[2] == *ranks[0], true);
    TCOMPARE(*ranks[0] <  *ranks[1], true);
    TCOMPARE(*ranks[1] == *ranks[5], true);
    TCOMPARE(*ranks[5] <  *ranks[3], true);
}

void tst_moga::test_AverageFitnessExample()
{
    PSetBase*       base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);

    TString funcName   = "Tigon::Representation::MinEx";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    UserDefinedInit* init = new UserDefinedInit(pg);
    init->defineInitialDVecs(dataset);

    Evaluator* evaluator = new Evaluator(init);

    /* Algorithm */

    MOGANonDominanceRanking* ranking = new MOGANonDominanceRanking(evaluator);

    AverageFitness*        avgfit = new AverageFitness(ranking);
    avgfit->defineOutputTags(TStringList{Tigon::TMainOptimizationSet});

    avgfit->evaluate();

    ISet* pop = avgfit->outputSet(0);

    cout << "Check average fitness values with Table 16 from Deb's book." << endl;
    cout << "x1\tx2\tf1\tf2\taverage fitness" << endl;

    for(IMappingSPtr sol : pop->all()) {
        TVector<double> dvec = sol->doubleDecisionVec();
        TVector<double> objs = sol->doubleObjectiveVec();
        double fitness = sol->doubleCost();

        for(auto d : dvec) {
            cout << d << "\t";
        }
        for(auto o : objs) {
            cout << o << "\t";
        }
        cout << fitness << endl;
    }

    ISet* population = init->outputSet(0);
    TVector<IElementSPtr> sfitness;
    for(IMappingSPtr sol : population->all()) {
        sfitness.push_back(sol->cost());
    }

    TCOMPARE(*sfitness[4] == *sfitness[2], true);
    TCOMPARE(*sfitness[2] == *sfitness[0], true);
    TCOMPARE(*sfitness[0] >  *sfitness[1], true);
    TCOMPARE(*sfitness[1] == *sfitness[5], true);
    TCOMPARE(*sfitness[5] >  *sfitness[3], true);
}

void tst_moga::test_SharedFitnessExample()
{
    PSetBase*       base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);

    TString funcName   = "Tigon::Representation::MinEx";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    UserDefinedInit* init = new UserDefinedInit(pg);
    init->defineInitialDVecs(dataset);

    Evaluator* evaluator = new Evaluator(init);

    /* Algorithm */

    MOGANonDominanceRanking* ranking = new MOGANonDominanceRanking(evaluator);

    AverageFitness*        avgfit = new AverageFitness(ranking);
    avgfit->defineOutputTags(TStringList{Tigon::TMainOptimizationSet});

    SharedFitness*         shafit = new SharedFitness(avgfit);
    shafit->evaluate();

    TVector<ISet*> ranks = shafit->outputSets();
    int nranks = ranks.size();
    QCOMPARE(nranks, 3);

    cout << "x1\tx2\tf1\tf2\scaled fitness" << endl;
    for(int i=0; i<nranks; i++) {
        ISet* rank = ranks[i];
        for(IMappingSPtr sol : rank->all()) {
            TVector<double> dvec = sol->doubleDecisionVec();
            TVector<double> objs = sol->doubleObjectiveVec();
            double fitness = sol->doubleCost();

            for(auto d : dvec) {
                cout << d << "\t";
            }
            for(auto o : objs) {
                cout << o << "\t";
            }
            cout << fitness << endl;
        }
    }

    ISet* pop = init->outputSet(0);
    TVector<IElementSPtr> sfitness;
    for(IMappingSPtr sol : pop->all()) {
        sfitness.push_back(sol->cost());
    }

    TCOMPARE(*sfitness[4] >  *sfitness[2], true);
    TCOMPARE(*sfitness[2] == *sfitness[0], true);
    TCOMPARE(*sfitness[0] >  *sfitness[1], true);
    TCOMPARE(*sfitness[1] == *sfitness[5], true);
    TCOMPARE(*sfitness[5] >  *sfitness[3], true);
}

void tst_moga::test_RouletteWheelSelection()
{
    PSetBase*        base = new PSetBase();
    ProblemGenerator*  pg = new ProblemGenerator(base);

    TString funcName   = "Tigon::Representation::DTLZ1";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    TVector<double> fitness = {25.0, 5.0, 40.0, 10.0, 20.0};
    IMappingSPtr imap1(new IMapping(prob)); imap1->defineCost(fitness[0]);
    IMappingSPtr imap2(new IMapping(prob)); imap2->defineCost(fitness[1]);
    IMappingSPtr imap3(new IMapping(prob)); imap3->defineCost(fitness[2]);
    IMappingSPtr imap4(new IMapping(prob)); imap4->defineCost(fitness[3]);
    IMappingSPtr imap5(new IMapping(prob)); imap5->defineCost(fitness[4]);

    TVector<IMappingSPtr> set = {imap1, imap2, imap3, imap4, imap5};
    for(auto sol : set) {
        sol->defineEvaluated(true);
    }

    UserDefinedInit* init = new UserDefinedInit(pg);
    init->defineInitialSet(new ISet(set));
    init->addOutputTag(Tigon::TFitness);
    init->addOutputTag(Tigon::TForSelection);

    RouletteWheelSelection* sel = new RouletteWheelSelection(init);
    int nSamples = 10000;
    sel->TP_defineOutputSetSize(nSamples);

    sel->evaluate();

    ISet* oSet = sel->outputSet(0);

    QCOMPARE(oSet->size()==nSamples, true);

    int N = fitness.size();
    TVector<int> counter(N,0);
    for(int i=0; i<N; i++) {
        for(IMappingSPtr sol: oSet->all()) {
            if( areDoublesEqual(fitness[i],sol->doubleCost()) ) {
                counter[i]++;
            }
        }
        cout << counter[i] << " " << endl;
    }

    QCOMPARE(counter[2]>counter[0],true);
    QCOMPARE(counter[0]>counter[4],true);
    QCOMPARE(counter[4]>counter[3],true);
    QCOMPARE(counter[3]>counter[1],true);
}

void tst_moga::test_StochasticUniversalSampling()
{
    PSetBase*        base = new PSetBase();
    ProblemGenerator*  pg = new ProblemGenerator(base);

    TString funcName   = "Tigon::Representation::DTLZ1";
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    TVector<double> fitness = {25.0, 5.0, 40.0, 10.0, 20.0};
    IMappingSPtr imap1(new IMapping(prob)); imap1->defineCost(fitness[0]);
    IMappingSPtr imap2(new IMapping(prob)); imap2->defineCost(fitness[1]);
    IMappingSPtr imap3(new IMapping(prob)); imap3->defineCost(fitness[2]);
    IMappingSPtr imap4(new IMapping(prob)); imap4->defineCost(fitness[3]);
    IMappingSPtr imap5(new IMapping(prob)); imap5->defineCost(fitness[4]);

    TVector<IMappingSPtr> set = {imap1, imap2, imap3, imap4, imap5};
    for(auto sol : set) {
        sol->defineEvaluated(true);
    }

    UserDefinedInit* init = new UserDefinedInit(pg);
    init->defineInitialSet(new ISet(set));
    init->addOutputTag(Tigon::TFitness);
    init->addOutputTag(Tigon::TForSelection);

    StochasticUniversalSampling* sel = new StochasticUniversalSampling(init);
    int nSamples = 10000;
    sel->TP_defineOutputSetSize(nSamples);

    sel->evaluate();

    ISet* oSet = sel->outputSet(0);

    QCOMPARE(oSet->size()==nSamples, true);

    int N = fitness.size();
    TVector<int> counter(N,0);
    for(int i=0; i<N; i++) {
        for(IMappingSPtr sol: oSet->all()) {
            if( areDoublesEqual(fitness[i],sol->doubleCost()) ) {
                counter[i]++;
            }
        }
        cout << counter[i] << " " << endl;
    }

    QCOMPARE(counter[2]>counter[0],true);
    QCOMPARE(counter[0]>counter[4],true);
    QCOMPARE(counter[4]>counter[3],true);
    QCOMPARE(counter[3]>counter[1],true);
}

#endif

#ifdef WORK_FLOW
void tst_moga::test_MOGA_workflow()
{
    int dVecSize = 6;
    int oVecSize = 2;
    int populationSize = 100;
    int maxiter = 5;
    int budget = populationSize*maxiter;

    cout.precision(5);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init     = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);
    evaluator->defineBudget(budget);

    /* Algorithm */

    MOGANonDominanceRanking* ranking = new MOGANonDominanceRanking(evaluator);

    AverageFitness*        avgfit = new AverageFitness(ranking);
    avgfit->defineOutputTags(TStringList{Tigon::TMainOptimizationSet});

    SharedFitness*         shafit = new SharedFitness(avgfit);

    StochasticUniversalSampling* elite = new StochasticUniversalSampling(shafit);
    elite->defineInputTags(TStringList{Tigon::TMainOptimizationSet});
    elite->addOutputTag(Tigon::TForSelection);

    RandFiltrationForDirection* filtered = new RandFiltrationForDirection(elite);
    filtered->addInputTag(Tigon::TMatingPool);
    filtered->addOutputTag(Tigon::TForNextIteration);
    filtered->addOutputTag(Tigon::TForPerturbation);

    SBXCrossOver* crossOver = new SBXCrossOver(filtered);
    crossOver->TP_defineSolutionCrossoverProbability(0.9);

    PolynomialMutation* pm = new PolynomialMutation(crossOver);
    pm->TP_defineSolutionMutationProbability(0.3);

    MergeForNextIteration* nxt = new MergeForNextIteration(pm);

    while(nxt->remainingBudget() > 0) {
        nxt->evaluate();
        nxt->incrementIteration();

        cout << "Remaining budget: " << nxt->remainingBudget() << endl;
    }

    delete nxt;
    delete pm;
    delete crossOver;
    delete filtered;
    delete elite;
    delete shafit;
    delete avgfit;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

#endif

#ifdef ALGORITHM
void tst_moga::test_MOGA()
{
    int iter;
    int dVecSize = 12;
    int oVecSize = 2;
    int populationSize = 50;
    int maxIter = 5;

    cout.precision(5);
    //TRAND.defineSeed(465654);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    MOGA* alg = new MOGA(evaluator);

    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        // Elite population selected after Stochastic Universal Sampling
        TStringList tags1;
        tags1 << Tigon::TMatingPool << Tigon::TForSelection;
        ISet* ndPop = alg->setWithTags(tags1);
        QCOMPARE(ndPop->size(),populationSize);

        // Print elite population
        cout << "fND{" << alg->currentIteration() << "} = [";
        for(int i=0; i < ndPop->size(); i++) {
            TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
            for(int var=0; var<vec.size(); var++) {
                cout << vec[var] << " ";
            }
            cout << "; ";
        }
        cout << "];" << endl;
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}
#endif

QTEST_GUILESS_MAIN(tst_moga)

#include "tst_moga.moc"
