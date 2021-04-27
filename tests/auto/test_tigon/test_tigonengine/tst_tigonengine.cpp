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
using namespace std;

#include <tigon/Tigon.h>
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

class tst_tigonengine : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_InitializeWorkflow();
    void test_WorkFlowNavigation();
    void test_CreateWorkFlow();
    void test_AppendWorkFlowsToEngine();
    void test_TigonOperatorFactory();
    void test_TigonFunctionFactory();
    void test_CreateWorkFlowUsingOpFactory();
    void test_LoadProblemFromXML();
    void test_LoadOperatorFromXML();
    void test_LoadAlgorithmFromXML();
    void test_CreateWorkFlowFromXML();
    void test_TerminationByBudget();
};

void tst_tigonengine::test_InitializeWorkflow()
{
    OptimizationLinearFlow* workflow = new OptimizationLinearFlow();
    QCOMPARE(workflow->size(), 0);
    QCOMPARE(workflow->currentIndex(), -1);
    bool res;
    res = workflow->nextNode() == 0;
    QCOMPARE(res, true);
    res = workflow->previousNode() == 0;
    QCOMPARE(res, true);
}

void tst_tigonengine::test_WorkFlowNavigation()
{
    PSetBase*              base       = new PSetBase();
    ProblemGenerator*      prob       = new ProblemGenerator(base);
    Evaluator*             evaluator  = new Evaluator(prob);
    NSGAIIPSA*             alg        = new NSGAIIPSA(evaluator);

    OptimizationLinearFlow* workflow = new OptimizationLinearFlow();
    workflow->appendNode(base);
    workflow->appendNode(prob);
    workflow->appendNode(evaluator);
    workflow->appendNode(alg);
    QCOMPARE(workflow->size(), 4);
    QCOMPARE(workflow->currentIndex(), 3);

    bool res;
    res = workflow->nextNode() == 0;
    QCOMPARE(res, true);
    QCOMPARE(workflow->currentIndex(), 3);
    QCOMPARE(workflow->previousNode(), evaluator);
    QCOMPARE(workflow->currentIndex(), 2);
    QCOMPARE(workflow->previousNode(), prob);
    QCOMPARE(workflow->currentIndex(), 1);
    QCOMPARE(workflow->previousNode(), base);
    QCOMPARE(workflow->currentIndex(), 0);
    res = workflow->previousNode() == 0;
    QCOMPARE(res, true);
    QCOMPARE(workflow->nextNode(), prob);
    QCOMPARE(workflow->nextNode(), evaluator);
    QCOMPARE(workflow->nextNode(), alg);
    res = workflow->nextNode() == 0;
    QCOMPARE(res, true);
    delete workflow;
}

void tst_tigonengine::test_CreateWorkFlow()
{
    int dVecSize = 5;
    int oVecSize = 2;
    int populationSize = 100;
    int maxIter = 5;

    // Workflow
    OptimizationLinearFlow* workflow = new OptimizationLinearFlow();

    // Base
    PSetBase*  base = new PSetBase();
    workflow->appendNode(base);

    // Problem formulation
    IFunctionSPtr func = IFunctionSPtr(
                TigonFunctionFactory::instance()->
                createFunction("Tigon::Representation::DTLZ1"));
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);

    IFormulation* form  = new IFormulation(workflow->currentNode());
    form->defineProblem(prob);
    workflow->appendNode(form);

    // Init
    RandomInit* init = new RandomInit(workflow->currentNode());
    init->TP_defineSetSize(populationSize);
    workflow->appendNode(init);

    Evaluator* evaluator = new Evaluator(init);
    workflow->appendNode(evaluator);

    // Algorithm
    NSGAIIPSA* alg  = new NSGAIIPSA(workflow->currentNode());
    workflow->appendNode(alg);

    // End node
    ITermination* termination = new ITermination(alg);
    workflow->appendNode(termination);
    termination->defineMaxIteration(maxIter);

    // Check
    QCOMPARE(workflow->node(0), base);
    QCOMPARE(workflow->node(1), form);
    QCOMPARE(workflow->node(2), init);
    QCOMPARE(workflow->node(3), evaluator);
    QCOMPARE(workflow->node(4), alg);
    QCOMPARE(workflow->node(5), termination);

    // Run workflow
    while(!workflow->isTerminate()) {
        workflow->evaluate();
        workflow->incrementIteration();
        TStringList tags;
        tags << Tigon::TFitness << Tigon::TForSelection;
        ISet* ndPop = alg->setWithTags(tags);
        cout << "fND{" << workflow->currentIteration() << "} = [";
        for(int i = 0; i < ndPop->size(); i++) {
            TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
            for(int var=0; var<vec.size(); var++) {
                cout << vec[var] << " ";
            }
            cout << "; ";
        }
        cout << "];" << endl;
    }
    QCOMPARE(workflow->finalNode()->decisionVecSize(), dVecSize);
    QCOMPARE(workflow->finalNode()->objectiveVecSize(), oVecSize);
    workflow->clearFlow();
}

