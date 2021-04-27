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
#include <tigon/Tigon.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

class tst_pluginsystem : public QObject
{
    Q_OBJECT

public:

private slots:
    void initTestCase();
    void test_loading();
    void test_registration();
    void test_unloading();
    void test_usePluginInWorkflow();

};

TString PLUGIN_PATH;

void tst_pluginsystem::initTestCase()
{
    PLUGIN_PATH = QCoreApplication::applicationDirPath().toStdString();
}
void tst_pluginsystem::test_loading()
{
    PluginManager* pm = PluginManager::instance();
    std::cout << "Path: " << PLUGIN_PATH << std::endl;
    pm->definePath(PLUGIN_PATH);
    QCOMPARE(pm->path() == PLUGIN_PATH, true);

    pm->loadPlugins();

    TigonFunctionFactory* factory = TigonFunctionFactory::instance();
    TStringList functions = factory->registeredFunctions();
    cout << functions << endl;
    QCOMPARE(vectorContains(functions,
                            TString("Tigon::Representation::ExampleFunctionFromPlugin")),
             true);

    IFunction* func = factory->createFunction("Tigon::Representation::ExampleFunctionFromPlugin");
    QCOMPARE(func == 0, false);
    QCOMPARE(func->className(), TString("Tigon::Representation::ExampleFunctionFromPlugin"));

    delete func;
}

void tst_pluginsystem::test_registration()
{
    TigonFunctionFactory* factory = TigonFunctionFactory::instance();
    TStringList functions = factory->registeredFunctions();
    // The plugin manager and TigonFunctionFactory are static instances,
    // the memory should not be cleared.
    QCOMPARE(vectorContains(functions,
                            TString("Tigon::Representation::ExampleFunctionFromPlugin")),
             true);

    IFunction* func = factory->createFunction("Tigon::Representation::ExampleFunctionFromPlugin");
    QCOMPARE(func == 0, false);
    QCOMPARE(func->className(), TString("Tigon::Representation::ExampleFunctionFromPlugin"));

    func->TP_defineNInputs(5);
    func->TP_defineNOutputs(2);

    QCOMPARE(func->TP_nInputs(), 5);
    QCOMPARE(func->TP_nOutputs(), 2);

    delete func;
}


void tst_pluginsystem::test_unloading()
{
    PluginManager* pm = PluginManager::instance();
    QCOMPARE(pm->nplugins(), 1);
    pm->unloadPlugins();

    QCOMPARE(pm->nplugins(), 0);

    TigonFunctionFactory* factory = TigonFunctionFactory::instance();
    QCOMPARE(vectorContains(factory->registeredFunctions(),
                            TString("Tigon::Representation::ExampleFunctionFromPlugin")),
             false);


}

void tst_pluginsystem::test_usePluginInWorkflow()
{
    // Load plugins
    PluginManager* pm = PluginManager::instance();
    std::cout << "Path: " << PLUGIN_PATH << std::endl;
    pm->definePath(PLUGIN_PATH);
    QCOMPARE(pm->path() == PLUGIN_PATH, true);
    pm->loadPlugins();

    // Configuration
    int dVecSize = 5;
    int oVecSize = 2;
    int populationSize = 50;
    int nIters = 100;

    TigonFunctionFactory* funcFact = TigonFunctionFactory::instance();

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(
                funcFact->createFunction("Tigon::Representation::ExampleFunctionFromPlugin"));
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit* init     = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    MOEAD* moead = new MOEAD(evaluator);

    for (int i=0; i<nIters; i++) {
        moead->evaluate();
    }

    ISet* solution  = nonDominanceSort(moead->set(0))[0];
    for(int j=0;j<solution->size();j++) {
        TVector<double> obj = solution->at(j)->doubleObjectiveVec();
        double sum = std::accumulate(obj.begin(), obj.end(), 0.0);
        QCOMPARE(sum < 1.5, true);
        //cout << obj << endl;
    }

    delete moead;
    delete evaluator;
    delete init;
    delete prob;
    delete base;

    pm->unloadPlugins();
}
QTEST_GUILESS_MAIN(tst_pluginsystem)

#include "tst_pluginsystem.moc"
