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

#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

class tst_sparego : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_NeighbourhoodFiltration();
    void test_SparegoInit();
    void test_SimplexLatticeDirectionIterator();
    void test_PreferencesLattice();
    void test_GeneralizedDecomposition();
    void test_SparegoValidation();
    void test_UncertainRobustness();
    void test_DirectionFiltration();
    void test_FitnessBasedInit();
    void test_ExpectedImprovement();
    void test_RandExpectedImprovement();
    void test_SurrogateBasedOptimizer();
    void test_sParEGO_workflow();
    void test_sParEGO();
    void test_LogDirectionVector();
    void test_sParEGOWithMonteCarloValidation();
};

inline void writeVector(QTextStream &st, TVector<double> vec, QString sep = "\t", QString endLine = "\n")
{
    for(int i=0; i<vec.size(); i++) {
        st << vec[i];
        if(i<vec.size()-1) {
            st << sep;
        }
    }
    st << endLine;
}

ProblemSPtr testProblem()
{
    int nDVars  = 8;
    int nParams = 5;
    int nConsts = 2;

    /// Define the problem
    IFunctionSPtr func(new DTLZ2);
    func->TP_defineNInputs(nDVars+nParams);
    func->TP_defineNOutputs(5);
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
    goals[0]->defineValue(70.0);
    goals[1]->defineValue(1.625);
    goals[2]->defineValue(1.2);
    goals[3]->defineValue(90.0);
    goals[4]->defineValue(90.0);

    /// Parameters
    TVector<double> samps;
    TVector<IElementSPtr> paramVec = prob->parameterVector();
    paramVec[0]->defineDist(new NormalDistribution(0.5, 0.1));
    samps << 1.0 << 2.0 << 3.0;
    paramVec[1]->defineDist(new SampledDistribution(samps));
    samps.resize(2);
    paramVec[2]->defineDist(new SampledDistribution(samps));
    samps.clear();
    samps << 0.0 << 45.0;
    paramVec[3]->defineDist(new SampledDistribution(samps));
    paramVec[4]->defineDist(new NormalDistribution(20.0, 1.0));

    return prob;
}

ProblemSPtr testProblem2()
{
    int nDVars  = 8;
    int nParams = 5;
    int nConsts = 2;
    int nOutputs =5;

    /// Define the problem
    IFunctionSPtr func(new DTLZ2);
    func->TP_defineNInputs(nDVars+nParams);
    func->TP_defineNOutputs(nOutputs);
    TVector<int> params;
    for(int i=0; i<nParams; i++) {
        params << nDVars + i;
    }
    TVector<int> constraints;
    for(int i=0; i<nConsts; i++) {
        constraints << i;
    }
    TVector<int> objectives;
    for(int i=nConsts; i<nOutputs; i++) {
        objectives << i;
    }
    ProblemSPtr   prob(new Problem);
    prob->appendFunction(func, params, constraints, objectives);
    prob->processProblemDefinition();

    /// Parameters
    TVector<double> samps;
    TVector<IElementSPtr> paramVec = prob->parameterVector();
    paramVec[0]->defineDist(new NormalDistribution(0.5, 0.1));
    samps << 1.0 << 2.0 << 3.0;
    paramVec[1]->defineDist(new SampledDistribution(samps));
    samps.resize(2);
    paramVec[2]->defineDist(new SampledDistribution(samps));
    samps.clear();
    samps << 0.0 << 45.0;
    paramVec[3]->defineDist(new SampledDistribution(samps));
    paramVec[4]->defineDist(new NormalDistribution(20.0, 1.0));

    return prob;
}

void tst_sparego::test_NeighbourhoodFiltration()
{
    TRAND.defineSeed(0);
    int   popSize = 300;
    int   nSets   = 4;
    double radius  = 0.1;
    int   dVecSize   = 3;
    int   oVecSize   = 2;

    PSetBase*                base  = new PSetBase();
    IFormulation*            prob  = new IFormulation(base);
    LHSInit*                 init  = new LHSInit(prob);
    NeighbourhoodFiltration* nFilt = new NeighbourhoodFiltration(init);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(popSize);

    nFilt->TP_defineNeighbourhoodRadius(radius);
    nFilt->TP_defineMaxSolutions(nSets);

    nFilt->evaluate();
    TVector<ISet*> neighbourhoods = nFilt->outputSets();
    QCOMPARE(neighbourhoods.size() == nSets, true);

    delete nFilt;
    delete init;
    delete base;
}

