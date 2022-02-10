/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#include <QChar>
#include <QString>
#include <QSharedPointer>

#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>

#define TEST_LINEAR_CORRELATION
#define TEST_LINEAR_OBJECTIVE_REDUCTION
#ifdef CSDP_LIBRARY
  #define TEST_NONLINEAR_OBJECTIVE_REDUCTION
#endif

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_objectivereduction : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TEST_LINEAR_CORRELATION
    void test_Correlation_FixedInput1();
    void test_Correlation_FixedInput2();
    void test_Correlation_RandomInput();
#endif
#ifdef TEST_LINEAR_OBJECTIVE_REDUCTION
    void test_PCABasedObjectiveReduction();
    void test_LPCA_Operator();
#endif
#ifdef TEST_NONLINEAR_OBJECTIVE_REDUCTION
    void test_MVUPCABasedObjectiveReduction();
    void test_NLMVUPCA_Operator();
#endif
};

#ifdef TEST_LINEAR_CORRELATION
void tst_objectivereduction::test_Correlation_FixedInput1()
{
    int n = 4;
    int dimx = 2;

    TVector<TVector<double>> x(dimx);

    // Grades of 4 students
    //     s1   s2   s3   s4
    x[0] << 3 << 1 << 3 << 9; // Maths
    x[1] << 4 << 4 << 8 << 8; // Poetry

    CorrelationMatrix* corr = new CorrelationMatrix(x);
    QCOMPARE(corr->nSamples(),n);

    TMatrixReal cr = corr->correlationMatrix();
    Eigen::VectorXcd eivals = cr.eigenvalues();

    /// Checking that the correlation matrix is valid.
    /// It needs to be positive semidefinite, implying that
    /// it is a Hermitian matrix all of whose eigenvalues are nonnegative.
    for(int i=0; i<eivals.size(); i++) {
        QCOMPARE(eivals(i).real()>=0.0,true);
    }

    TVector<TVector<double>> cor_expect(dimx);
    cor_expect[0] << 1.0     << 2.0/3.0;
    cor_expect[1] << 2.0/3.0 << 1.0;

    QCOMPARE(cr(0,0),cor_expect[0][0]);
    QCOMPARE(cr(0,1),cor_expect[0][1]);
    QCOMPARE(cr(1,0),cor_expect[1][0]);
    QCOMPARE(cr(1,1),cor_expect[1][1]);

    delete corr;
}

void tst_objectivereduction::test_Correlation_FixedInput2()
{
    int n = 4;
    int dimx = 2;

    TMatrixReal x(dimx,n);
    x << 3, 1, 3, 9,
         4, 4, 8, 8;

    CorrelationMatrix* corr = new CorrelationMatrix(x);
    QCOMPARE(corr->nSamples(),n);

    TMatrixReal cr = corr->correlationMatrix();
    Eigen::VectorXcd eivals = cr.eigenvalues();

    /// Checking that the correlation matrix is valid.
    /// It needs to be positive semidefinite, implying that
    /// it is a Hermitian matrix all of whose eigenvalues are nonnegative.
    for(int i=0; i<eivals.size(); i++) {
        QCOMPARE(eivals(i).real()>=0.0,true);
    }

    TVector<TVector<double>> cor_expect(dimx);
    cor_expect[0] << 1.0     << 2.0/3.0;
    cor_expect[1] << 2.0/3.0 << 1.0;

    QCOMPARE(cr(0,0),cor_expect[0][0]);
    QCOMPARE(cr(0,1),cor_expect[0][1]);
    QCOMPARE(cr(1,0),cor_expect[1][0]);
    QCOMPARE(cr(1,1),cor_expect[1][1]);

    delete corr;
}