void tst_tigonengine::test_AppendWorkFlowsToEngine()
{
    OptimizationLinearFlow* workflow1 = new OptimizationLinearFlow();
    OptimizationLinearFlow* workflow2 = new OptimizationLinearFlow();
    OptimizationLinearFlow* workflow3 = new OptimizationLinearFlow();

    TigonEngine* engine = TigonEngine::instance();
    engine->appendFlow(workflow1);
    engine->appendFlow(workflow2);
    engine->appendFlow(workflow3);

    QCOMPARE(engine->currentFlow(), workflow3);
    engine->setCurrentIndex(0);
    QCOMPARE(engine->currentFlow(), workflow1);
    engine->setCurrentIndex(1);
    QCOMPARE(engine->currentFlow(), workflow2);

    engine->clearFlow();

    bool res = engine->currentFlow() == 0;
    QCOMPARE(res, true);
}

void tst_tigonengine::test_TigonOperatorFactory()
{
    TString name("");
    PSetBase* base = new PSetBase;
    TigonEngine* engine = TigonEngine::instance();
    IPSet* op = engine->createOperator(name, base);
    bool res = (op == 0);
    QCOMPARE(res, true);

    name = TString("Tigon::Operators::ProblemGenerator");
    IPSet* pg = engine->createOperator(name, base);
    res = (name == pg->className());
    QCOMPARE(res, true);

    name = TString("Tigon::Operators::RandomInit");
    IPSet* randInit = engine->createOperator(name, pg);
    res = (name == randInit->className());
    QCOMPARE(res, true);

    name = TString("Tigon::Operators::Evaluator");
    IPSet* evaluator = engine->createOperator(name, base);
    res = (name == evaluator->className());
    QCOMPARE(res, true);

    name = TString("Tigon::Algorithms::NSGAIIPSA");
    IPSet* nsgapsa = engine->createOperator("Tigon::Algorithms::NSGAIIPSA", evaluator);
    res = (name == nsgapsa->className());
    QCOMPARE(res, true);

    name = TString("Tigon::Algorithms::ACROMUSE");
    IPSet* arcomuse = engine->createOperator("Tigon::Algorithms::ACROMUSE", evaluator);
    res = (name == arcomuse->className());
    QCOMPARE(res, true);


    delete op;
    delete pg;
    delete randInit;
    delete evaluator;
    delete nsgapsa;
    delete arcomuse;
}

void tst_tigonengine::test_TigonFunctionFactory()
{
    TigonEngine* engine = TigonEngine::instance();
    TString name;
    bool res;
    {
        name = TString("SomethingRandom_Haha");
        IFunction* func = engine->createFunction(name);
        res = (func == 0);
        QCOMPARE(res, true);
        delete func;
    }

    TStringList allInternalTestFunctions;
    allInternalTestFunctions << "DTLZ1" << "DTLZ2" << "CTP1" << "CTP2"
                             << "CTP3" << "CTP4" << "CTP5" << "CTP6";

    foreach(const TString& str, allInternalTestFunctions) {
        name = TString("Tigon::Representation::" + str);
        cout << "Test loading " << name << endl;
        IFunction* func = engine->createFunction(name);
        res = (name == func->className());
        QCOMPARE(res, true);
        delete func;
    }
}