void tst_sparego::test_SparegoInit()
{
    TRAND.defineSeed(0);
    int   dVecSize  = 15;
    int   oVecSize  = 3;
    int   popSize   = 200;
    double nRaduis   = 0.1;
    double pertRatio = 0.5;

    PSetBase*                 base  = new PSetBase();
    IFormulation*             prob  = new IFormulation(base);
    SparegoInit*              init  = new SparegoInit(prob);
    NeighbourhoodFiltration*  nFilt = new NeighbourhoodFiltration(init);

    init->addOutputTag(Tigon::TForNeighbourhoods);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    init->TP_definePerturbedRatio(pertRatio);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    nFilt->TP_defineNeighbourhoodRadius(nRaduis);

    nFilt->evaluate();


    TVector<ISet*> neighbourhoods = nFilt->outputSets();
    TVector<int> counts;
    for(int i=0; i<neighbourhoods.size(); i++) {
        int s = neighbourhoods[i]->size();
        if(counts.size() < s) {
            counts.resize(s);
        }
        counts[s-1]++;
    }

    delete nFilt;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_SimplexLatticeDirectionIterator()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int dVecSize = 7;
    int oVecSize = 3;
    int nDirs    = 13;
    int maxIter  = 20;

    PSetBase*               base  = new PSetBase();
    IFormulation*           prob  = new IFormulation(base);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(prob);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    dirs->TP_defineReferenceSetSize(nDirs);

    for(int iter = 1; iter<=maxIter; iter++) {
        dirs->evaluate();
    }
    cout << "Requested direction vectors:\t" << nDirs << endl;
    cout << "Total direction vectors:\t" << dirs-> actualReferenceSetSize() << endl;

    delete dirs;
    delete prob;
    delete base;
}

void tst_sparego::test_PreferencesLattice()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int dVecSize = 7;
    int oVecSize = 3;
    int nDirs    = 13;
    int popSize  = 30;
    int nIter    = 20;
    TVector<double> goals;
    goals << 1.0 << 2.0 << 1.0;
    TVector<ElementType> types(3, RealType);

    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    LHSInit*                  init = new LHSInit(prob);
    Evaluator*                eval = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);
    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    dirs->TP_defineReferenceSetSize(nDirs);

    init->TP_defineSetSize(popSize);

    for(int iter = 1; iter<=nIter; iter++) {
        dirs->evaluate();
    }
    delete dirs;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_GeneralizedDecomposition()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int dVecSize = 7;
    int oVecSize = 3;
    int nDirs    = 30;
    int popSize  = 15;
    int nIter    = 5;

    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    LHSInit*                  init = new LHSInit(prob);
    Evaluator*                eval = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition* scal = new GeneralizedDecomposition(dirs);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    dirs->TP_defineReferenceSetSize(nDirs);

    init->TP_defineSetSize(popSize);

    for(int iter=1; iter<=nIter; iter++) {
        scal->evaluate();
    }

    delete scal;
    delete eval;
    delete init;
    delete dirs;
    delete prob;
    delete base;
}

