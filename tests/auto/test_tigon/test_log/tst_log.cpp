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

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Log;

class tst_log : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_constructors();
    void test_evaluationEntry();
    void test_populationEntry();
    void test_initialSetFromPopEntry();
    void test_initialSetFromEvalEntries();
    void test_saveAndLoadPopulationFromLogFile();
    void test_saveAndLoadPopulationFromEvaluationsLogFile();
    void test_saveAndLoadPopulationFromPopulationsLogFile();
};

void tst_log::test_constructors()
{
    LogManager      logObject;
    LogManagerSPtr  logSharedPointer(new LogManager);

    logSharedPointer.reset();
}

void tst_log::test_evaluationEntry()
{
    TRAND.defineSeed(0);

    int popSize = 6;

    PSetBase*     base = new PSetBase;
    IFormulation* form = new IFormulation(base);
    RandomInit*  init = new RandomInit(form);
    Evaluator*    eval = new Evaluator(init);

    IFunctionSPtr func(new DTLZ1);
    form->appendFunction(func);

    init->TP_defineSetSize(popSize);

    eval->TP_defineParallel(false);

    LogManagerSPtr log = eval->log();
    log->defineLogAll(true);

    eval->evaluate();

    JsonObject logData = log->evaluationsLog();

    //qDebug() << logData;
    QCOMPARE(logData[LEvals].toArray().size(), popSize);
    // output the log

    // check it works with parallel evaluation too, and that the output is
    // different (CODeM1 should have a different evaluation every time)
    eval->TP_defineParallel(true);
    for(int i = 0; i < popSize; i++) {
        eval->outputSet(0)->at(i)->defineEvaluated(false);
    }

    eval->evaluate();
    logData = log->evaluationsLog();
    QCOMPARE(logData[LEvals].toArray().size(), 2*popSize); // previous entries are still there
    //qDebug() << logData;
}

void tst_log::test_populationEntry()
{
    TRAND.defineSeed(0);

    int popSize = 2;

    PSetBase*     base = new PSetBase;
    IFormulation* form = new IFormulation(base);
    RandomInit*   init = new RandomInit(form);
    Evaluator*    eval = new Evaluator(init);

    IFunctionSPtr func(new DTLZ1);
    form->appendFunction(func);

    init->TP_defineSetSize(popSize);

    LogManagerSPtr log = eval->log();

    init->evaluate();
    ISet* pop = init->outputSet(0);
    TString name("Not evaluated yet");

    // This is not triggered by any Tigon operator at the moment
    log->logPopulation(pop, name);

    /// \note toJsonArray() in JsonUtils.cpp is modified such that only
    /// evaluated solutions are logged.
    JsonArray logData = log->populationsLog()[LPops].toArray();
    QCOMPARE(logData.size(), 1);
    JsonObject popObj = logData[0].toObject();
    QCOMPARE(popObj.contains(Log::LName), true);
    QCOMPARE(popObj.contains(Log::LTags), false);
    QCOMPARE(popObj[Log::LName].toString() == name, true);

    JsonObject solObj = popObj[Log::LSolutions].toArray()[0].toObject();
    QCOMPARE(solObj.contains(Log::LDVec), false);
    QCOMPARE(solObj.contains(Log::LWVec), false);
    QCOMPARE(solObj.contains(Log::LObjEval), false);
    QCOMPARE(solObj[Log::LObjEval].toBool(), false);
    QCOMPARE(solObj.contains(Log::LConstEval), false);
    QCOMPARE(solObj[Log::LConstEval].toBool(), false);
    QCOMPARE(solObj.contains(Log::LOVec), false);
    QCOMPARE(solObj.contains(Log::LCVec), false);
    QCOMPARE(solObj.contains(Log::LScalarised), false);
    QCOMPARE(solObj.contains(Log::LCost), false);

    eval->evaluate();
    name = ""; // no name is provided, so the tags are recorded instead
    JsonObject customFields;
    TString budget("Used Budget");
    customFields[budget] = 20;
    TString iterationCount("Iteration");
    customFields[iterationCount] = 8;
    log->logPopulation(pop, name, customFields);

    logData = log->populationsLog()[LPops].toArray();
    QCOMPARE(logData.size(), 2);
    popObj = logData[1].toObject();
    QCOMPARE(popObj.contains(Log::LName), false);
    QCOMPARE(popObj.contains(Log::LTags), true);

    TStringList lst = pop->tags();

    QCOMPARE(popObj[Log::LTags].toString(), join(lst, Tigon::TagDelimiter));
    QCOMPARE(popObj.contains(budget), true);
    QCOMPARE(popObj[budget].toInt(), 20);
    QCOMPARE(popObj.contains(iterationCount), true);
    QCOMPARE(popObj[iterationCount].toInt(), 8);

    solObj = popObj[Log::LSolutions].toArray()[0].toObject();
    QCOMPARE(solObj.contains(Log::LDVec), true);
    QCOMPARE(solObj.contains(Log::LWVec), true);
    QCOMPARE(solObj.contains(Log::LObjEval), true);
    QCOMPARE(solObj[Log::LObjEval].toBool(), true);
    QCOMPARE(solObj.contains(Log::LConstEval), true);
    QCOMPARE(solObj[Log::LConstEval].toBool(), true);
    QCOMPARE(solObj.contains(Log::LOVec), true);
    QCOMPARE(solObj.contains(Log::LCVec), false); // it is empty
    QCOMPARE(solObj.contains(Log::LScalarised), true);
    QCOMPARE(solObj.contains(Log::LCost), true);
}