void tst_tigonengine::test_CreateWorkFlowUsingOpFactory()
{
    int dVecSize = 5;
    int oVecSize = 2;
    int populationSize = 10;
    int maxIter = 2;

    // Workflow
    TigonEngine* engine = TigonEngine::instance();
    OptimizationLinearFlow* workflow = new OptimizationLinearFlow();

    // Base
    PSetBase* base = new PSetBase();
    workflow->appendNode(base);

    // Problem formulation
    IFunctionSPtr func = IFunctionSPtr(
                TigonFunctionFactory::instance()->
                createFunction("Tigon::Representation::DTLZ1"));
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);

    IFormulation* form  = new IFormulation(workflow->currentNode());
    form->defineProblem(prob);
    workflow->appendNode(form);

    // Init
    IPSet* init = engine->createOperator("Tigon::Operators::RandomInit",
                                         workflow->currentNode());
    init->setProperty("OptimizationSetSize", convertToString(populationSize));
    workflow->appendNode(init);

    Evaluator* evaluator = new Evaluator(init);
    workflow->appendNode(evaluator);

    // Algorithm
    IPSet* alg = engine->createOperator("Tigon::Algorithms::NSGAIIPSA",
                                        workflow->currentNode());
    workflow->appendNode(alg);

    IPSet* end = engine->createOperator("Tigon::Operators::ITermination",
                                        workflow->currentNode());
    workflow->appendNode(end);
    static_cast<Tigon::Operators::ITermination*>(end)->TP_defineMaxIteration(maxIter);

    // Check
    QCOMPARE(workflow->node(0), base);
    QCOMPARE(workflow->node(1), form);
    QCOMPARE(workflow->node(2), init);
    QCOMPARE(workflow->node(3), evaluator);
    QCOMPARE(workflow->node(4), alg);
    QCOMPARE(workflow->node(5), end);

    // Run workflow
    for(int iter=0; iter<workflow->maxIteration(); iter++) {
        workflow->evaluate();
        workflow->incrementIteration();
        TStringList tags;
        tags << Tigon::TFitness << Tigon::TForSelection;
        ISet* ndPop = alg->setWithTags(tags);
        cout << "fND{" << workflow->currentIteration() << "} = [";
        for(int i = 0; i < ndPop->size(); i++) {
            TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
            for(int var=0; var<vec.size(); var++) {
                cout << vec[var] << " ";
            }
            cout << "; ";
        }
        cout << "];" << endl;
    }
    QCOMPARE(workflow->finalNode()->decisionVecSize(), dVecSize);
    QCOMPARE(workflow->finalNode()->objectiveVecSize(), oVecSize);
}

void tst_tigonengine::test_LoadProblemFromXML()
{
    TigonEngine* engine = TigonEngine::instance();
    engine->clearFlow();
    bool success = engine->loadFlowFromFile("loadproblem.lgr");
    QCOMPARE(success, true);

    ProblemGenerator* pg = dynamic_cast<ProblemGenerator*>(engine->currentFlow()->currentNode());
    QCOMPARE(pg != nullptr, true);
    QCOMPARE(pg->functionVec().size() == 2, true);
}

void tst_tigonengine::test_LoadOperatorFromXML()
{
    TigonEngine* engine = TigonEngine::instance();
    engine->clearFlow();
    bool success = engine->loadFlowFromFile("loadingoperator.lgr");
    QCOMPARE(success, true);

    RandomInit* op = dynamic_cast<RandomInit*>(engine->currentFlow()->currentNode());
    QCOMPARE(op != nullptr, true);
    QCOMPARE(op->TP_setSize(), 12345);
    QCOMPARE(op->outputTags().size() == 2, true);
    QCOMPARE(op->outputTags()[1] == TString("FOR_EVALUATION"), true);
}

void tst_tigonengine::test_LoadAlgorithmFromXML()
{
    TigonEngine* engine = TigonEngine::instance();
    engine->clearFlow();
    bool success = engine->loadFlowFromFile("loadingalgorithm.lgr");
    QCOMPARE(success, true);

    IAlgorithm* alg = static_cast<IAlgorithm*>(engine->currentFlow()->finalNode());
    TVector<IOperator*> ops = alg->operators();
    PSAEliteSelection* op1 = static_cast<PSAEliteSelection*>(ops[2]);
    QCOMPARE(op1->TP_eliteRatio(), 0.12345);

    TournamentFiltrationForDirection* op2 = static_cast<TournamentFiltrationForDirection*>(ops[3]);
    QCOMPARE(op2->TP_mappingsPerSet(), 12345);
}