void tst_sparego::test_SparegoValidation()
{
    TRAND.defineSeed(0);

    int   dVecSize = 5;
    int   oVecSize = 2;
    int   popSize  = 10;
    int   nDirs    = 30;
    double nRaduis  = 0.1;

    PSetBase*                 base    = new PSetBase();
    IFormulation*             prob    = new IFormulation(base);
    SparegoInit*              init    = new SparegoInit(prob);
    Evaluator*                eval    = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition* scal    = new GeneralizedDecomposition(dirs);
    SparegoValidation*        valNom  = new SparegoValidation(scal);
    SparegoValidation*        valConf = new SparegoValidation(scal);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    dirs->TP_defineReferenceSetSize(nDirs);

    valNom->TP_defineNeighbourhoodRadius(nRaduis);
    valConf->TP_defineNeighbourhoodRadius(nRaduis);

    valNom->TP_defineConfidenceInSample(UnchangedDistribution); // this is the default
    valConf->TP_defineConfidenceInSample(ConfidenceDecrease);

    TVector<TVector<double> > nomObjMean(popSize, TVector<double>(oVecSize));
    TVector<TVector<double> > confObjMean(popSize, TVector<double>(oVecSize));
    TVector<TVector<double> > nomObjStd(popSize, TVector<double>(oVecSize));
    TVector<TVector<double> > confObjStd(popSize, TVector<double>(oVecSize));
    TVector<double> nomCostMean(popSize);
    TVector<double> confCostMean(popSize);
    TVector<double> nomCostStd(popSize);
    TVector<double> confCostStd(popSize);

    // Nominal
    valNom->evaluate();

    int exSize = 1;
    QCOMPARE(valNom->outputSets().size() == exSize, true);
    ISet* pop = valNom->outputSet(0);
    QCOMPARE(pop->size(), popSize);

    int nObj = valNom->objectiveVecSize();
    QCOMPARE(nObj == oVecSize, true);

    IDistribution* dist;
    for(int i = 0; i < popSize; i++) {
        dist = pop->at(i)->cost()->dist();
        QCOMPARE(dist == nullptr, false);
        nomCostMean[i] = dist->mean();
        nomCostStd[i] = dist->std();
        for(int j = 0; j < nObj; j++) {
            dist = pop->at(i)->objectiveVar(j)->dist();
            QCOMPARE(dist == nullptr, false);
            nomObjMean[i][j] = dist->mean();
            nomObjStd[i][j] = dist->std();
        }
    }

    // Confidence
    valConf->evaluateOnlyThisNode();

    exSize = 1;
    QCOMPARE(valConf->outputSets().size() == exSize, true);
    ISet* newPop = valConf->outputSet(0);
    QCOMPARE(newPop, pop);

    for(int i = 0; i < popSize; i++) {
        dist = pop->at(i)->cost()->dist();
        QCOMPARE(dist == nullptr, false);
        confCostMean[i] = dist->mean();
        confCostStd[i] = dist->std();
        for(int j = 0; j < nObj; j++) {
            dist = pop->at(i)->objectiveVar(j)->dist();
            QCOMPARE(dist == nullptr, false);
            confObjMean[i][j] = dist->mean();
            confObjStd[i][j] = dist->std();
        }
    }
    delete valConf;
    delete valNom;
    delete scal;
    delete dirs;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_UncertainRobustness()
{
    TRAND.defineSeed(0);

    int   dVecSize   = 5;
    int   oVecSize   = 2;
    int   popSize    = 100;
    int   nDirs      = 30;
    double nRaduis   = 0.1;
    TVector<double> goals;
    goals << 1.6 << 3.2;
    TVector<ElementType> types(2, RealType);

    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    SparegoInit*              init = new SparegoInit(prob);
    Evaluator*                eval = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition* scal    = new GeneralizedDecomposition(dirs);
    FreezeOperatorOutput*     scal2   = new FreezeOperatorOutput(scal);
    FreezeOperatorOutput*     scal3   = new FreezeOperatorOutput(scal2);
    SparegoValidation*        valNom  = new SparegoValidation(scal3);
    SparegoValidation*        valConf = new SparegoValidation(valNom);
    SparegoValidation*        valThresh = new SparegoValidation(valConf);


    /// Compare three different robustness assignment schemes

    // 1. Do not consider sample size (assume normal distribution for the sample
    //    mean and variance):
    RobustnessAssignment* iValNom = new RobustnessAssignment(valThresh);

    // 2. The mean and variance are random variables according to the sample
    //    size, and the 5th percentile is used for both to construct a
    //    normal distribution:
    RobustnessAssignment* iValConf = new RobustnessAssignment(iValNom);

    // 3. Same as 1, but with Threshold Indicator
    RobustnessAssignment* iValThresh = new RobustnessAssignment(iValConf);

    // Branch the modified distribution from the nominal
    scal2->defineInputTags(scal->outputTags());
    scal2->defineOutputTags(TStringList({TString("ForValConf")}));
    scal3->defineInputTags(scal->outputTags());
    scal3->defineOutputTags(TStringList({TString("ForValThresh")}));
    valConf->defineOutputTags(TStringList({TString("ForValConf")}));
    iValConf->defineOutputTags(TStringList({TString("ForValConf")}));
    valThresh->defineOutputTags(TStringList({TString("ForValThresh")}));
    iValThresh->defineOutputTags(TStringList({TString("ForValThresh")}));

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);
    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    init->TP_defineSetSize(popSize);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    dirs->TP_defineReferenceSetSize(nDirs);

    valNom->TP_defineNeighbourhoodRadius(nRaduis);
    valConf->TP_defineNeighbourhoodRadius(nRaduis);
    valThresh->TP_defineNeighbourhoodRadius(nRaduis);

    valNom->TP_defineConfidenceInSample(UnchangedDistribution); // this is the default
    valConf->TP_defineConfidenceInSample(ConfidenceDecrease);
    valThresh->TP_defineConfidenceInSample(UnchangedDistribution);

    iValNom->defineIndicator(ConfidenceType, 0.9);
    iValConf->defineIndicator(ConfidenceType, 0.9);
    iValThresh->defineIndicator(ThresholdType);

    TVector<double> nomCostMean(popSize);
    TVector<double> confCostMean(popSize);
    TVector<double> threshCostMean(popSize);
    TVector<double> nomCostStd(popSize);
    TVector<double> confCostStd(popSize);
    TVector<double> threshCostStd(popSize);
    TVector<double> nomIndicator(popSize);
    TVector<double> confIndicator(popSize);
    TVector<double> threshIndicator(popSize);

    iValThresh->evaluate();

    // Nominal
    int exSize = 1;
    QCOMPARE(iValNom->outputSets().size() == exSize, true);
    ISet* pop = iValNom->outputSet(0);
    QCOMPARE(pop->size(), popSize);

    int nObj = iValNom->objectiveVecSize();
    QCOMPARE(nObj == oVecSize, true);

    IDistribution* dist;
    for(int i = 0; i < popSize; i++) {
        nomIndicator[i] = pop->at(i)->doubleCost();
        dist = pop->at(i)->cost()->dist();
        QCOMPARE(dist == nullptr, false);
        nomCostMean[i] = dist->mean();
        nomCostStd[i] = dist->std();
    }

    // Confidence
    exSize = 1;
    QCOMPARE(iValConf->outputSets().size() == exSize, true);
    ISet* newPopC = iValConf->outputSet(0);
    QCOMPARE(newPopC == nullptr, false);
    QCOMPARE(newPopC != pop, true);

    for(int i = 0; i < popSize; i++) {
        confIndicator[i] = newPopC->at(i)->doubleCost();
        dist = newPopC->at(i)->cost()->dist();
        QCOMPARE(dist == nullptr, false);
        confCostMean[i] = dist->mean();
        confCostStd[i] = dist->std();
    }

    // Threshold
    exSize = 1;
    QCOMPARE(iValThresh->outputSets().size() == exSize, true);
    ISet* newPopT = iValThresh->outputSet(0);
    QCOMPARE(newPopT == nullptr, false);
    QCOMPARE(newPopT != pop, true);

    for(int i = 0; i < popSize; i++) {
        threshIndicator[i] = newPopT->at(i)->doubleCost();
        dist = newPopT->at(i)->cost()->dist();
        QCOMPARE(dist == nullptr, false);
        threshCostMean[i] = dist->mean();
        threshCostStd[i] = dist->std();
    }

    delete iValThresh;
    delete iValConf;
    delete iValNom;
    delete valThresh;
    delete valConf;
    delete valNom;
    delete scal3;
    delete scal2;
    delete scal;
    delete dirs;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_DirectionFiltration()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int dVecSize = 7;
    int oVecSize = 3;
    int nDirs    = 200;
    int popSize  = 20;
    int maxSurrogateSize  = 9;

    PSetBase*                   base = new PSetBase();
    IFormulation*               prob = new IFormulation(base);
    SparegoInit*                init = new SparegoInit(prob);

    Evaluator*                  eval = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition*   scal = new GeneralizedDecomposition(dirs);
    DirectionFitnessFiltration* srt  = new DirectionFitnessFiltration(scal);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    dirs->TP_defineReferenceSetSize(nDirs);

    init->TP_defineSetSize(popSize);

    srt->TP_defineMaxSolutions(maxSurrogateSize);

    srt->evaluate();

    //Display the results:

    TVector<double> vec = srt->dirVec();
    cout << "dirVec = " << vec << endl;

    ISet* pop = init->outputSet(0);
    QCOMPARE(pop == nullptr, false);
    QCOMPARE(pop->size(), popSize);

    ISet* filtrated = srt->outputSet(0);
    QCOMPARE(filtrated == nullptr, false);
    QCOMPARE(filtrated->size(), maxSurrogateSize);


    delete srt;
    delete eval;
    delete init;
    delete dirs;
    delete prob;
    delete base;
}