ProblemSPtr testProblem()
{
    int nDVars  = 8;
    int nParams = 5;
    int nObjs   = 5;
    int nConsts = 2;

    /// Define the problem
    IFunctionSPtr func(new DTLZ2);
    func->TP_defineNInputs(nDVars+nParams);
    func->TP_defineNOutputs(nObjs);
    TVector<int> params;
    for(int i=0; i<nParams; i++) {
        params << nDVars + i;
    }
    TVector<int> constraints;
    for(int i=0; i<nConsts; i++) {
        constraints << i;
    }
    ProblemSPtr   prob(new Problem);
    prob->appendFunction(func, params, constraints);
    prob->processProblemDefinition();

    /// Decision variable uncertainty
    //  Package tab thickness
    BoxConstraintsDataSPtr box = prob->boxConstraints();
    UncertaintyMapping* umap = new UncertaintyMapping;
            prob->redefineDVarType(1, OrdinalType);
    box->replaceLowerBound(1, IElement(OrdinalType, 0));
    box->replaceUpperBound(1, IElement(OrdinalType, 1));
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
    // Pressure load
    goals[0]->defineValue(70.0);
    // Max Y cell regional displacement
    goals[1]->defineValue(1.625);
    // Max Y delta between cells
    goals[2]->defineValue(1.2);
    // Left tab max axial
    goals[3]->defineValue(90.0);
    // Right tab max axial
    goals[4]->defineValue(90.0);

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

void tst_log::test_initialSetFromPopEntry()
{
    int popSize = 4;

    ProblemSPtr prob = testProblem();

    /// one workflow to create some evaluated solutions
    PSetBase*      base  = new PSetBase();
    IFormulation*  form  = new IFormulation(base);
    LHSInit*       init  = new LHSInit(form);
    Evaluator*     eval  = new Evaluator(init);

    form->defineProblem(prob);

    init->TP_defineSetSize(popSize);

    LogManagerSPtr log = eval->log();
    log->defineLogAll(true);

    eval->TP_defineParallel(false);
    eval->evaluate();

    ISet* pop = eval->outputSet(0);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(pop->at(i)->isEvaluated(), true);
    }

    // This is not triggered by any Tigon operator at the moment
    TString name("Original Population");
    log->logPopulation(pop, name);


    /// Second workflow for populating from population entry
    PSetBase*          base2  = new PSetBase();
    IFormulation*      form2  = new IFormulation(base2);
    UserDefinedInit*   init2  = new UserDefinedInit(form2);

    form2->defineProblem(prob);
    init2->TP_defineSetSize(0);

    JsonDocument jdoc;
    jdoc.setObject(log->populationsLog());
    init2->defineSetFromJson(jdoc.toJson());

    init2->evaluate();

    // Check the solutions are appended, and are already evaluated
    ISet* solSet = init2->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), popSize);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);
    }

    delete init2;
    delete form2;
    delete base2;
    delete eval;
    delete init;
    delete form;
    delete base;
}

void tst_log::test_initialSetFromEvalEntries()
{
    int popSize = 4;

    ProblemSPtr prob = testProblem();

    /// one workflow to create some evaluated solutions
    PSetBase*      base  = new PSetBase();
    IFormulation*  form  = new IFormulation(base);
    LHSInit*       init  = new LHSInit(form);
    Evaluator*     eval  = new Evaluator(init);

    form->defineProblem(prob);

    init->TP_defineSetSize(popSize);

    LogManagerSPtr log = eval->log();
    log->defineLogAll(true);

    eval->TP_defineParallel(false);
    eval->evaluate();

    ISet* pop = eval->outputSet(0);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(pop->at(i)->isEvaluated(), true);
    }

    // This is not triggered by any Tigon operator at the moment
    TString name("Original Population");
    log->logPopulation(pop, name);

    /// Second workflow for populating from evaluation entries
    PSetBase*         base2  = new PSetBase();
    IFormulation*     form2  = new IFormulation(base2);
    UserDefinedInit*  init2  = new UserDefinedInit(form2);

    form2->defineProblem(prob);
    init2->TP_defineSetSize(0);

    JsonDocument jdoc;
    jdoc.setObject(log->evaluationsLog());
    init2->defineSetFromJson(jdoc.toJson());

    init2->evaluate();

    // Check the solutions are appended, and are already evaluated
    ISet* solSet = init2->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), popSize);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);
    }

    delete init2;
    delete form2;
    delete base2;
    delete eval;
    delete init;
    delete form;
    delete base;
}

