/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <QtTest>
#include <QDebug>

#include <iostream>
#include <tigon/Tigon.h>

#define TST_NSGAIII_OPERATORS
#define WORK_FLOW
#define ALGORITHM

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_nsgaiii : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_NSGAIII_OPERATORS
    /* Add the necessary operators */
    void test_NSGAIIINiching();
#endif

#ifdef WORK_FLOW
    void test_NSGAIII_workflow();
#endif
#ifdef ALGORITHM
    void test_NSGAIII();
#endif
};

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

#ifdef TST_NSGAIII_OPERATORS

void tst_nsgaiii::test_NSGAIIINiching()
{
    TRAND.defineSeed(2);
    cout.precision(5);

    int dVecSize = 6;
    int oVecSize = 3;
    int popSize = 210;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init     = new RandomInit(prob);
    init->TP_defineSetSize(popSize);

    Evaluator* evaluator = new Evaluator(init);

    NonDominanceRanking* ranking = new NonDominanceRanking(evaluator);
    NSGAIIINiching*      niching = new NSGAIIINiching(ranking);

    double eliteRatio = 0.5;
    niching->TP_defineEliteRatio(eliteRatio);

    niching->evaluate();

    int eliteSize = popSize*eliteRatio;

    TVector<ISet*> allRanks = ranking->outputSets();

    int popSizeCounter = 0;
    ISet* Fl = new ISet();

    for(ISet* rank : allRanks) {
        popSizeCounter += rank->size();
        if(popSizeCounter >= eliteSize) {
            Fl->append(rank);
        }
    }

    bool areThereSolutionsWithDifferentCosts = false;
    if(Fl->size() != eliteSize) {

        double costComp = Fl->at(0)->doubleCost();
        for(auto sol : Fl->all()) {
            if(!areDoublesEqual(costComp, sol->doubleCost())) {
                areThereSolutionsWithDifferentCosts = true;
            }
        }
        QCOMPARE(areThereSolutionsWithDifferentCosts, true);
    }

    delete Fl;
    delete niching;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

#endif

#ifdef WORK_FLOW
void tst_nsgaiii::test_NSGAIII_workflow()
{
    TRAND.defineSeed(2);
    cout.precision(5);

    int dVecSize = 6;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 2;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init     = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    /* Algorithm */
    NonDominanceRanking* ranking = new NonDominanceRanking(evaluator);
    NSGAIIINiching*     niching = new NSGAIIINiching(ranking);
    NSGAIIEliteSelection*  elite = new NSGAIIEliteSelection(niching);
    TournamentFiltrationForDirection* children =
            new TournamentFiltrationForDirection(elite);
    SBXCrossOver*      crossOver = new SBXCrossOver(children);
    PolynomialMutation*       pm = new PolynomialMutation(crossOver);
    MergeForNextIteration*   nxt = new MergeForNextIteration(pm);

    elite->addOutputTag(Tigon::TForSelection);
    elite->addOutputTag(Tigon::TForModification);
    children->TP_defineMappingsPerSet(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addInputTag(Tigon::TForModification);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForPerturbation);

    //Display the results:
    for(int iter=0; iter<maxIter; iter++) {
        nxt->evaluate();
        nxt->incrementIteration();

        cout << "Iteration " << nxt->currentIteration() << endl;
    }

    TStringList eliteTags;
    eliteTags << Tigon::TForNextIteration << Tigon::TForSelection;
    ISet* ndPop = nxt->setWithTags(eliteTags);

    std::cout << "Objectives:" << std::endl;
    std::cout << "f = [";
    for(auto sol : ndPop->all()) {
        auto f = sol->doubleObjectiveVec();

        std::cout << "[";
        for(auto elem : f) {
            std::cout << elem << " ";
        }
        std::cout << "]; " << std::endl;
    }
    std::cout << "]; " << std::endl;


    std::cout << "Decision variables:" << std::endl;
    std::cout << "x = [";
    for(auto sol : ndPop->all()) {
        auto x = sol->doubleDecisionVec();

        std::cout << "[";
        for(auto elem : x) {
            std::cout << elem << " ";
        }
        std::cout << "]; " << std::endl;
    }
    std::cout << "]; " << std::endl;

    delete nxt;
    delete pm;
    delete crossOver;
    delete children;
    delete elite;
    delete niching;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}
#endif

#ifdef ALGORITHM
void tst_nsgaiii::test_NSGAIII()
{
    TRAND.defineSeed(2);
    cout.precision(5);

    int iter;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 5;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);

    NSGAIII* alg = new NSGAIII(evaluator);

    TStringList mainOptimizationTag;
    mainOptimizationTag << Tigon::TMainOptimizationSet;
    TStringList eliteTags;
    eliteTags << Tigon::TForNextIteration << Tigon::TForSelection;


    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        // Mixed population
        ISet* mixedPop = alg->setWithTags(mainOptimizationTag);
        QCOMPARE(mixedPop->size(),2*populationSize);

        // Elite population
        ISet* ndPop = alg->setWithTags(eliteTags);
        QCOMPARE(ndPop->size(),populationSize);

        // Print Elite population
        cout << "fND{" << alg->currentIteration() << "} = [";
        for(int i=0; i < ndPop->size(); i++) {
            TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
            for(auto var : vec) {
                cout << var << " ";
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

QTEST_GUILESS_MAIN(tst_nsgaiii)

#include "tst_nsgaiii.moc"
