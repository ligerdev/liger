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
#include <QCoreApplication>
#include <QDir>
#include <iostream>
#include <tigon/Tigon.h>

DISABLE_WARNINGS
#include <eigen/bench/BenchTimer.h>
ENABLE_WARNINGS

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

void runFastBatchEvaluator(int maxIter, int popSize)
{
    cout << "Using Fast Matlab Evaluator" << endl;
    TRAND.defineSeed(1991);

    const TString TestBinPath = QCoreApplication::applicationDirPath().toStdString();

    IFunction* pfunc = TigonFunctionFactory::instance()->createFunction(
                "Tigon::Representation::MatlabFunction");
    pfunc->definePath(TestBinPath + "/DTLZ1.m");
    IFunctionSPtr func = IFunctionSPtr(pfunc);

    OptimizationLinearFlow flow;
    PSetBase* base = new PSetBase();
    flow.appendNode(base);

    IFormulation* prob = new IFormulation(flow.currentNode());
    prob->appendFunction(func);
    flow.appendNode(prob);

    RandomInit* init = new RandomInit(flow.currentNode());
    init->TP_defineSetSize(popSize);
    flow.appendNode(init);

    BatchEvaluator* eval = new BatchEvaluator(flow.currentNode());
    flow.appendNode(eval);

    NSGAII* alg = new NSGAII(flow.currentNode());
    flow.appendNode(alg);

    flow.evaluate();

    Eigen::BenchTimer timer;
    // warm up
    cout << "Start timer" << endl;
    timer.start();
    for(int i=1; i<maxIter; i++) {
        flow.evaluate();
        if(i % 10 == 0) {
            cout << "Iter [" << i << "]" << endl;
        }
    }
    timer.stop();

    cout << "Time: " << timer.value() << " seconds" << endl;
    //MatlabPool::getInstance().emptyPool();

    ISet* pop = flow.finalNode()->allSets().at(0);
    cout << endl;
    cout << "sol = " << pop;
    cout << endl << endl;
}

void runGenericBatchEvaluator(int maxIter, int popSize)
{
    cout << "Using Generic Evaluator" << endl;
    TRAND.defineSeed(1991);

    const TString TestBinPath = QCoreApplication::applicationDirPath().toStdString();

    IFunction* pfunc = TigonFunctionFactory::instance()->createFunction(
                "Tigon::Representation::MatlabFunction");
    pfunc->definePath(TestBinPath + "/DTLZ1.m");
    IFunctionSPtr func = IFunctionSPtr(pfunc);

    OptimizationLinearFlow flow;
    PSetBase* base = new PSetBase();
    flow.appendNode(base);

    IFormulation* prob = new IFormulation(flow.currentNode());
    prob->appendFunction(func);
    flow.appendNode(prob);

    RandomInit* init = new RandomInit(flow.currentNode());
    init->TP_defineSetSize(popSize);
    flow.appendNode(init);

    Evaluator* eval = new Evaluator(flow.currentNode());
    flow.appendNode(eval);

    NSGAII* alg = new NSGAII(flow.currentNode());
    flow.appendNode(alg);

    flow.evaluate();

    Eigen::BenchTimer timer;
    cout << "Start timer" << endl;
    timer.start();
    for(int i=1; i<maxIter; i++) {
        flow.evaluate();
        if(i % 10 == 0) {
            cout << "Iter [" << i << "]" << endl;
        }
    }
    timer.stop();

    cout << "Time: " << timer.value() << " seconds" << endl;
    //MatlabPool::getInstance().emptyPool();

    ISet* pop = flow.finalNode()->allSets().at(0);
    cout << endl;
    cout << "sol = " << pop;
    cout << endl << endl;
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    QDir cpath = QCoreApplication::applicationDirPath();
    cpath.cdUp();
    cpath.cdUp();
#ifdef __APPLE__
    cpath.cd("bin/Liger.app/Contents/TigonPlugIns");
#else
    cpath.cd("lib/tigon/plugins");
#endif

    TString pluginPath = cpath.path().toStdString();

    cout << pluginPath  << endl;
    PluginManager::instance()->definePath(pluginPath);
    PluginManager::instance()->loadPlugins();

    TMap<TString, TString> errors = PluginManager::instance()->errorStates();
    TStringList keys = getMapKeys(errors);
    for(const TString &k : keys) {
        cout << k << ": " << errors[k] << endl;
    }

    cout << "Running Benchmarks..." << endl;

    const int maxIter = 101;
    const int popSize = 100;

    cout << "Max iters    :  " << maxIter << endl;
    cout << "Timed iters  :  " << maxIter - 1  << endl;
    cout << "Init Pop size:  " << popSize << endl;

    cout << "==================" << endl;
    runGenericBatchEvaluator(maxIter, popSize);

    cout << "==================" << endl;
    runFastBatchEvaluator(maxIter, popSize);

    PluginManager::instance()->unloadPlugins();

    return 0;
}