void tst_sparego::test_FitnessBasedInit()
{
    TRAND.defineSeed(0);
    cout.precision(5);

    int   dVecSize = 10;
    int   oVecSize = 3;
    int   nDirs    = 200;
    int   popSize  = 50;
    double eliteRatio = 0.4;

    PSetBase*                 base  = new PSetBase();
    IFormulation*             prob  = new IFormulation(base);
    SparegoInit*              init  = new SparegoInit(prob);
    Evaluator*                eval  = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition* scal  = new GeneralizedDecomposition(dirs);
    FitnessEliteSelection*    elite = new FitnessEliteSelection(scal);
    FitnessBasedInit*         fInit = new FitnessBasedInit(elite);

    init->addOutputTag(Tigon::TForSelection);

    // FitnessBasedInit needs to be manually connected
    fInit->defineInputTags(init->outputTags());

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    dirs->TP_defineReferenceSetSize(nDirs);

    init->TP_defineSetSize(popSize);

    fInit->TP_defineSetSize(popSize);
    fInit->defineOriginalSolutionsRatio(eliteRatio);

    // for testing, varify the same solutions are selected by both operators
    elite->TP_defineEliteRatio(fInit->originalSolutionsRatio());

    fInit->evaluate();

    // Display the results
    ISet* eliteSet = elite->outputSet(0);
    ISet* fInitSet = fInit->outputSet(0);
    int exSize = (int)(eliteRatio * popSize);
    QCOMPARE(eliteSet == nullptr, false);
    QCOMPARE(fInitSet == nullptr, false);
    QCOMPARE(eliteSet->size(), exSize);
    QCOMPARE(fInitSet->size(), popSize);

    delete fInit;
    delete elite;
    delete scal;
    delete eval;
    delete init;
    delete dirs;
    delete prob;
    delete base;
}

