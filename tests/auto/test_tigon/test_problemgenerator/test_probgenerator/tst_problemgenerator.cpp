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

#include <QDir>
#include <iostream>

#include <tigon/Tigon.h>

using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace std;

class tst_problemgenerator : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_properties();
    void test_MultiFunctionPathes();
    void test_SingleFunctionPath();
    void test_InternalProb_DTLZProbs();
    void test_LoadLib();
    void test_ExternalProb_SharedLib();
    void test_Properties();
    void test_UncertaintyMapping();
    void test_Parameters();
};

void inline printLog(ProblemGenerator* pg)
{
    cout << "\n======= Test Problem =======" << endl;
    cout << "Name         : " << pg->problemName() << endl;
    cout << "Description  : " << pg->problemDescription() << endl;
    cout << "ID           : " << pg->problemID() << endl;
    foreach(IFunctionSPtr func, pg->problem()->functionVec()) {
        cout << endl;
        cout << "Function Name: " << func->name() << endl;
        cout << "Function Desp: " << func->description() << endl;
        cout << "Function ID  : " << func->ID()<< endl;
        cout << endl;
    }
}

void inline print2DVector(TVector<TVector<int> > vec)
{
    for(int i=0; i<vec.size(); i++) {
        cout << "[ ";
        for(int j=0; j<vec[i].size(); j++) {
            cout << vec[i][j] << "\t";
        }
        cout << "]" << endl;
    }
    cout << endl;
}

void tst_problemgenerator::test_properties()
{
    ProblemGenerator* pg = new ProblemGenerator();
    std::cout << join(pg->propertyNames(), ";") << std::endl;
    QCOMPARE(pg->propertyCount(), 25);
}

void tst_problemgenerator::test_MultiFunctionPathes()
{
    int nVar = 5;
    int nObj = 2;

    TString expectedPath = "Tigon::Representation::DTLZ1;Tigon::Representation::DTLZ2";

    TStringList probs;
    probs << "DTLZ1" << "DTLZ2";

    PSetBase* base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    ProblemSPtr prob = pg->problem();
    foreach(const TString& probName, probs) {
        IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, "Tigon::Representation::" + probName);
        func->TP_defineNInputs(nVar);
        func->TP_defineNOutputs(nObj);
        prob->appendFunction(func);
    }
    pg->defineProblem(prob);
    pg->evaluate();

    std::cout << pg->functionPathes() << endl;
    TString pathes = pg->functionPathes();
    bool res = expectedPath == pathes;
    QCOMPARE(res, true);

    delete base;
    delete pg;
}

void tst_problemgenerator::test_SingleFunctionPath()
{
    int nVar = 5;
    int nObj = 2;

    TString expectedPath = "Tigon::Representation::DTLZ1";

    TStringList probs;
    probs << "DTLZ1";

    PSetBase* base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    ProblemSPtr prob = pg->problem();
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, "Tigon::Representation::DTLZ1");
    func->TP_defineNInputs(nVar);
    func->TP_defineNOutputs(nObj);
    prob->appendFunction(func);
    pg->defineProblem(prob);
    pg->evaluate();

    std::cout << pg->functionPathes() << endl;
    TString pathes = pg->functionPathes();
    bool res = expectedPath == pathes;
    QCOMPARE(res, true);

    delete base;
    delete pg;
}

void tst_problemgenerator::test_InternalProb_DTLZProbs()
{
    int nVar = 5;
    int nObj = 2;

    TStringList probs;
    probs << "DTLZ1" << "DTLZ2";

    foreach(const TString& probName, probs) {
        PSetBase* base = new PSetBase();
        ProblemGenerator* pg = new ProblemGenerator(base);
        IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal,
                                                "Tigon::Representation::" + probName);
        ProblemSPtr prob = pg->problem();
        func->TP_defineNInputs(nVar);
        func->TP_defineNOutputs(nObj);
        QCOMPARE(func->TP_nOutputs(), nObj);
        QCOMPARE(func->TP_nInputs(),  nVar);

        prob->appendFunction(func);
        pg->defineProblem(prob);
        pg->evaluate();

        printLog(pg);

        QCOMPARE(pg->functionVec()[0]->TP_nInputs(),  nVar);
        QCOMPARE(pg->functionVec()[0]->TP_nOutputs(), nObj);
        QCOMPARE(pg->decisionVecSize(),  nVar);
        QCOMPARE(pg->objectiveVecSize(), nObj);
        QCOMPARE(pg->problem()->functionVec()[0]->name(), probName);
        delete base;
        delete pg;
    }
}

