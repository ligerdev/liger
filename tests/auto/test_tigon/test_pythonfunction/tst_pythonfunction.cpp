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

#include <tigon/Tigon.h>
#include <iostream>
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace std;

class tst_pythonfunction : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef PYTHON_API
    void initTestCase();
    void constructors();
    void definePath();
    void evaluate();
    void mulitEvaluate();
    void batchEvaluate();
    void importExternalModule();
    void workflowInternal();
    void loadPythonFuncFromProblemGenerator();
    void loadPythonFunctionFromWorkflowFile();
#endif
};

#ifdef PYTHON_API
TString TestBinPath;

void tst_pythonfunction::initTestCase()
{
    TestBinPath = QCoreApplication::applicationDirPath().toStdString();
}

void tst_pythonfunction::constructors()
{
    PythonFunction pyfunc;

    QCOMPARE(pyfunc.pythonFunctionName().empty(), true);
    QCOMPARE(pyfunc.pythonFunctionDirectory().empty(), true);

    PythonFunction* pyfuncPtr = new PythonFunction();

    QCOMPARE(pyfuncPtr->pythonFunctionName().empty(), true);
    QCOMPARE(pyfuncPtr->pythonFunctionDirectory().empty(), true);

    delete pyfuncPtr;
}

void tst_pythonfunction::definePath()
{
    TString path(TestBinPath + "/pythonfunc.py");
    PythonFunction pyfunc;
    pyfunc.definePath(path);

    QCOMPARE(pyfunc.TP_nInputs(), 5);
    QCOMPARE(pyfunc.TP_nOutputs(), 2);
    for (int i=0; i<pyfunc.TP_nInputs(); ++i){
        double lb = i*1.0;
        QCOMPARE(pyfunc.boxConstraints()->lowerBound(i).value<double>(), lb);
        QCOMPARE(pyfunc.boxConstraints()->upperBound(i).value<double>(), 1.0+lb);
    }

    QCOMPARE(pyfunc.pythonFunctionName(), TString("pythonfunc"));
    QCOMPARE(pyfunc.pythonFunctionDirectory(), TestBinPath);
}

void tst_pythonfunction::evaluate()
{
    TString path(TestBinPath + "/pythonfunc.py");
    PythonFunction pyfunc;
    pyfunc.definePath(path);

    TVector<IElementSPtr> input(5);
    TVector<IElementSPtr> output(2);
    for (size_t i=0; i<5; ++i) {
        input[i] = IElementSPtr(new IElement(static_cast<double>(i+1)));
    }

    for (size_t i=0; i<2; ++i) {
        output[i] = IElementSPtr(new IElement);
    }

    pyfunc.evaluate(input, output);

    QCOMPARE(output[0]->value<double>(), 15.0);
    QCOMPARE(output[1]->value<double>(), 120.0);
}

void tst_pythonfunction::mulitEvaluate()
{
    TString path(TestBinPath + "/pythonfunc.py");
    PythonFunction pyfunc;
    pyfunc.definePath(path);

    // 5 samples
    size_t M = 5;

    for (size_t i = 0; i<M; ++i) {
        TVector<IElementSPtr> input(5);
        TVector<IElementSPtr> output(2);

        double sum = 0.0;
        double product = 1.0;
        for (size_t j=0; j<5; ++j) {
            input[j] = IElementSPtr(new IElement(static_cast<double>(i*j+1)));
            sum += input[j]->value<double>();
            product *= input[j]->value<double>();
        }

        for (size_t j=0; j<2; ++j) {
            output[j] = IElementSPtr(new IElement);
        }

        pyfunc.evaluate(input, output);

        QCOMPARE(output[0]->value<double>(), sum);
        QCOMPARE(output[1]->value<double>(), product);
    }
}

void tst_pythonfunction::batchEvaluate()
{
    TString path(TestBinPath + "/pythonfunc_batch.py");
    PythonFunction pyfunc;
    pyfunc.definePath(path);

    // 5 samples
    size_t M = 5;

    TVector<TVector<IElementSPtr>> inputs(M);
    TVector<TVector<IElementSPtr>> outputs(M);

    TVector<double> sums(M);
    TVector<double> products(M);

    for (size_t i = 0; i<M; ++i) {
        TVector<IElementSPtr> input(5);
        TVector<IElementSPtr> output(2);

        double sum = 0.0;
        double product = 1.0;
        for (size_t j=0; j<5; ++j) {
            input[j] = IElementSPtr(new IElement(static_cast<double>(i*j+1)));
            sum += input[j]->value<double>();
            product *= input[j]->value<double>();
        }

        for (size_t j=0; j<2; ++j) {
            output[j] = IElementSPtr(new IElement);
        }

        inputs[i] = input;
        outputs[i] = output;
        sums[i] = sum;
        products[i] = product;
    }

    pyfunc.batchEvaluate(inputs, outputs);

    for (size_t i = 0; i<M; ++i) {
        TVector<IElementSPtr> output = outputs[i];

        QCOMPARE(output[0]->value<double>(), sums[i]);
        QCOMPARE(output[1]->value<double>(), products[i]);
    }
}

void tst_pythonfunction::importExternalModule()
{
    TString path(TestBinPath + "/pythonImport.py");
    PythonFunction pyfunc;
    pyfunc.definePath(path);

    TVector<IElementSPtr> input(5);
    TVector<IElementSPtr> output(2);
    for (size_t i=0; i<5; ++i) {
        input[i] = IElementSPtr(new IElement(static_cast<double>(i+1)));
    }

    for (size_t i=0; i<2; ++i) {
        output[i] = IElementSPtr(new IElement);
    }

    pyfunc.evaluate(input, output);

    QCOMPARE(output[0]->value<double>(), 15.0);
    QCOMPARE(output[1]->value<double>(), 120.0);
}