void tst_tigonengine::test_CreateWorkFlowFromXML()
{
    TigonEngine* engine = TigonEngine::instance();
    engine->clearFlow();
    bool success = engine->loadFlowFromFile("loadingworkflow.lgr");

    QCOMPARE(success, true);
    QCOMPARE(engine->currentFlow()->maxIteration(), 200);
    QCOMPARE(engine->currentFlow()->remainingIterations(), 200);
    QCOMPARE(engine->currentFlow()->remainingBudget(), 10000);
    QCOMPARE(engine->currentFlow()->finalNode()->decisionVecSize(), 5);
    QCOMPARE(engine->currentFlow()->finalNode()->objectiveVecSize(), 2);
    QCOMPARE(engine->currentFlow()->size(), 6);

    // Run
    int previousBudget = engine->currentFlow()->remainingBudget();
    while(!engine->currentFlow()->isTerminate()) {
        engine->currentFlow()->evaluate();
        engine->currentFlow()->incrementIteration();
        QCOMPARE(engine->currentFlow()->remainingBudget() < previousBudget, true);
        previousBudget = engine->currentFlow()->remainingBudget();
    }

    ISet* ndPop = engine->currentFlow()->finalNode()->set(0);
    for(int i = 0; i < ndPop->size(); i++) {
        TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
        QCOMPARE(std::accumulate(vec.begin(), vec.end(),0.0) < 1.5, true);
        for(int var=0; var<vec.size(); var++) {
            cout << vec[var] << " ";
        }
        cout << "; ";
    }
    cout << "];" << endl;
}

void tst_tigonengine::test_TerminationByBudget()
{
    int dVecSize = 5;
    int oVecSize = 2;
    int populationSize = 50;
    int budget = 10000;

    // Workflow
    TigonEngine* engine = TigonEngine::instance();
    OptimizationLinearFlow* workflow = new OptimizationLinearFlow();

    // Base
    PSetBase* base = new PSetBase();
    workflow->appendNode(base);

    // Problem formulation
    IFunctionSPtr func = IFunctionSPtr(
                TigonFunctionFactory::instance()->
                createFunction("Tigon::Representation::DTLZ2"));
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);

    IFormulation* form  = new IFormulation(workflow->currentNode());
    form->defineProblem(prob);
    workflow->appendNode(form);

    // Init
    IPSet* init = engine->createOperator("Tigon::Operators::RandomInit",
                                         workflow->currentNode());
    init->setProperty("OptimizationSetSize", convertToString(populationSize));
    workflow->appendNode(init);

    IPSet* evaluator = engine->createOperator("Tigon::Operators::Evaluator",
                                                  workflow->currentNode());
    workflow->appendNode(evaluator);

    // Algorithm
    IPSet* alg = engine->createOperator("Tigon::Algorithms::NSGAIIPSA",
                                        workflow->currentNode());
    workflow->appendNode(alg);

    // End
    IPSet* end = engine->createOperator("Tigon::Operators::ITermination",
                                        workflow->currentNode());
    workflow->appendNode(end);
    end->setProperty("budget", convertToString(budget));

    // Append flow
    engine->appendFlow(workflow);
    QCOMPARE(engine->currentFlow()->remainingBudget(), budget);

    // Run
    while(!engine->currentFlow()->isTerminate()) {
        engine->currentFlow()->evaluate();
        engine->currentFlow()->incrementIteration();
        if(engine->currentFlow()->remainingBudget() % 50 == 0) {
            cout << "Iteration: "<< engine->currentFlow()->currentIteration()
                 << " | Remaining buget:  "
                 << engine->currentFlow()->remainingBudget() << " / "
                 << engine->currentFlow()->budget() << endl;
        }
    }
    cout << "====== Terminated ======" << endl;
    cout << "Iteration: "<< engine->currentFlow()->currentIteration()
         << " | Remaining buget:  "
         << engine->currentFlow()->remainingBudget() << " / "
         << engine->currentFlow()->budget() << endl;
    ISet* ndPop = engine->currentFlow()->finalNode()->set(0);
    for(int i = 0; i < ndPop->size(); i++) {
        TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
        for(int var=0; var<vec.size(); var++) {
            cout << vec[var] << " ";
        }
        cout << "; ";
    }
    cout << "];" << endl;
}

QTEST_GUILESS_MAIN(tst_tigonengine)

#include "tst_tigonengine.moc"