void tst_objectivereduction::test_Correlation_RandomInput()
{
    int n = 200;
    int dimx = 10;

    TVector<TVector<double>> x(dimx,TVector<double>(n));
    for(int i=0; i<dimx; i++) {
        for(int j=0; j<n; j++) {
            x[i][j] = TRAND.randUni();
        }
    }

    CorrelationMatrix* corr = new CorrelationMatrix(x);
    QCOMPARE(corr->nSamples(),n);

    TMatrixReal cr = corr->correlationMatrix();
    Eigen::VectorXcd eivals = cr.eigenvalues();

    cout << "Correlation Matrix" << endl;
    cout << cr << endl;

    /// Checking that the correlation matrix is valid.
    /// It needs to be positive semidefinite, implying that
    /// it is a Hermitian matrix all of whose eigenvalues are nonnegative.
    for(int i=0; i<eivals.size(); i++) {
        QCOMPARE(eivals(i).real()>=0.0,true);
    }

    delete corr;
}
#endif

#ifdef TEST_LINEAR_OBJECTIVE_REDUCTION
void tst_objectivereduction::test_PCABasedObjectiveReduction()
{
    TRAND.defineSeed(0);

    int I = 3;                // dimensionality of the POF
    int M = 5;                // number of objectives
    int ncon = M - I + 1;     // number of constraints
    int k = 2;                // number of decision variables in the g function
    int nFreeVariables = M-1; // number of free variables that describe the POF

    int dVecSize = nFreeVariables + k; // number of inputs
    int oVecSize = M+ncon;             // number of outputs
    int populationSize = 100;
    int maxIter = 40;

    PSetBase*         base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    ProblemSPtr prob = pg->problem();

    TString funcName = "Tigon::Representation::DTLZ5_" + std::to_string(I);
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    TVector<int> paramInd;
    TVector<int> cnstrInd;
    TVector<int> objInd;

    for(int i=0; i<M; i++) {
        objInd << i;
    }
    for(int i=0; i<ncon; i++) {
        cnstrInd << M+i;
    }

    //ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    // Define constraint threshold
    TVector<double> threshVec(ncon, 0.0);
    TVector<ElementType> types(ncon, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    RandomInit* init = new RandomInit(pg);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);

    NSGAII* alg = new NSGAII(evaluator);

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
    }

    // Elite population tags
    TStringList tags;
    tags << Tigon::TForNextIteration << Tigon::TForSelection;
    ISet* set = alg->setWithTags(tags);

    cout << "Objective values:" << endl;
    for(auto sol : set->all()) {
        for(auto obj : sol->doubleObjectiveVec()) {
            cout << obj << " ";
        }
        cout << endl;
    }

    PCABasedObjectiveReduction* objred = new PCABasedObjectiveReduction();
    objred->updateData(set);

    TVector<int> essentialSet = objred->essentialSet();
    cout << "Essential set = " << essentialSet << endl;
    QCOMPARE((int)essentialSet.size(), I);

    TVector<int> redundantSet = objred->redundantSet();
    cout << "Redundant set = " << redundantSet << endl;
    QCOMPARE((int)redundantSet.size(), M-I);

    // correlations between the objectives
    TMap<int, TVector<int>> corrs = objred->correlatedObjectiveSubSets();

    // objectives in {f1,...,fM-I+1} are non-conflicting among themselves,
    // hence, they are correlated
    for(int i=0; i<M-I+1; i++) {
        TVector<int> correlations = corrs.at(i);
        QCOMPARE((int)correlations.size(), M-I+1);
    }

    // objectives in {fM-I+2,...,fM} is in conflict with all the remaining
    // objectives, hence, they do not correlate with the other objectives
    for(int i=M-I+1; i<M; i++) {
        TVector<int> correlations = corrs.at(i);
        QCOMPARE((int)correlations.size(), 1);
    }

    // print the correlations
    TVector<int> objectives = getMapKeys(corrs);
    for(auto obj: objectives) {
        TVector<int> correlations = corrs.at(obj);
        cout << obj+1 << "th objective = " << correlations << endl;
    }

    // error incurred if an objective is to be removed
    TVector<double> error = objred->error();
    // print the error
    cout << "Error = " << error << endl;

    delete objred;
}