void tst_log::test_saveAndLoadPopulationFromLogFile()
{
    int popSize = 4;

    ProblemSPtr prob = testProblem();

    /// one workflow to create some evaluated solutions
    PSetBase*      base  = new PSetBase();
    IFormulation*  form  = new IFormulation(base);
    LHSInit*       init  = new LHSInit(form);
    Evaluator*     eval  = new Evaluator(init);

    form->defineProblem(prob);

    init->TP_defineSetSize(popSize);

    LogManagerSPtr log = eval->log();
    log->defineLogAll(true);

    eval->TP_defineParallel(false);
    eval->evaluate();

    ISet* pop = eval->outputSet(0);
    log->logPopulation(pop);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(pop->at(i)->isEvaluated(), true);
    }

    /// Save the populations log to a file
    QString fileName("BatteryLog.json");
    QFile jFile(fileName);
    if (!jFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }

    JsonDocument jdoc(log->all());
    jFile.write(jdoc.toJson().c_str());

    jFile.close();

    /// Second workflow for loading the population from the log
    PSetBase*         base2  = new PSetBase();
    IFormulation*     form2  = new IFormulation(base2);
    UserDefinedInit*  init2  = new UserDefinedInit(form2);

    form2->defineProblem(prob);
    init2->TP_defineSetSize(0);

    QFile ifile(fileName);
    bool fileExists = ifile.open(QFile::ReadOnly);
    QCOMPARE(fileExists, true);

    init2->defineInitialSetFromFile(fileName.toStdString());

    init2->evaluate();

    // Check the solutions are appended, and are already evaluated
    ISet* solSet = init2->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), popSize);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);
    }

    delete init2;
    delete form2;
    delete base2;
    delete eval;
    delete init;
    delete form;
    delete base;
}

void tst_log::test_saveAndLoadPopulationFromEvaluationsLogFile()
{
    int popSize = 4;

    ProblemSPtr prob = testProblem();

    /// one workflow to create some evaluated solutions
    PSetBase*      base  = new PSetBase();
    IFormulation*  form  = new IFormulation(base);
    LHSInit*       init  = new LHSInit(form);
    Evaluator*     eval  = new Evaluator(init);

    form->defineProblem(prob);

    init->TP_defineSetSize(popSize);

    LogManagerSPtr log = eval->log();
    log->defineLogAll(true);

    eval->TP_defineParallel(false);
    eval->evaluate();

    ISet* pop = eval->outputSet(0);
    log->logPopulation(pop);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(pop->at(i)->isEvaluated(), true);
    }

    /// Save the populations log to a file
    QString fileName("BatteryEvaluationLog.json");
    QFile jFile(fileName);
    if (!jFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }

    JsonDocument jdoc(log->evaluationsLog());
    jFile.write(jdoc.toJson().c_str());
    jFile.close();

    /// Second workflow for loading the population from the log
    PSetBase*          base2  = new PSetBase();
    IFormulation*      form2  = new IFormulation(base2);
    UserDefinedInit*   init2  = new UserDefinedInit(form2);

    form2->defineProblem(prob);
    init2->TP_defineSetSize(0);

    QFile ifile(fileName);
    bool fileExists = ifile.open(QFile::ReadOnly);
    QCOMPARE(fileExists, true);

    init2->defineInitialSetFromFile(fileName.toStdString());

    init2->evaluate();

    // Check the solutions are appended, and are already evaluated
    ISet* solSet = init2->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), popSize);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);
    }

    delete init2;
    delete form2;
    delete base2;
    delete eval;
    delete init;
    delete form;
    delete base;
}

void tst_log::test_saveAndLoadPopulationFromPopulationsLogFile()
{
    int popSize = 4;

    ProblemSPtr prob = testProblem();

    /// one workflow to create some evaluated solutions
    PSetBase*      base  = new PSetBase();
    IFormulation*  form  = new IFormulation(base);
    LHSInit*       init  = new LHSInit(form);
    Evaluator*     eval  = new Evaluator(init);

    form->defineProblem(prob);

    init->TP_defineSetSize(popSize);

    LogManagerSPtr log = eval->log();
    log->defineLogAll(true);

    eval->TP_defineParallel(false);
    eval->evaluate();

    ISet* pop = eval->outputSet(0);
    log->logPopulation(pop);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(pop->at(i)->isEvaluated(), true);
    }

    /// Save the populations log to a file
    QString fileName("BatteryPopulationLog.json");
    QFile jFile(fileName);
    if (!jFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
    }

    JsonDocument jdoc(log->populationsLog());
    jFile.write(jdoc.toJson().c_str());
    jFile.close();

    /// Second workflow for loading the population from the log
    PSetBase*      base2  = new PSetBase();
    IFormulation*  form2  = new IFormulation(base2);
    UserDefinedInit*  init2  = new UserDefinedInit(form2);

    form2->defineProblem(prob);
    init2->TP_defineSetSize(0);

    init2->defineInitialSetFromFile(fileName.toStdString());

    init2->evaluate();

    // Check the solutions are appended, and are already evaluated
    ISet* solSet = init2->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), popSize);

    for(int i = 0; i < popSize; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);
    }

    delete init2;
    delete form2;
    delete base2;
    delete eval;
    delete init;
    delete form;
    delete base;
}

QTEST_GUILESS_MAIN(tst_log)

#include "tst_log.moc"