void tst_problemgenerator::test_LoadLib()
{
    QSKIP("SharedLib is not working.");
    TString fileName = QString(QCoreApplication::applicationDirPath()
            + QDir::separator() + "testprob").toStdString();
    QLibrary* refLib = new QLibrary(QString::fromStdString(fileName));
    typedef IFunction* (*RefTLibFuncFromSharedLib) ();
    QCOMPARE(refLib->load(), true);

    RefTLibFuncFromSharedLib libFunc =
            (RefTLibFuncFromSharedLib) refLib->resolve("funcFromSharedLib");
    if(libFunc != 0) {
        cout << "Shared Library loaded. :)" << endl;
    } else {
        cout << "Shared Library not loaded. :(" << endl;
    }
    bool res = libFunc == 0;
    QCOMPARE(res, false);

    // cppcheck-suppress nullPointerRedundantCheck
    IFunction* func = libFunc();
    cout << "\tTest Prob From Lib - name:        "
         << func->name() << endl;
    cout << "\tTest Prob From Lib - description: "
         << func->description() << endl;
    QCOMPARE(func->name(), TString("TestProb"));
}

void tst_problemgenerator::test_ExternalProb_SharedLib()
{
    QSKIP("SharedLib is not working.");
    PSetBase* base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    TString fileName = QString(QCoreApplication::applicationDirPath()
            + QDir::separator() + "testprob").toStdString();
    IFunctionSPtr func = pg->createFunction(FuncTypeStrSharedLib, fileName);

    int nvar = func->inputPrpts().size();
    int nobj = func->outputPrpts().size();
    TString name = func->name();

    ProblemSPtr prob = pg->problem();
    prob->appendFunction(func);
    pg->defineProblem(prob);
    pg->evaluate();

    printLog(pg);

    QCOMPARE(pg->decisionVecSize(),  nvar);
    QCOMPARE(pg->objectiveVecSize(), nobj);
    QCOMPARE(pg->problem()->functionVec()[0]->name(), name);
    delete base;
    delete pg;
}

void tst_problemgenerator::test_Properties()
{
    ProblemGenerator* pg = new ProblemGenerator(new PSetBase);
    std::cout << "pg->propertyCount()  " << pg->propertyCount() << std::endl;
    for(int i=0; i<pg->propertyCount(); i++) {
        TString name = pg->propertyNames()[i];
        std::cout << name << "\t = " << pg->propertyValue(name) << std::endl;
    }
    delete pg;
}

ProblemSPtr testProblem()
{
    int nDVars  = 8;
    int nParams = 5;
    int nObjs   = 5;

    /// Define the problem
    IFunctionSPtr func(new DTLZ2);
    func->TP_defineNInputs(nDVars+nParams);
    func->TP_defineNOutputs(nObjs);
    TVector<int> params;
    for(int i=0; i<nParams; i++) {
        params << nDVars + i;
    }
    ProblemSPtr   prob(new Problem);
    prob->appendFunction(func, params);
    prob->processProblemDefinition();

    /// Decision variable uncertainty
    //  Package tab thickness
    BoxConstraintsDataSPtr box = prob->boxConstraints();
    UncertaintyMapping* umap = new UncertaintyMapping;
    prob->redefineDVarType(0, OrdinalType);
    prob->redefineDVarType(1, OrdinalType);
    box->replaceLowerBound(1, IElement(OrdinalType, 0));
    box->replaceUpperBound(1, IElement(OrdinalType, 1));
    prob->redefineDVarType(5, NominalType);
    prob->redefineDVarType(6, NominalType);
    prob->redefineDVarType(7, NominalType);
    prob->redefineDVarType(9, NominalType);
    prob->redefineDVarType(10, NominalType);
    prob->redefineDVarType(11, OrdinalType);

    TVector<double> constTerms;
    constTerms << 0.9 << 0.05;
    TVector<double> linTerms;
    linTerms << 0.35 << 0.01;
    umap->defineDistributionType(NormalDistType);
    umap->defineConstTerms(constTerms);
    umap->defineLinearTerms(linTerms);
    prob->redefineDVarUncertainty(1, umap);

    /// Function output uncertainty
    //  Pressure load
    umap = new UncertaintyMapping;
    fill(constTerms, 0.0);
    linTerms[0] = 0.7;
    linTerms[1] = 1.3;
    umap->defineDistributionType(UniformDistType);
    umap->defineConstTerms(constTerms);
    umap->defineLinearTerms(linTerms);
    prob->redefineFuncOutputUncertainty(0, 0, umap);

    /// Goals
    TVector<IElementSPtr> goals = prob->goalVector();
    TVector<bool> setGoals = prob->setGoalVector();
    // Pressure load
    goals[0]->defineValue(70.0); setGoals[0] = true;
    // Max Y cell regional displacement
    goals[1]->defineValue(1.625); setGoals[1] = true;
    // Max Y delta between cells
    goals[2]->defineValue(1.2); setGoals[2] = true;
    // Left tab max axial
    goals[3]->defineValue(90.0); setGoals[3] = true;
    // Right tab max axial
    goals[4]->defineValue(90.0); setGoals[4] = true;

    prob->defineSetGoalVector(setGoals);

    /// Parameters
    TVector<double> samps;
    TVector<IElementSPtr> paramVec = prob->parameterVector();
    // Pressure
    paramVec[0]->defineDist(new NormalDistribution(0.5, 0.1));
    // Load curve - low / medium / high
    samps << 1.0 << 2.0 << 3.0;
    paramVec[1]->defineDist(new SampledDistribution(samps));
    // Load axis - X / Y
    samps.resize(2);
    paramVec[2]->defineDist(new SampledDistribution(samps));
    // Load angle - 0 / 45
    samps.clear();
    samps << 0.0 << 45.0;
    paramVec[3]->defineDist(new SampledDistribution(samps));
    // Temperature
    paramVec[4]->defineDist(new NormalDistribution(20.0, 1.0));

    return prob;
}