void tst_pythonfunction::workflowInternal()
{
    using namespace Tigon::Algorithms;

    OptimizationLinearFlow flow;
    int populationSize = 20;
    int maxIter = 20;

    // Base
    PSetBase* base = new PSetBase;
    flow.appendNode(base);

    // Problem
    IFunctionSPtr func = IFunctionSPtr(new PythonFunction);
    TString path(TestBinPath + "/pySchaffer.py");
    func->definePath(path);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);

    IFormulation* form  = new IFormulation(base);
    form->defineProblem(prob);
    flow.appendNode(form);

    // Init
    RandomInit* init = new RandomInit(form);
    init->TP_defineSetSize(populationSize);
    flow.appendNode(init);

    Evaluator* evaluator = new Evaluator(init);
    flow.appendNode(evaluator);

    // Algorithm
    NSGAIIPSA* alg  = new NSGAIIPSA(evaluator);
    flow.appendNode(alg);

    // Termination
    ITermination* end = new ITermination(alg);
    flow.appendNode(end);
    end->TP_defineMaxIteration(maxIter);

    // Run flow
    for(int iter=0; iter<flow.maxIteration(); ++iter) {
        flow.evaluate();
        flow.incrementIteration();
        cout << iter << endl;
    }

    ISet* ndPop = alg->set(0);
    for(int i = 0; i < ndPop->size(); i++) {
        TVector<double> vec = ndPop->at(i)->doubleObjectiveVec();
        cout << vec << endl;
    }
}

void tst_pythonfunction::loadPythonFuncFromProblemGenerator()
{
    ProblemGenerator pg;
    TString className("Tigon::Representation::PythonFunction");
    TString funcPath = TestBinPath + "/" + "pySchaffer.py";
    IFunctionSPtr pyfunc = pg.createFunctionByName(className, funcPath);
    QCOMPARE(bool(pyfunc), true);
    QCOMPARE(pyfunc->inputPrpts().size() == 2, true);
    QCOMPARE(pyfunc->outputPrpts().size() == 2, true);
    QCOMPARE(pyfunc->functionProperties().name(), TString("Schaffer"));

    for(size_t i=0; i<pyfunc->boxConstraints()->lowerBounds().size(); i++) {
        QCOMPARE(pyfunc->boxConstraints()->lowerBound(i).value(), -10.0);
    }

    for(size_t i=0; i<pyfunc->boxConstraints()->upperBounds().size(); i++) {
        QCOMPARE(pyfunc->boxConstraints()->upperBound(i).value(), 10.0);
    }

    TVector<IElementSPtr> input(2);
    TVector<IElementSPtr> output(2);
    for (size_t i=0; i<2; ++i) {
        input[i] = IElementSPtr(new IElement(static_cast<double>(i+1)));
    }

    for (size_t i=0; i<2; ++i) {
        output[i] = IElementSPtr(new IElement);
    }

    pyfunc->evaluate(input, output);

    QCOMPARE(output[0]->value<double>(), 1.0);
    QCOMPARE(output[1]->value<double>(), 1.0);
}

void tst_pythonfunction::loadPythonFunctionFromWorkflowFile()
{
    TigonEngine* eng = TigonEngine::instance();
    TString workflowFile =  TestBinPath+ "/pythonfunction_workflow.lgr";

    QFile file(QString::fromStdString(workflowFile));
    if(file.open(QIODevice::ReadWrite)) {
        QTextStream tstream(&file);
        QString content = tstream.readAll();
        if(content.indexOf("####REPLACE_ME####") > -1) {
            content.replace("####REPLACE_ME####", QString::fromStdString(TestBinPath));
            file.resize(0);
            tstream << content;
        }
        file.close();
    } else {
        QFAIL("Workflow cannot open");
    }

    eng->loadFlowFromFile(workflowFile);
    for(int i=0; i<eng->currentFlow()->size(); i++) {
        IPSet* op = eng->currentFlow()->node(i);
        cout << "* Operator: "<< op->className() << endl;
        if(op->className() == TString("Tigon::Operators::RandomInit")) {
            RandomInit* init = dynamic_cast<RandomInit*>(op);
            if(init) {
                cout <<"OptimizationSetSize: " << init->TP_setSize() << endl;
                QCOMPARE(init->TP_setSize(), 10);
            }
            continue;
        }

        if(op->className() == TString("Tigon::Operators::ProblemGenerator")) {
            ProblemGenerator* pg = dynamic_cast<ProblemGenerator*>(op);
            if(pg) {
                cout << "Function Vector Size: " << pg->functionVec().size() << endl;
                cout << "Function[0]: " << pg->functionVec()[0]->name() << endl;
                QCOMPARE(pg->functionVec()[0]->name(), TString("Test Function"));
            }
            continue;
        }
    }

    cout << "* maxIter: " << eng->currentFlow()->maxIteration() << endl;

    while(!eng->currentFlow()->isTerminate()) {
        eng->currentFlow()->evaluate();
        eng->currentFlow()->incrementIteration();
        cout << "Iter " << eng->currentFlow()->currentIteration() << endl;;
    }

    QCOMPARE(eng->currentFlow()->currentIteration(),
             eng->currentFlow()->maxIteration());

    ISet* pop = eng->currentFlow()->finalNode()->allSets().at(0);
    cout << "sol = " << pop;
}

#endif

QTEST_GUILESS_MAIN(tst_pythonfunction)

#include "tst_pythonfunction.moc"