void tst_objectivereduction::test_LPCA_Operator()
{
    TRAND.defineSeed(0);

    int I = 3;                // dimensionality of the POF
    int M = 5;                // number of objectives
    int ncon = M - I + 1;     // number of constraints
    int k = 2;                // number of decision variables in the g function
    int nFreeVariables = M-1; // number of free variables that describe the POF

    int dVecSize = nFreeVariables + k; // number of inputs
    int oVecSize = M+ncon;             // number of outputs
    int populationSize = 100;
    int maxIter = 40;

    PSetBase*             base = new PSetBase();
    ProblemGenerator*       pg = new ProblemGenerator(base);
    RandomInit*           init = new RandomInit(pg);
    Evaluator*       evaluator = new Evaluator(init);
    NSGAII*                alg = new NSGAII(evaluator);
    LPCA*               objred = new LPCA(alg);
    ITermination*         tend = new ITermination(objred);

    ProblemSPtr prob = pg->problem();
    TString funcName = "Tigon::Representation::DTLZ5_" + std::to_string(I);
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    TVector<int> objInd;
    for(int i=0; i<M; i++) {
        objInd << i;
    }
    TVector<int> cnstrInd;
    for(int i=0; i<ncon; i++) {
        cnstrInd << M+i;
    }
    //ProblemSPtr prob(new Problem);
    prob->appendFunction(func, TVector<int>(), cnstrInd, objInd);
    prob->processProblemDefinition();
    // Define constraint threshold
    TVector<double> threshVec(ncon, 0.0);
    TVector<ElementType> types(ncon, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();
    pg->defineProblem(prob);

    init->TP_defineSetSize(2*populationSize);

    // Objective reduction operates on the elite population of NSGA-II
    objred->clearInputTags();
    objred->addInputTag(Tigon::TForNextIteration);
    objred->addInputTag(Tigon::TForSelection);
    objred->TP_defineOperateOnFinal(true);

    // Define termination criterion
    tend->TP_defineMaxIteration(maxIter);

    for(int iter=0; iter<=maxIter; iter++) {
        tend->evaluate();
        tend->incrementIteration();
    }

    TVector<int> essentialSet;
    TVector<int> redundantSet;
    int i=0;
    TVector<bool> objESet = objred->objectiveReductionData()
                                  ->essentialObjectiveSet();
    for(auto elem : objESet) {
        if(elem) essentialSet.push_back(i);
        else redundantSet.push_back(i);
        i++;
    }

    cout << "Essential set = " << essentialSet << endl;
    QCOMPARE((int)essentialSet.size(), I);

    cout << "Redundant set = " << redundantSet << endl;
    QCOMPARE((int)redundantSet.size(), M-I);

    // objectives in {f1,...,fM-I+1} are non-conflicting among themselves,
    // hence, they are correlated
    for(int i=0; i<M-I+1; i++) {
        TVector<CorrelatedObjectives> correlations =
                objred->objectiveReductionData()->correlatedObjectives()[i];
        QCOMPARE((int)correlations.size(), M-I+1);
    }

    // objectives in {fM-I+2,...,fM} is in conflict with all the remaining
    // objectives, hence, they do not correlate with the other objectives
    for(int i=M-I+1; i<M; i++) {
        TVector<CorrelatedObjectives> correlations =
                objred->objectiveReductionData()->correlatedObjectives()[i];
        QCOMPARE((int)correlations.size(), 1);
    }

    // print the correlations
    for(int i=0; i<M; i++) {
        TVector<CorrelatedObjectives> correlations =
                objred->objectiveReductionData()->correlatedObjectives()[i];
        cout << i+1 << "th objective = ";
        for(auto elem : correlations) {
            cout << elem.objectiveIdx+1 << "(" << elem.correlationStrength << ") ";
        }
        cout << endl;
    }

    // error incurred if an objective is to be removed
    TVector<double> error = objred->objectiveReductionData()->error();
    // print the error
    cout << "Error = " << error << endl;

    delete tend;
    delete objred;
    delete alg;
    delete evaluator;
    delete init;
    delete pg;
    delete base;
}

#endif

#ifdef TEST_NONLINEAR_OBJECTIVE_REDUCTION
void tst_objectivereduction::test_MVUPCABasedObjectiveReduction()
{
    TRAND.defineSeed(0);

    int I = 3;                // dimensionality of the POF
    int M = 5;                // number of objectives
    int ncon = M - I + 1;     // number of constraints
    int k = 2;                // number of decision variables in the g function
    int nFreeVariables = M-1; // number of free variables that describe the POF

    int dVecSize = nFreeVariables + k; // number of inputs
    int oVecSize = M+ncon;             // number of outputs
    int populationSize = 100;
    int maxIter = 40;

    PSetBase*         base = new PSetBase();
    ProblemGenerator* pg = new ProblemGenerator(base);
    ProblemSPtr prob = pg->problem();

    TString funcName = "Tigon::Representation::DTLZ5_" + std::to_string(I);
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);

    TVector<int> paramInd;
    TVector<int> cnstrInd;
    TVector<int> objInd;

    for(int i=0; i<M; i++) {
        objInd << i;
    }
    for(int i=0; i<ncon; i++) {
        cnstrInd << M+i;
    }

    //ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    // Define constraint threshold
    TVector<double> threshVec(ncon, 0.0);
    TVector<ElementType> types(ncon, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    pg->defineProblem(prob);

    RandomInit* init = new RandomInit(pg);
    init->TP_defineSetSize(2*populationSize);

    Evaluator* evaluator = new Evaluator(init);

    NSGAII* alg = new NSGAII(evaluator);

    for(int iter=0; iter<maxIter; iter++) {
        alg->evaluate();
        alg->incrementIteration();
    }

    // Elite population tags
    TStringList tags;
    tags << Tigon::TForNextIteration << Tigon::TForSelection;
    ISet* set = alg->setWithTags(tags);

    cout << "Objective values:" << endl;
    for(auto sol : set->all()) {
        for(auto obj : sol->doubleObjectiveVec()) {
            cout << obj << " ";
        }
        cout << endl;
    }

    MVUPCABasedObjectiveReduction* objred = new MVUPCABasedObjectiveReduction();
    objred->defineStandardize(false);
    objred->updateData(set);

    TVector<int> essentialSet = objred->essentialSet();
    cout << "Essential set = " << essentialSet << endl;
    QCOMPARE((int)essentialSet.size(), I);

    TVector<int> redundantSet = objred->redundantSet();
    cout << "Redundant set = " << redundantSet << endl;
    QCOMPARE((int)redundantSet.size(), M-I);

    // correlations between the objectives
    TMap<int, TVector<int>> corrs = objred->correlatedObjectiveSubSets();

    // objectives in {f1,...,fM-I+1} are non-conflicting among themselves,
    // hence, they are correlated
    for(int i=0; i<M-I+1; i++) {
        TVector<int> correlations = corrs.at(i);
        QCOMPARE((int)correlations.size(), M-I+1);
    }

    // objectives in {fM-I+2,...,fM} is in conflict with all the remaining
    // objectives, hence, they do not correlate with the other objectives
    for(int i=M-I+1; i<M; i++) {
        TVector<int> correlations = corrs.at(i);
        QCOMPARE((int)correlations.size(), 1);
    }

    // print the correlations
    TVector<int> objectives = getMapKeys(corrs);
    for(auto obj: objectives) {
        TVector<int> correlations = corrs.at(obj);
        cout << obj+1 << "th objective = " << correlations << endl;
    }

    // error incurred if an objective is to be removed
    TVector<double> error = objred->error();
    // print the error
    cout << "Error = " << error << endl;

    delete objred;
}

void tst_objectivereduction::test_NLMVUPCA_Operator()
{
    TRAND.defineSeed(0);

    int I = 3;                // dimensionality of the POF
    int M = 5;                // number of objectives
    int ncon = M - I + 1;     // number of constraints
    int k = 2;                // number of decision variables in the g function
    int nFreeVariables = M-1; // number of free variables that describe the POF

    int dVecSize = nFreeVariables + k; // number of inputs
    int oVecSize = M+ncon;             // number of outputs
    int populationSize = 100;
    int maxIter = 40;

    PSetBase*             base = new PSetBase();
    ProblemGenerator*       pg = new ProblemGenerator(base);
    RandomInit*           init = new RandomInit(pg);
    Evaluator*       evaluator = new Evaluator(init);
    NSGAII*                alg = new NSGAII(evaluator);
    NLMVUPCA*           objred = new NLMVUPCA(alg);
    ITermination*         tend = new ITermination(objred);

    ProblemSPtr prob = pg->problem();
    TString funcName = "Tigon::Representation::DTLZ5_" + std::to_string(I);
    IFunctionSPtr func = pg->createFunction(FuncTypeStrInternal, funcName);

    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    TVector<int> objInd;
    for(int i=0; i<M; i++) {
        objInd << i;
    }
    TVector<int> cnstrInd;
    for(int i=0; i<ncon; i++) {
        cnstrInd << M+i;
    }
    //ProblemSPtr prob(new Problem);
    prob->appendFunction(func, TVector<int>(), cnstrInd, objInd);
    prob->processProblemDefinition();
    // Define constraint threshold
    TVector<double> threshVec(ncon, 0.0);
    TVector<ElementType> types(ncon, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();
    pg->defineProblem(prob);

    init->TP_defineSetSize(2*populationSize);

    // Objective reduction operates on the elite population of NSGA-II
    objred->clearInputTags();
    objred->addInputTag(Tigon::TForNextIteration);
    objred->addInputTag(Tigon::TForSelection);
    objred->TP_defineOperateOnFinal(true);
    objred->TP_defineStandardize(false);

    // Define termination criterion
    tend->TP_defineMaxIteration(maxIter);

    for(int iter=0; iter<=maxIter; iter++) {
        tend->evaluate();
        tend->incrementIteration();
    }

    TVector<int> essentialSet;
    TVector<int> redundantSet;
    int i=0;
    TVector<bool> objESet = objred->objectiveReductionData()
                                  ->essentialObjectiveSet();
    for(auto elem : objESet) {
        if(elem) essentialSet.push_back(i);
        else redundantSet.push_back(i);
        i++;
    }

    cout << "Essential set = " << essentialSet << endl;
    QCOMPARE((int)essentialSet.size(), I);

    cout << "Redundant set = " << redundantSet << endl;
    QCOMPARE((int)redundantSet.size(), M-I);

    // objectives in {f1,...,fM-I+1} are non-conflicting among themselves,
    // hence, they are correlated
    for(int i=0; i<M-I+1; i++) {
        TVector<CorrelatedObjectives> correlations =
                objred->objectiveReductionData()->correlatedObjectives()[i];
        QCOMPARE((int)correlations.size(), M-I+1);
    }

    // objectives in {fM-I+2,...,fM} is in conflict with all the remaining
    // objectives, hence, they do not correlate with the other objectives
    for(int i=M-I+1; i<M; i++) {
        TVector<CorrelatedObjectives> correlations =
                objred->objectiveReductionData()->correlatedObjectives()[i];
        QCOMPARE((int)correlations.size(), 1);
    }

    // print the correlations
    for(int i=0; i<M; i++) {
        TVector<CorrelatedObjectives> correlations =
                objred->objectiveReductionData()->correlatedObjectives()[i];
        cout << i+1 << "th objective = ";
        for(auto elem : correlations) {
            cout << elem.objectiveIdx+1 << "(" << elem.correlationStrength << ") ";
        }
        cout << endl;
    }

    // error incurred if an objective is to be removed
    TVector<double> error = objred->objectiveReductionData()->error();
    // print the error
    cout << "Error = " << error << endl;

    delete tend;
    delete objred;
    delete alg;
    delete evaluator;
    delete init;
    delete pg;
    delete base;
}

#endif

QTEST_GUILESS_MAIN(tst_objectivereduction)

#include "tst_objectivereduction.moc"