void tst_problemgenerator::test_UncertaintyMapping()
{
    /// Define the problem
    ProblemSPtr   prob = testProblem();
    ProblemDefinitionStatus status = prob->processProblemDefinition();
    QCOMPARE(status, FullyDefined);

    ProblemGenerator* pg = new ProblemGenerator;
    pg->defineProblem(prob);

    TVector<UncertaintyMapping*> dUncertainties = pg->dVecUncertainties();
    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties = pg->funcOutUncertainties();

    TString dVecUncertaintiesJson = pg->dVecUncertaintiesJson();
    TString funcOutUncertaintiesJson = pg->funcOutUncertaintiesJson();

    pg->defineDVecUncertaintiesFromJson(dVecUncertaintiesJson);
    pg->defineFuncOutVecUncertaintiesFromJson(funcOutUncertaintiesJson);

    pg->processFormulation();

    TVector<UncertaintyMapping*> dUncertainties2
            = pg->dVecUncertainties();
    TVector<TVector<UncertaintyMapping*> > funcOutUncertainties2
            = pg->funcOutUncertainties();

    QCOMPARE(dUncertainties.size(), dUncertainties2.size());
    QCOMPARE(funcOutUncertainties.size(), funcOutUncertainties2.size());

    TString dVecUncertaintiesJson2 = pg->dVecUncertaintiesJson();
    TString funcOutUncertaintiesJson2 = pg->funcOutUncertaintiesJson();

    trim(dVecUncertaintiesJson);
    trim(dVecUncertaintiesJson2);
    QCOMPARE(dVecUncertaintiesJson, dVecUncertaintiesJson2);

    trim(funcOutUncertaintiesJson);
    trim(funcOutUncertaintiesJson2);
    QCOMPARE(funcOutUncertaintiesJson, funcOutUncertaintiesJson2);
}

void tst_problemgenerator::test_Parameters()
{
    int nInput = 5;
    int nOutput = 3;

    ProblemGenerator* pg = new ProblemGenerator(new PSetBase);
    ProblemSPtr prob = pg->problem();
    IFunctionSPtr fun1 = pg->createFunction(FuncTypeStrInternal,
                                            "Tigon::Representation::CTP1");
    TVector<int> param;
    param << 0;
    prob->appendFunction(fun1,param);
    prob->processProblemDefinition();
    pg->defineProblem(prob);

    IFunctionSPtr fun2 = pg->createFunction(FuncTypeStrInternal,
                                            "Tigon::Representation::DTLZ1");
    fun2->TP_defineNInputs(nInput);
    fun2->TP_defineNOutputs(nOutput);

    prob->appendFunction(fun2);

    QCOMPARE(prob->processProblemDefinition() == Tigon::FullyDefined, true);
}

QTEST_GUILESS_MAIN(tst_problemgenerator)

#include "tst_problemgenerator.moc"