void tst_sparego::test_ExpectedImprovement()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);


    IFunctionSPtr func = IFunctionSPtr(new Alpine2);
    func->TP_defineNInputs(1);
    prob->appendFunction(func);
    prob->evaluate();

    ISet* evaluatedPoints = new ISet;
    for(double i=0.0; i<=10.0; i+=2.3) {
        IMappingSPtr imap = prob->createOptimizationMapping(evaluatedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    for(double i=0.6; i<10.0; i*=2) {
        IMappingSPtr imap = prob->createOptimizationMapping(evaluatedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    init->defineInitialSet(evaluatedPoints);

    eval->evaluate();

    // Fit a Kriging model to the evaluated pionts
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(int i=0; i<eSet->size(); i++) {
        // decision vector
        xx.push_back(eSet->at(i)->doubleDecisionVec());
        // function value
        yy.push_back(eSet->at(i)->doubleCost());
    }

    PowerVariogram* powerV = new PowerVariogram(xx, yy, 1.999);
    KrigingSPtr km = KrigingSPtr(new Kriging(powerV));
    km->estimateErrors(true);

    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Create IFunctions from the Kriging model
    IFunctionSPtr func2 = IFunctionSPtr(new KrigingSurrogate(km));
    IFunctionSPtr func3 = IFunctionSPtr(new ExpectedImprovement(km));

    // Kriging model results
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);

    prob2->appendFunction(func2);
    prob2->appendFunction(func3);

    prob2->evaluate();

    // Create query points
    TVector<double> xq;
    ISet* queryPoints = new ISet;

    for(double x=0.0; x<=10.0; x += 0.01) {
        xq.push_back(x);
        IMappingSPtr imap = prob2->createOptimizationMapping(queryPoints);
        imap->defineDecisionVar(0, IElement(x));
    }

    init2->defineInitialSet(queryPoints);

    eval2->evaluate();


    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_RandExpectedImprovement()
{
    // Use one IPSet to evaluate a set of solutions on a single objective
    // problem
    PSetBase*                 base = new PSetBase();
    IFormulation*             prob = new IFormulation(base);
    UserDefinedInit*          init = new UserDefinedInit(prob);
    Evaluator*                eval = new Evaluator(init);
    eval->TP_defineSingleObjective(true);

    IFunctionSPtr func = IFunctionSPtr(new Alpine2);
    func->TP_defineNInputs(1);
    prob->appendFunction(func);
    prob->evaluate();

    ISet* evaluatedPoints = new ISet;
    TVector<Tigon::ElementType> tVec(1, RealType);
    for(double i=0.0; i<=10.0; i+=2.3) {
        IMappingSPtr imap = prob->createOptimizationMapping(evaluatedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    for(double i=0.6; i<10.0; i*=2) {
        IMappingSPtr imap = prob->createOptimizationMapping(evaluatedPoints);
        imap->defineDecisionVar(0, IElement(i));
    }
    init->defineInitialSet(evaluatedPoints);

    eval->evaluate();

    // Fit a Kriging model to the evaluated pionts
    ISet* eSet = init->outputSet(0);
    TVector<TVector<double> > xx;
    TVector<double> yy;
    for(int i=0; i<eSet->size(); i++) {
        // decision vector
        xx.push_back(eSet->at(i)->doubleDecisionVec());
        // function value
        yy.push_back(eSet->at(i)->doubleCost());
    }

    PowerVariogram* powerV = new PowerVariogram(xx, yy, 1.999);
    KrigingSPtr km = KrigingSPtr(new Kriging(powerV));
    km->estimateErrors(true);

    /// Use a new IPSet to evaluate a dense set of solutions on the Kriging
    /// model and to evaluate its expected improvement

    // Create query points
    TVector<double> xq;
    ISet* queryPoints = new ISet;

    for(double x=0.0; x<=10.0; x += 0.01) {
        xq.push_back(x);
        IMappingSPtr imap = prob->createOptimizationMapping(queryPoints);
        imap->defineDecisionVar(0, IElement(x));
    }

    // Create an IFunction from the Kriging model
    IFunctionSPtr func2 = IFunctionSPtr(new RandExpectedImprovement(km));
    //    ((RandExpectedImprovement*)func2.get())->TP_defineBandwith(
    //                ((RandExpectedImprovement*)func2.get())->TP_bandwith() / 10.0);

    // Expected improvement results
    PSetBase*                 base2 = new PSetBase();
    IFormulation*             prob2 = new IFormulation(base2);
    UserDefinedInit*          init2 = new UserDefinedInit(prob2);
    Evaluator*                eval2 = new Evaluator(init2);
    eval2->TP_defineSingleObjective(true);

    prob2->appendFunction(func2);
    init2->defineInitialSet(queryPoints);

    eval2->evaluate();

    // write the results to an .m file:
    QFile file("tst_randExpectedImprovement.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    ISet* expImp = init2->outputSet(0);

    out << "figure(4)\n"
           "clc;clear;clf;\n\n"

           "x = (0:0.01:10)';\n"
           "y = -sqrt(x).*sin(x);\n" << endl;

    out << "% Evaluated points:" << endl << "xx = [";
    for(int i=0; i<xx.size(); i++) {
        writeVector(out, xx[i]," ","; ");
    }
    out << "];" << endl << "yy = [";
    for(int i=0; i<yy.size(); i++) {
        out << yy[i] << "; ";
    }
    out << "];" << endl<< endl;

    out << "% Model estimation:" << endl << "xq = [";
    for(int i=0; i<xq.size(); i++) {
        out << xq[i] << "; ";
    }
    out << "];" << endl << "yq = [";
    for(int i=0; i<xq.size(); i++) {
        double yq = ((RandExpectedImprovement*)func2.get())->
                value(queryPoints->at(i)->decisionVec());
        out << yq << "; ";
    }
    out << "];" << endl << "err = [";
    for(int i=0; i<xq.size(); i++) {
        double err = ((RandExpectedImprovement*)func2.get())->
                error(queryPoints->at(i)->decisionVec());

        out << err << "; ";
    }
    out << "];" << endl << endl;

    out << "% Expected improvement:" << endl << "expImp = [";
    for(int i=0; i<xq.size(); i++) {
        double ei = expImp->at(i)->doubleCost();
        out << -ei << "; ";
    }
    out << "];" << endl;

    out << "% Plot the results\n"
           "realFunc = plot(x,y,'b'); hold on;\n"
           "TigonReal = plot(xx,yy,'ok','MarkerFaceColor','k');\n"
           "Model = plot(xq, yq, '--r', 'lineWidth', 2);\n"
           "Error = plot(xq, err, 'r');\n"
           "ExpectedImprovement = plot(xq, expImp*5, '-.g');\n\n"
           "legend([realFunc, TigonReal, Model, Error, ExpectedImprovement],...\n"
           "'Real function', 'Sample points', ...\n"
           "'Kriging estimation', 'Estimation error', "
           "'Expected improvement *5',...\n"
           "'location', 'northWest');\n"
           "grid on;\n"
           "title('Kernel density-based error');" << endl;

    delete eval2;
    delete init2;
    delete prob2;
    delete base2;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_SurrogateBasedOptimizer()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize   = 10;
    int   oVecSize   = 3;
    int   popSize    = 100;
    int   nDirs      = 30;
    double nRaduis    = 0.1;
    int   maxSurrogateSize = 20;

    PSetBase*                   base = new PSetBase();
    IFormulation*               prob = new IFormulation(base);
    SparegoInit*                init = new SparegoInit(prob);
    Evaluator*                  eval = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition*   scal = new GeneralizedDecomposition(dirs);
    ConstraintPenalty*          pnlt = new ConstraintPenalty(scal);
    SparegoValidation*          val  = new SparegoValidation(pnlt);
    RobustnessAssignment*       iVal = new RobustnessAssignment(val);
    DirectionFitnessFiltration* filt = new DirectionFitnessFiltration(iVal);
    SurrogateBasedOptimizerWithPerturbation* opt  =
            new SurrogateBasedOptimizerWithPerturbation(filt);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    dirs->TP_defineReferenceSetSize(nDirs);

    filt->TP_defineMaxSolutions(maxSurrogateSize);

    val->TP_defineNeighbourhoodRadius(nRaduis);
    opt->TP_defineNeighbourhoodRadius(nRaduis);

    iVal->defineIndicator(ConfidenceType, 0.9);

    opt->evaluate();

    delete opt;
    delete filt;
    delete iVal;
    delete val;
    delete pnlt;
    delete scal;
    delete dirs;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_sParEGO_workflow()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize = 5;
    int   oVecSize = 3;
    int   nDirs    = 30;
    int   popSize  = 50;
    double nRaduis  = 0.2;
    int   budget   = 80;
    int   maxSurrogateSize = 52;

    PSetBase*                   base = new PSetBase();
    IFormulation*               prob = new IFormulation(base);
    SparegoInit*                init = new SparegoInit(prob);
    Evaluator*                  eval = new Evaluator(init);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(eval);
    GeneralizedDecomposition*   scal = new GeneralizedDecomposition(dirs);
    ConstraintPenalty*          pnlt = new ConstraintPenalty(scal);
    SparegoValidation*          val  = new SparegoValidation(pnlt);
    RobustnessAssignment*       iVal = new RobustnessAssignment(val);
    DirectionFitnessFiltration* filt = new DirectionFitnessFiltration(iVal);
    SurrogateBasedOptimizerWithPerturbation* opt  =
            new SurrogateBasedOptimizerWithPerturbation(filt);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    dirs->TP_defineReferenceSetSize(nDirs);

    filt->TP_defineMaxSolutions(maxSurrogateSize);

    val->TP_defineNeighbourhoodRadius(nRaduis);

    eval->defineBudget(budget);
    opt->TP_defineNeighbourhoodRadius(nRaduis);

    iVal->defineIndicator(ConfidenceType, 0.9);


    while(opt->remainingBudget() > 0) {
        opt->evaluate();
        opt->incrementIteration();
    }

    delete opt;
    delete filt;
    delete iVal;
    delete val;
    delete pnlt;
    delete scal;
    delete dirs;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_sParEGO()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int   dVecSize = 5;
    int   oVecSize = 3;
    int   nDirs    = 20;
    int   popSize  = 10;
    double nRaduis  = 0.2;
    int   budget   = 14;
    int   maxSurrogateSize = 30;

    PSetBase*      base = new PSetBase();
    IFormulation*  prob = new IFormulation(base);
    SparegoInit*   init = new SparegoInit(prob);
    Evaluator*     eval = new Evaluator(init);
    sParEGO*       alg  = new sParEGO(eval);

    IFunctionSPtr func = IFunctionSPtr(new GECCO2016);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    init->TP_defineSetSize(popSize);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    alg->defineReferenceSetSize(nDirs);
    alg->TP_defineMaxSolutions(maxSurrogateSize);
    alg->TP_defineNeighbourhoodRadius(nRaduis);

    alg->defineIndicator(ConfidenceType, 0.9);

    alg->defineBudget(budget);

    while(alg->remainingBudget() > 0) {
        alg->evaluate();
        alg->incrementIteration();
    }

    delete alg;
    delete eval;
    delete init;
    delete prob;
    delete base;
}

void tst_sparego::test_LogDirectionVector()
{
    TRAND.defineSeed(1);
    cout.precision(5);

    int N = 10; // population size
    int nDirs = 6; // implies that the simplex lattice precision is H=2
    int budget = N + nDirs*2;
    // the maximum distance for neighbourhood is 0.2*sqrt(8) ~= 0.56569
    double nRaduis = 0.2;
    int maxSurrogateSize = 10;

    /// sParEGO optimization workflow
    PSetBase*          base = new PSetBase();
    IFormulation*      form = new IFormulation(base);
    SparegoInit*       init = new SparegoInit(form);
    Evaluator*         eval = new Evaluator(init);
    sParEGO*           alg  = new sParEGO(eval);

    ProblemSPtr prob = testProblem2();
    form->defineProblem(prob);

    init->TP_defineSetSize(N);
    init->TP_defineNeighbourhoodRadius(nRaduis);

    alg->defineReferenceSetSize(nDirs);
    alg->TP_defineMaxSolutions(maxSurrogateSize);
    alg->TP_defineNeighbourhoodRadius(nRaduis);

    alg->defineIndicator(ConfidenceType, 0.9);
    alg->defineBudget(budget);

    while(alg->remainingBudget() > 0) {
        alg->evaluate();
        alg->incrementIteration();
    }

    TVector<IMappingSPtr> solutions = alg->evaluatedMappings();

    TVector<TVector<double>> ref = {{1.0,0,0.0},{0.5,0.5,0},{0,1.0,0},
                                    {0,0.5,0.5},{0.5,0,0.5},{0,0,1.0}};
    TVector<bool> foundRef(ref.size(),false);

    for(auto sol : solutions) {

        TVector<double> dv = sol->weightingVec();

        for(int i=0; i<ref.size(); i++) {
            if( areVectorsEqual(ref[i],dv) ) {
                foundRef[i]=true;
                break;
            }
        }
    }

    // check that the new solutions contain all direction vectors
    auto res = std::find(foundRef.begin(),foundRef.end(),false);
    QCOMPARE(bool(res==foundRef.end()), true);

    delete alg;
    delete eval;
    delete init;
    delete form;
    delete base;
}

void tst_sparego::test_sParEGOWithMonteCarloValidation()
{
    TRAND.defineSeed(0);
    cout.precision(4);

    int    dVecSize = 3;
    int    oVecSize = 2;
    int    N        = 20;  // number of Monte Carlo evaluations
    int    popsize  = 8;   // population size generated by SparegoInit
    double nRaduis  = 0.2;
    int    nDirs    = 6;
    int    nIter    = 2;
    int    maxSurrogateSize = 100;
    double confidenceLevel = 0.9;

    int budget = 0;
    for(int i=1; i<=(nDirs*nIter); i++) {
        budget += N * (popsize + (i*2-2));
    }
    //  1. iteration: popsize*N
    //  2. iteration: popsize*N + 2*N
    //  3. iteration: popsize*N + 4*N
    //  4. iteration: popsize*N + 6*N
    // ...
    // 12. iteration: popsize*N + 22*N

    /// create the workflow
    PSetBase*          base = new PSetBase();
    IFormulation*      form = new IFormulation(base);
    SparegoInit*       init = new SparegoInit(form);
    SimplexLatticeDirectionIterator* dirs =
            new SimplexLatticeDirectionIterator(init);

    ValidationWithScalarisation* val = new ValidationWithScalarisation(dirs);
    RobustnessAssignment*       iVal = new RobustnessAssignment(val);
    DirectionFitnessFiltration* filt = new DirectionFitnessFiltration(iVal);
    SurrogateBasedOptimizerWithPerturbation* opt =
            new SurrogateBasedOptimizerWithPerturbation(filt);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    form->appendFunction(func);

    init->TP_defineSetSize(popsize);
    init->TP_defineNeighbourhoodRadius(nRaduis);
    init->addOutputTag(Tigon::TValidation);

    dirs->TP_defineReferenceSetSize(nDirs);

    filt->TP_defineMaxSolutions(maxSurrogateSize);

    val->TP_defineOperateOnFinal(false);
    val->TP_defineCountEvaluations(true);
    val->TP_defineScalarisingFunction(Tigon::WeightedChebyshev);
    val->TP_defineNEvaluations(N);
    val->addAdditionalOutputTag(Tigon::TFitness);

    opt->defineBudget(budget);
    opt->TP_defineNeighbourhoodRadius(nRaduis);
    opt->TP_defineErrorMethod(Tigon::ErrDensityBased);

    iVal->defineIndicator(ConfidenceType, confidenceLevel);

    while(opt->remainingBudget() > 0) {
        opt->evaluate();
        opt->incrementIteration();

        cout << "Iteration: " << opt->currentIteration()
             << ", Used budget " << opt->usedBudget()
             << ", Remaining budget " << opt->remainingBudget() << endl;
    }

    delete opt;
    delete filt;
    delete iVal;
    delete val;
    delete dirs;
    delete init;
    delete form;
    delete base;
}

QTEST_GUILESS_MAIN(tst_sparego)

#include "tst_sparego.moc"
