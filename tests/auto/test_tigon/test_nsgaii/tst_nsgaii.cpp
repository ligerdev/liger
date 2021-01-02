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
#include <QtTest>
#include <QDebug>

#include <iostream>
#include <tigon/Tigon.h>

#define TST_NSGAII_OPERATORS
#define WORK_FLOW
#define ALGORITHM

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_nsgaii : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_NSGAII_OPERATORS
    /* Add the necessary operators */
    void test_NonDominanceRanking();
    void test_NonDominanceRanking_ConstraintHandling();
    void test_NSGAIICrowding();
    void test_NSGAIIEliteSelection();
    void test_TournamentForDirection();
#endif

#ifdef WORK_FLOW
    void test_NSGAII_workflow();
#endif
#ifdef ALGORITHM
    void test_NSGAII();
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

#ifdef TST_NSGAII_OPERATORS

void tst_nsgaii::test_NonDominanceRanking()
{
    int popSize = 10;
    int nVar = 10;
    int nObj = 3;

    PSetBase*               base = new PSetBase();
    IFormulation*           prob = new IFormulation(base);
    RandomInit*             init = new RandomInit(prob);
    Evaluator*         evaluator = new Evaluator(init);
    NonDominanceRanking* ranking = new NonDominanceRanking(evaluator);

    IFunctionSPtr         func  = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(nVar);
    func->TP_defineNOutputs(nObj);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);

    ranking->TP_defineIsConstrainedHandlingUsed(false);

    ranking->evaluate();

    TVector<ISet*> ranks = ranking->outputSets();

    for(size_t i=0; i<ranks.size()-1; i++) {
        ISet* betterRank = ranks[i];
        // select a solution from upper rank
        for(auto betterSol: betterRank->all()) {
            for(size_t j=i+1; j<ranks.size(); j++) {
                ISet* worseRank = ranks[j];
                // select a solution from lower rank
                for(auto worseSol: worseRank->all()) {

                    // a worse rank solution does not dominate a better rank
                    // solution. Either the better rank dominates the worse rank
                    // solution, or neither solutions dominate the other.
                    tribool rs = *worseSol <= *betterSol;
                    QCOMPARE(bool(rs.value != true), true);
                    // note that rs.value can be either false or incomparable
                }
            }
        }
    }

    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_nsgaii::test_NonDominanceRanking_ConstraintHandling()
{
    int popSize = 100;

    PSetBase*               base = new PSetBase();
    IFormulation*           form = new IFormulation(base);
    RandomInit*             init = new RandomInit(form);
    Evaluator*         evaluator = new Evaluator(init);
    NonDominanceRanking* ranking = new NonDominanceRanking(evaluator);

    /// Define the problem
    IFunctionSPtr func(new CTP1);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2 << 3;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(2, 0.0);
    TVector<ElementType> types(2, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    init->TP_defineSetSize(popSize);
    form->defineProblem(prob);

    ranking->TP_defineIsConstrainedHandlingUsed(true);

    ranking->evaluate();

    ISet infeasible;

    TVector<ISet*> ranks = ranking->outputSets();
    for(int i=0; i<ranks.size(); i++) {
        ISet* front = ranks[i];

        for(auto sol: front->all()) {
            if(!sol->isFeasible()) { // solution is infeasible
                // the cost set for infeasible solutions is equal to its rank
                QCOMPARE(sol->cost()->value<int>(),i);

                // collect all infeasible solutions into a single set
                infeasible.append(sol);
            }
        }
    }

    for(int i=0; i<infeasible.size()-1; i++) {

        IMappingSPtr a = infeasible.at(i);
        double constrViolationA = 0.0;
        TVector<double> constrA = a->doubleConstraintVec();
        for (size_t c=0; c<constrA.size(); c++) {
            if(constrA[c]>threshVec[c]) {
                constrViolationA += constrA[c]-threshVec[c];
            }
        }

        for(int j=i+1; j<infeasible.size(); j++) {

            IMappingSPtr b = infeasible.at(j);
            double constrViolationB = 0.0;
            TVector<double> constrB = b->doubleConstraintVec();
            for (size_t c=0; c<constrB.size(); c++) {
                if(constrB[c]>threshVec[c]) {
                    constrViolationB += constrB[c]-threshVec[c];
                }
            }

            // if the rank of solution a is lower than b,
            // then a is less infeasible than b
            if( a->doubleCost() < b->doubleCost() ) {
                QCOMPARE(constrViolationA<constrViolationB,true);
            }
            else {
                // if the rank of solution a is higher than b,
                // then a is more infeasible than b
                if (a->doubleCost() > b->doubleCost() ) {
                    QCOMPARE(constrViolationA>constrViolationB,true);
                }
                else {
                    // infeasible solutions with the same rank have exactly
                    // the same level of infeasibility
                    QCOMPARE(areDoublesEqual(constrViolationA,constrViolationB),
                             true);
                }
            }
        }
    }
}

void tst_nsgaii::test_NSGAIICrowding()
{
    int dVecSize = 6;
    int oVecSize = 2;
    int populationSize = 80;

    cout.precision(5);

    PSetBase*               base = new PSetBase();
    IFormulation*           prob = new IFormulation(base);
    RandomInit*             init = new RandomInit(prob);
    Evaluator*         evaluator = new Evaluator(init);
    NonDominanceRanking* ranking = new NonDominanceRanking(evaluator);
    NSGAIICrowding*     crowding = new NSGAIICrowding(ranking);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(populationSize);

    crowding->evaluate();

    TVector<ISet*> ranks = ranking->outputSets();
    for(size_t i=0; i<ranks.size(); i++) {
        ISet* front = ranks[i];

        for(auto sol: front->all()) {
            double cost = sol->doubleCost();
            int rank = (int)i;
            QCOMPARE((cost<(rank+1.0)) && (cost>=rank), true);
        }
    }

    //Display the results:
    cout << "Ranks of non-dominance and cost based on rank and crowding "<< endl;
    cout << "obj-1\tobj-2\tcrowding" << endl;
    for(size_t i=0; i<ranks.size(); i++) {
        cout << "Rank " << i << endl;
        ISet* front = ranks[i];
        for(auto sol: front->all()) {
            TVector<double> obj = sol->doubleObjectiveVec();
            double cost = sol->doubleCost();
            for(auto o: obj) {
                cout << o << "\t";
            }
            cout << cost << endl;
        }
        cout << endl;
    }

    delete crowding;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_nsgaii::test_NSGAIIEliteSelection()
{
    int i,j;
    int dVecSize = 7;
    int oVecSize = 2;
    int populationSize = 20;

    cout.precision(5);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator*       evaluator      = new Evaluator(init);
    NonDominanceRanking*  ranking   = new NonDominanceRanking(evaluator);
    NSGAIICrowding* crowding        = new NSGAIICrowding(ranking);
    NSGAIIEliteSelection* elite     = new NSGAIIEliteSelection(crowding);

    elite->evaluate();

    //Display the results:
    TVector<ISet*> ranks = ranking->outputSets();
    cout << "Ranks of non-dominance and cost based on rank and crowding "
            "(objective values)-[cost]:" << endl;
    for(i=0; i<ranks.size(); i++) {
        cout << "Rank " << i << ":\t";
        ISet* front = ranks.at(i);
        for(j=0; j<front->size(); j++) {
            TVector<IElementSPtr> oVec = front->at(j)->objectiveVec();
            double cost = front->at(j)->doubleCost();
            cout << "(" << oVec.at(0)->value<double>() <<
                    "," << oVec.at(1)->value<double>() << ")-[" << cost << "]\t ";
        }
        cout << endl;
    }

    for(i=0; i<ranks.size(); i++) {
        cout << "Rank " << i << endl;
        ISet* front = ranks.at(i);
        for(j=0; j<front->size(); j++) {
            TVector<IElementSPtr> oVec = front->at(j)->objectiveVec();
            cout << oVec.at(0)->value<double>() <<
                    "\t" << oVec.at(1)->value<double>() << endl;
        }
        cout << endl;
    }

    ISet* elitePop = elite->outputSet(0);
    cout << "Elite solutions (objective values)-[cost]:" << endl;
    for(int i=0; i<elitePop->size(); i++) {
        TVector<IElementSPtr> oVec = elitePop->at(i)->objectiveVec();
        double cost = elitePop->at(i)->doubleCost();
        cout << "(" << oVec.at(0)->value<double>() <<
                "," << oVec.at(1)->value<double>() << ")-[" << cost << "]\t ";
    }
    cout << endl;

    delete elite;
    delete crowding;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_nsgaii::test_TournamentForDirection() {

    int dVecSize = 7;
    int oVecSize = 2;
    int populationSize = 20;

    cout.precision(5);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    Evaluator*       evaluator      = new Evaluator(init);
    NonDominanceRanking*  ranking   = new NonDominanceRanking(evaluator);
    NSGAIICrowding* crowding        = new NSGAIICrowding(ranking);
    NSGAIIEliteSelection* elite     = new NSGAIIEliteSelection(crowding);
    TournamentFiltrationForDirection* children =
            new TournamentFiltrationForDirection(elite);
    children->TP_defineMappingsPerSet(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);

    children->evaluate();

    int nISet = children->outputSets().size();
    QCOMPARE(nISet,populationSize/2);
    for(int i=0;i<nISet;i++) {
        ISet* childSet = children->outputSet(i);
        QCOMPARE(childSet->size(),2);

        TVector<double> child1 = childSet->at(0)->doubleObjectiveVec();
        TVector<double> child2 = childSet->at(1)->doubleObjectiveVec();

        cout << "Set " << i+1 << endl;
        cout << child1[0] << " " << child1[1] << endl;
        cout << child2[0] << " " << child2[1] << endl;
        cout << endl;
    }

    delete children;
    delete elite;
    delete crowding;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}
#endif

#ifdef WORK_FLOW
void tst_nsgaii::test_NSGAII_workflow()
{
    int dVecSize = 6;
    int oVecSize = 2;
    int populationSize = 80;
    int maxIter = 2;

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

    /* Algorithm */
    NonDominanceRanking* ranking = new NonDominanceRanking(evaluator);
    NSGAIICrowding* crowding = new NSGAIICrowding(ranking);

    NSGAIIEliteSelection* elite = new NSGAIIEliteSelection(crowding);
    elite->addOutputTag(Tigon::TForSelection);

    TournamentFiltrationForDirection* children =
            new TournamentFiltrationForDirection(elite);
    children->TP_defineMappingsPerSet(2);
    children->TP_defineSelectionMethod(Tigon::ShuffledSelection);
    children->addOutputTag(Tigon::TForNextIteration);
    children->addOutputTag(Tigon::TForPerturbation);

    SBXCrossOver* crossOver = new SBXCrossOver(children);
    PolynomialMutation* pm = new PolynomialMutation(crossOver);
    MergeForNextIteration* nxt = new MergeForNextIteration(pm);

    //Display the results:
    for(int iter=0; iter<maxIter; iter++) {
        nxt->evaluate();
        nxt->incrementIteration();

        cout << "Generation " << nxt->currentIteration() << endl;

        for(int i=0;i<ranking->outputSets().size();i++) {
            ISet* Pop = ranking->outputSet(i);
            cout << "Rank{" << i+1 << "} = [" << endl;
            for(int j=0;j<Pop->size();j++) {
                TVector<double> obj = Pop->at(j)->doubleObjectiveVec();
                for(int var=0; var<obj.size(); var++) {
                    cout << obj[var] << " ";
                }
                cout << endl;
            }
            cout << "];" << endl;
        }
        cout << endl;
    }

    delete nxt;
    delete pm;
    delete crossOver;
    delete children;
    delete elite;
    delete crowding;
    delete ranking;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}
#endif

#ifdef ALGORITHM
void tst_nsgaii::test_NSGAII()
{
    int iter;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 5;

    cout.precision(5);
    srand(465654);

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);

    NSGAII* alg = new NSGAII(evaluator);

    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        // Mixed population
        TStringList tags2;
        tags2 << Tigon::TMainOptimizationSet;
        ISet* mixedPop = alg->setWithTags(tags2);
        QCOMPARE(mixedPop->size(),2*populationSize);

        // Elite population
        TStringList tags1;
        tags1 << Tigon::TForNextIteration << Tigon::TForSelection;
        ISet* ndPop = alg->setWithTags(tags1);
        QCOMPARE(ndPop->size(),populationSize);

        // Print Elite population
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

QTEST_GUILESS_MAIN(tst_nsgaii)

#include "tst_nsgaii.moc"
