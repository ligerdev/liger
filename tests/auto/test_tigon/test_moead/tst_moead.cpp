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
#include <QDir>
#include <QChar>

#include <QSharedPointer>

#include <iostream>
#include <iomanip>

// Boost includes
#include <boost/math/special_functions/factorials.hpp>
using namespace boost::math;

#include <tigon/Tigon.h>
#include <tigon/Utils/TigonUtils.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

// Uncomment to add operator tests
#define TST_MOEAD_OPERATORS
#define TST_GOALS
#define WORK_FLOW

//TESTED_COMPONENT=src/libs/tigon/....
class tst_moead : public QObject
{
    Q_OBJECT

public:

private slots:

#ifdef TST_MOEAD_OPERATORS
    /* Add the necessary operators */
    void test_WeightVectorInit();
    void test_WeightVectorInitCentroidDistanceOrder();
    void test_Scalarization();
    void test_MOEADNeighbourhoodFiltration();
    void test_MOEADRandSetReplacement();
    void test_MOEADResizeSets();
    void test_RandFiltrationForDirection();
    void test_MOEADNeighbourhoodUpdate();
#endif

// Uncomment to add workflow tests
#ifdef WORK_FLOW
    void test_MOEAD_workflow();
    void test_MOEAD();
#endif

#ifdef TST_GOALS
    void test_goalBelowSimplexLatticeObjectiveReduction();
    void test_goalAboveSimplexLatticeObjectiveReduction();
    void test_goalBelowSimplexLattice();
    void test_goalAboveSimplexLattice();
    void test_goalMaximization();
    void test_goalVectorsNotSet();
#endif

};

#ifdef TST_MOEAD_OPERATORS

void tst_moead::test_WeightVectorInit()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int M = oVecSize;
    int H = 10;
    int N = 200;

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(N);

    Evaluator* evaluator = new Evaluator(init);

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);
    wvector->TP_defineRegularisationMethod(Tigon::InitialOrder);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);
    int N_expected = simplexLatticeSize(H,M);
    int N_obtained = oSet->size();

    // check the number of weight vectors
    QCOMPARE(N_expected,N_obtained);

    TVector<TVector<double>> simplex = simplexLatticeIterative(H, M);

    for(int i=0; i<N_obtained; i++) {
        TVector <double> vec = oSet->at(i)->weightingVec();
        double vecSum = std::accumulate(vec.begin(), vec.end(), 0.0);
        // check if the sum of each weight vector adds up to 1
        QCOMPARE(vecSum,1.0);

        for(int j=0; j<vec.size(); j++) {
            // check if the weight vectors are in their initial order
            QCOMPARE(vec[j],simplex[i][j]);
        }
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_WeightVectorInitCentroidDistanceOrder()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int H = 10;
    int N = 200;

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(N);

    Evaluator* evaluator = new Evaluator(init);

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);
    wvector->TP_defineRegularisationMethod(Tigon::CentroidBasedOrder);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector<TVector<double>> simplexObt;
    simplexObt.reserve(oSet->size());
    for(auto solution : oSet->all()) {
        simplexObt.push_back(solution->weightingVec());
    }

    /// 1. Find the centroid of the weight vectors
    TVector<double> centroid = sumColumnsDoubleVector(simplexObt);
    std::transform(centroid.begin(), centroid.end(),
                   centroid.begin(),
                   std::bind1st(std::multiplies<double>(),
                                1.0/simplexObt.size()));

    /// 2. Determine the distance between all weight vectors and the centroid
    TVector<double> distance;
    distance.reserve(simplexObt.size());
    for(auto row: simplexObt) {
        double dist = angleDistance(centroid, row);
        distance.push_back(dist);
    }
    centroid.clear();

    /// 3. Sorted indices of the weight vectors based on their distance to the
    /// centroid
    TVector<int> sortedInd = indSort(distance);
    distance.clear();

    // Consider the solution (sol1) which is associated with the weight vector
    // that is closer to the centroid, let it be wv. Consider also the
    // solution (sol2) which is associated to the weight vector that is further
    // away from the centroid. Then:
    // => The distance between sol1 and wv is smaller than or equal to the
    // distance between sol2 and wv.

    int first = sortedInd[0];
    int last  = sortedInd[sortedInd.size()-1];

    // weight vector closer to the centroid
    TVector<double> wv = simplexObt[first];

    TVector<double> sol1 = oSet->at(first)->doubleObjectiveVec();
    TVector<double> sol2  = oSet->at(last)->doubleObjectiveVec();

    double d1First = angleDistance(wv,sol1);
    double d2First = angleDistance(wv,sol2);

    QCOMPARE(bool(d1First<=d2First),true);

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_Scalarization() {

    int dVecSize = 7;
    int oVecSize = 3;
    int H = 10;
    int populationSize = 63;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    Tigon::ScalarisationType scalarisationFunction = Tigon::WeightedChebyshev;
    Scalarization* scalarization = new Scalarization(wvector);
    scalarization->TP_defineScalarisingFunction(scalarisationFunction);
    scalarization->TP_defineWeightScopeApproach(Tigon::LocalWeight);
    scalarization->defineOutputTags(TStringList({Tigon::TMainOptimizationSet}));
    scalarization->addAdditionalOutputTag(TString(Tigon::TFitness));

    scalarization->evaluate();

    TVector<double> ideal = IElementVecToRealVec(scalarization->idealVec());
    TVector<double> antiIdeal = IElementVecToRealVec(scalarization->antiIdealVec());

    cout << ideal;
    cout << antiIdeal;

    // display the results
    ISet* oSet = scalarization->outputSet(0);

    int popsize = oSet->size();
    cout << "population size: " << popsize+1 << endl << endl;
    TVector <double> vec;
    cout << "Weight vectors"<< "\t\t" << "Objective vectors"<< "\t\t" << "Scalar value" << endl;
    for(int i=0; i<popsize; i++) {
        vec = oSet->at(i)->weightingVec();
        cout << vec;
        TVector<double> oVec = oSet->at(i)->doubleObjectiveVec();
        cout << oVec;
        cout << oSet->at(i)->doubleCost() << endl;
    }

    delete scalarization;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_MOEADNeighbourhoodFiltration()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int H = 10;
    int populationSize = 63;
    int neighbourhoodSize = 10;

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    NeighbourhoodFiltration* neighbourhood = new NeighbourhoodFiltration(wvector);
    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineNeighbourhoodSize(neighbourhoodSize);
    neighbourhood->TP_defineIsClearOutputSets(false);

    neighbourhood->evaluate();

    // display the results
    //int n_sets = neighbourhood->outputSets().size();
    //for(int i=0; i<n_sets; i++) {
    for(int i=0; i<5; i++) {
        ISet* oSet = neighbourhood->outputSet(i);
        int neighbourhoodsize = oSet->size();
        cout << "neighbourhood " << i+1 << ":" << endl;
        TVector <double> vec;
        for(int j=0; j<neighbourhoodsize; j++) {
            vec = oSet->at(j)->weightingVec();
            int vecSize = vec.size();
            cout << "\t";
            for(int h=0; h<vecSize; h++) {
                double value = vec[h];
                cout << value << " ";
            }
            cout << endl;
        }
    }

    delete neighbourhood;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_MOEADRandSetReplacement()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int H = 10;
    int populationSize = 63;
    int neighbourhoodSize = 10;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    NeighbourhoodFiltration* neighbourhood = new NeighbourhoodFiltration(wvector);
    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineNeighbourhoodSize(neighbourhoodSize);
    neighbourhood->TP_defineIsClearOutputSets(false);
    neighbourhood->addOutputTag(Tigon::TForSetReplacement); // activates RandSetReplacement

    RandSetReplacement* randreplacement = new RandSetReplacement(neighbourhood);
    randreplacement->defineReplacementSetTags(TStringList({Tigon::TMainOptimizationSet}));
    randreplacement->defineReplacementProbability(0.1);

    randreplacement->evaluate();

    // display the results
    for(int i=0; i<30; i++) {
        ISet* oSet = randreplacement->outputSet(i);
        int neighbourhoodsize = oSet->size();
        cout << "neighbourhood " << i+1 << "(" << neighbourhoodsize << "):" << endl;
        TVector <double> vec;
        for(int j=0; j<neighbourhoodsize; j++) {
            vec = oSet->at(j)->weightingVec();
            int vecSize = vec.size();
            cout << "\t";
            for(int h=0; h<vecSize; h++) {
                double value = vec[h];
                cout << value << " ";
            }
            cout << endl;
        }
    }

    delete randreplacement;
    delete neighbourhood;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_MOEADResizeSets()
{
    int dVecSize = 7;
    int oVecSize = 3;
    int H = 10;
    int populationSize = 63;
    int neighbourhoodSize = 10;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    Evaluator* evaluator = new Evaluator(init);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    NeighbourhoodFiltration* neighbourhood = new NeighbourhoodFiltration(wvector);
    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineNeighbourhoodSize(neighbourhoodSize);
    neighbourhood->TP_defineIsClearOutputSets(false);
    neighbourhood->addOutputTag(Tigon::TForResize); // activates TruncateSets

    TruncateSets* resizesets = new TruncateSets(neighbourhood);
    resizesets->TP_defineSetSize(3);

    resizesets->evaluate();

    // display the results
    //int n_sets = shuffled->outputSets().size();
    //for(int i=0; i<n_sets; i++) {
    for(int i=0; i<5; i++) {
        ISet* oSet = resizesets->outputSet(i);
        int neighbourhoodsize = oSet->size();
        cout << "new neighbourhood size" << i+1 << ":"
             << neighbourhoodsize << endl;
    }

    delete resizesets;
    delete neighbourhood;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_RandFiltrationForDirection()
{
    int iter;
    int dVecSize = 7;
    int oVecSize = 3;
    int H = 14;
    int M = oVecSize;
    int populationSize = 100;
    int maxIter = 2;
    int neighbourhoodSize = 20;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);

    Evaluator* evaluator = new Evaluator(init);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    Tigon::ScalarisationType scalarisationFunction = Tigon::WeightedChebyshev;
    Scalarization* scalarization = new Scalarization(wvector);
    scalarization->TP_defineScalarisingFunction(scalarisationFunction);
    scalarization->TP_defineWeightScopeApproach(Tigon::LocalWeight);
    scalarization->defineOutputTags(TStringList({Tigon::TMainOptimizationSet}));
    scalarization->addAdditionalOutputTag(Tigon::TFitness);

    NeighbourhoodFiltration* neighbourhood = new NeighbourhoodFiltration(scalarization);
    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineNeighbourhoodSize(neighbourhoodSize);
    neighbourhood->TP_defineIsClearOutputSets(false);
    neighbourhood->addOutputTag(Tigon::TForSetReplacement);

    MOEADNeighbourhoodUpdate* updateOperator = new MOEADNeighbourhoodUpdate(neighbourhood);
    updateOperator->TP_defineScalarisingFunction(scalarisationFunction);
    updateOperator->TP_defineIsGeneralisedDecomposition(false);

    RandSetReplacement* randreplacement = new RandSetReplacement(updateOperator);
    randreplacement->defineReplacementSetTags(TStringList({Tigon::TMainOptimizationSet}));
    randreplacement->addOutputTag(Tigon::TForSelection);

    RandFiltrationForDirection* filtForDirection = new RandFiltrationForDirection(randreplacement);
    filtForDirection->TP_defineInputSetSize(2);
    filtForDirection->TP_defineOutputSetSize(2);
    filtForDirection->addOutputTag(Tigon::TForPerturbation);
    filtForDirection->addOutputTag(Tigon::TForResize);
    filtForDirection->addOutputTag(Tigon::TForEvaluation);
    filtForDirection->addOutputTag(Tigon::TForMOEADUpdate);
    updateOperator->defineInputTags(TStringList({Tigon::TForMOEADUpdate}));
    updateOperator->defineOutputTags(filtForDirection->inputTags());

    SBXCrossOver* crossOver = new SBXCrossOver(filtForDirection);

    TruncateSets* resizeSets = new TruncateSets(crossOver);
    resizeSets->TP_defineSetSize(1);

    PolynomialMutation* pm = new PolynomialMutation(resizeSets);

    int N_new = factorial<double>(H+M-1) /
            (factorial<double>(H) * factorial<double>(M-1));

    // Display the results
    for(iter=0;iter<maxIter;iter++) {
        filtForDirection->evaluate();

        cout << "Iteration{" << filtForDirection->currentIteration() << "} = [" << endl;

        if(iter>0) {

            TVector <ISet*> vectorInputSets = filtForDirection->inputSets();
            TVector <ISet*> vectorOutputSets = filtForDirection->outputSets();

            int vectorInputSetsSize = vectorInputSets.size();
            int vectorOutputSetsSize = vectorOutputSets.size();

            // the number of input and outsets must be equal to the population size
            QCOMPARE(vectorInputSetsSize, N_new);
            QCOMPARE(vectorOutputSetsSize, N_new);

            // iSets
            for(int i=0; i<vectorInputSetsSize; i++) {
                ISet* theInputSet = filtForDirection->inputSet(i);
                //ISet* theInputSet = vectorInputSets[i];
                int inputSetSize = theInputSet->size();

                // the number of solutions per input set is equal to:
                // 1. the number of neighbours plus one solution, or
                // 2. the population size
                bool cond1 = inputSetSize==neighbourhoodSize+1;
                bool cond2 = inputSetSize==N_new;

                QCOMPARE( cond1 || cond2 ,true);
                for(int sol=0; sol<inputSetSize; sol++) {
                    // all solutions in the neighbourhood have to be scalarised
                    QCOMPARE(theInputSet->at(sol)->isScalarised(), true);
                }
            }

            // oSets
            for(int i=0; i<vectorOutputSetsSize; i++) {
                ISet* theOutputSet = filtForDirection->outputSet(i);
                //ISet* theOutputSet = vectorOutputSets[i];
                int outputSetSize = theOutputSet->size();

                // two childs must exist per output set
                QCOMPARE(outputSetSize,2);
            }
        }

        crossOver->evaluateOnlyThisNode();
        resizeSets->evaluateOnlyThisNode();
        pm->evaluateOnlyThisNode();
        pm->incrementIteration();
    }


    delete pm;
    delete resizeSets;
    delete crossOver;
    delete filtForDirection;
    delete randreplacement;
    delete updateOperator;
    delete neighbourhood;
    delete scalarization;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_MOEADNeighbourhoodUpdate()
{
    int iter;
    int dVecSize = 7;
    int oVecSize = 3;
    int H = 14;
    int M = oVecSize;
    int populationSize = 100;
    int maxIter = 2;
    int neighbourhoodSize = 20;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);

    Evaluator* evaluator = new Evaluator(init);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    Tigon::ScalarisationType scalarisationFunction = Tigon::WeightedChebyshev;
    Scalarization* scalarization = new Scalarization(wvector);
    scalarization->TP_defineScalarisingFunction(scalarisationFunction);
    scalarization->TP_defineWeightScopeApproach(Tigon::LocalWeight);
    scalarization->defineOutputTags(TStringList({Tigon::TMainOptimizationSet}));
    scalarization->addAdditionalOutputTag(Tigon::TFitness);

    NeighbourhoodFiltration* neighbourhood = new NeighbourhoodFiltration(scalarization);
    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineNeighbourhoodSize(neighbourhoodSize);
    neighbourhood->TP_defineIsClearOutputSets(false);
    neighbourhood->addOutputTag(Tigon::TForSetReplacement);

    MOEADNeighbourhoodUpdate* updateOperator = new MOEADNeighbourhoodUpdate(neighbourhood);
    updateOperator->TP_defineScalarisingFunction(scalarisationFunction);
    updateOperator->TP_defineIsGeneralisedDecomposition(false);
    updateOperator->TP_defineMaxNumberReplacements(2);

    RandSetReplacement* randreplacement = new RandSetReplacement(updateOperator);
    randreplacement->defineReplacementSetTags(TStringList({Tigon::TMainOptimizationSet}));
    randreplacement->defineReplacementProbability(0.1);
    randreplacement->addOutputTag(Tigon::TForSelection);

    RandFiltrationForDirection* filtForDirection = new RandFiltrationForDirection(randreplacement);
    filtForDirection->TP_defineInputSetSize(2);
    filtForDirection->addOutputTag(Tigon::TForPerturbation);
    filtForDirection->addOutputTag(Tigon::TForResize);
    filtForDirection->addOutputTag(Tigon::TForEvaluation);
    filtForDirection->addOutputTag(Tigon::TForMOEADUpdate);
    updateOperator->defineInputTags(TStringList({Tigon::TForMOEADUpdate}));
    updateOperator->defineOutputTags(filtForDirection->inputTags());

    SBXCrossOver* crossOver = new SBXCrossOver(filtForDirection);

    TruncateSets* resizeSets = new TruncateSets(crossOver);
    resizeSets->TP_defineSetSize(1);

    PolynomialMutation* pm = new PolynomialMutation(resizeSets);

    // Display the results
    for(iter=0;iter<maxIter;iter++) {
        updateOperator->evaluate();

        cout << "Iteration{" << updateOperator->currentIteration() << "} = [" << endl;

        if(iter>0) {

            TVector <ISet*> vectorInputSets = updateOperator->inputSets();
            TVector <ISet*> vectorOutputSets = updateOperator->outputSets();

            int vectorInputSetsSize = vectorInputSets.size();
            int vectorOutputSetsSize = vectorOutputSets.size();

            QCOMPARE(vectorInputSetsSize>0, true);
            QCOMPARE(vectorOutputSetsSize>0, true);

            // iSets
            for(int i=0; i<vectorInputSetsSize; i++) {
                ISet* theInputSet = updateOperator->inputSet(i);
                //ISet* theInputSet = vectorInputSets[i];
                int inputSetSize = theInputSet->size();
                // only one child should exist per input set
                QCOMPARE(inputSetSize,1);
                // the child has to be evaluated
                QCOMPARE(theInputSet->at(0)->isEvaluated(),true);
                //QCOMPARE(theInputSet->at(0)->isScalarised(),false);
            }

            // oSets
            for(int i=0; i<vectorOutputSetsSize; i++) {
                ISet* theOutputSet = updateOperator->outputSet(i);
                //ISet* theOutputSet = vectorOutputSets[i];
                int outputSetSize = theOutputSet->size();
                // the number of solutions per output set is equal to:
                // 1. the number of neighbours plus one solution, or
                // 2. the population size
                int N_new = factorial<double>(H+M-1) /
                        (factorial<double>(H) * factorial<double>(M-1));
                bool cond1 = outputSetSize==neighbourhoodSize+1;
                bool cond2 = outputSetSize==N_new;

                QCOMPARE( cond1 || cond2 ,true);
                for(int sol=0; sol<outputSetSize; sol++) {
                    // all solutions in the neighbourhood have to be scalarised
                    QCOMPARE(theOutputSet->at(sol)->isScalarised(), true);
                }
            }
        }

        randreplacement->evaluateOnlyThisNode();
        filtForDirection->evaluateOnlyThisNode();
        crossOver->evaluateOnlyThisNode();
        resizeSets->evaluateOnlyThisNode();
        pm->evaluateOnlyThisNode();
        pm->incrementIteration();
    }


    delete pm;
    delete resizeSets;
    delete crossOver;
    delete filtForDirection;
    delete randreplacement;
    delete updateOperator;
    delete neighbourhood;
    delete scalarization;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}
#endif

#ifdef WORK_FLOW
void tst_moead::test_MOEAD_workflow()
{
    int iter;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 290;
    int maxIter = 2;
    int H=14;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);

    Evaluator* evaluator = new Evaluator(init);
    //evaluator->TP_defineParallel(false);

    // MOEA/D starts

    WeightVectorInit* wvector = new WeightVectorInit(evaluator);
    wvector->TP_defineNumberPointsPerDimension(H);

    Tigon::ScalarisationType scalarisationFunction = Tigon::WeightedChebyshev;
    Scalarization* scalarization = new Scalarization(wvector);
    scalarization->TP_defineScalarisingFunction(scalarisationFunction);
    scalarization->TP_defineWeightScopeApproach(Tigon::LocalWeight);
    scalarization->defineOutputTags(TStringList({Tigon::TMainOptimizationSet}));
    scalarization->addAdditionalOutputTag(Tigon::TFitness);

    NeighbourhoodFiltration* neighbourhood = new NeighbourhoodFiltration(scalarization);
    neighbourhood->TP_defineNeighbourhoodCriterion(Tigon::NeighbourhoodSize);
    neighbourhood->TP_defineIsClearOutputSets(false);
    neighbourhood->addOutputTag(Tigon::TForSetReplacement);

    MOEADNeighbourhoodUpdate* updateOperator = new MOEADNeighbourhoodUpdate(neighbourhood);
    updateOperator->TP_defineScalarisingFunction(scalarisationFunction);
    updateOperator->TP_defineIsGeneralisedDecomposition(false);

    RandSetReplacement* randreplacement = new RandSetReplacement(updateOperator);
    randreplacement->defineReplacementSetTags(TStringList({Tigon::TMainOptimizationSet}));
    randreplacement->addOutputTag(Tigon::TForSelection);

    RandFiltrationForDirection* filtForDirection = new RandFiltrationForDirection(randreplacement);
    filtForDirection->TP_defineInputSetSize(2);
    filtForDirection->addOutputTag(Tigon::TForPerturbation);
    filtForDirection->addOutputTag(Tigon::TForResize);
    filtForDirection->addOutputTag(Tigon::TForEvaluation);
    filtForDirection->addOutputTag(Tigon::TForMOEADUpdate);
    updateOperator->defineInputTags(TStringList({Tigon::TForMOEADUpdate}));
    updateOperator->defineOutputTags(filtForDirection->inputTags());

    SBXCrossOver* crossOver = new SBXCrossOver(filtForDirection);

    TruncateSets* resizeSets = new TruncateSets(crossOver);
    resizeSets->TP_defineSetSize(1);

    PolynomialMutation* pm = new PolynomialMutation(resizeSets);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    // Display the results
    for(iter=0;iter<maxIter;iter++) {
        pm->evaluate();
        pm->incrementIteration();

        cout << "Iteration{" << pm->currentIteration() << "} = [" << endl;
        ISet* pop = pm->setWithTags(tags);
        //ISet* pop = init->outputSet(0);
        int popsize=pop->size();
        for(int i=0; i<popsize; i++) {
            TVector <double> vec = pop->at(i)->doubleObjectiveVec();
            for(size_t var=0; var<vec.size(); var++) {
                cout << vec[var] << " ";
            }
             cout << "; ";
        }
        cout << "];" << endl;
    }

    delete pm;
    delete resizeSets;
    delete crossOver;
    delete filtForDirection;
    delete randreplacement;
    delete updateOperator;
    delete neighbourhood;
    delete scalarization;
    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_MOEAD()
{
    int iter;
    //int dVecSize = 7;
    int dVecSize = 12;
    int oVecSize = 3;
    int populationSize = 100;
    int maxIter = 5;

    cout.precision(5);
    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);
    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->addOutputTag(Tigon::TFitness);

    Evaluator* evaluator = new Evaluator(init);
    //evaluator->TP_defineParallel(false);

    MOEAD* alg = new MOEAD(evaluator);

    TStringList tags;
    tags << Tigon::TMainOptimizationSet;

    // Display the results
    for(iter=0;iter<maxIter;iter++) {
        alg->evaluate();
        alg->incrementIteration();

        if(iter==(maxIter-1)) {
            cout << endl;
            cout << "Iteration{" << alg->currentIteration() << "} = [" << endl;
            ISet* pop = alg->setWithTags(tags);
            for(int i=0; i< pop->size(); i++) {
                TVector <double> vec = pop->at(i)->doubleObjectiveVec();
                for(size_t var=0; var<vec.size(); var++) {
                    cout << vec[var] << " ";
                }
                cout << "; ";
            }
            cout << "];" << endl;
        }
    }

    delete alg;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}
#endif

#ifdef TST_GOALS

void tst_moead::test_goalBelowSimplexLatticeObjectiveReduction() {

    int M=4;
    int populationSize = 210;
    int dVecSize=12;
    int oVecSize=M;

    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    Tigon::ProblemDefinitionStatus status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    prob->problem()->redefineObjOptimizationType(0,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(1,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(2,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(4,OptimizationType::Minimization);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    RandomInit*           init      = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    WeightVectorInit* wvector = new WeightVectorInit(evaluator);

    TVector <double> goals;
    goals << 0.2 << 0.2 << 0.2 << 0.2;
    TVector<ElementType> types(M, RealType);

    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    /// last objective is redundant
    TVector<bool> essentialObjs;
    essentialObjs << true << true << true << false;
    prob->problem()->defineEssentialObjectives(essentialObjs);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector <double> normGoalVector(M);
    for(int i=0;i<M;i++) {
        normGoalVector[i]=goals[i];
    }

    TVector<double> ideal = IElementVecToRealVec(wvector->idealVec());
    TVector<double> nadir = IElementVecToRealVec(wvector->nadirVec());
    normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

    // calculate the goal vector sum based on the components that have been set
    double goalVecSum=0.0;
    for(int i=0; i<M; i++) {
        if(!areDoublesEqual(goals[i],Tigon::Lowest) ) {
            goalVecSum += normGoalVector[i];
        }
    }

    // modify the goal vector based on goalVecSum
    for(int i=0; i<M; i++) {
        if(areDoublesEqual(goals[i],Tigon::Lowest)) {
            if(goalVecSum>1.0) {
                normGoalVector[i] = 1.0;
            }
            else {
                normGoalVector[i] = 0.0;
            }
        }
    }

    int N = oSet->size();
    TVector <double> wvec;
    cout << "weights: " << endl;
    for(int i=0; i<N; i++) {
        wvec = oSet->at(i)->weightingVec();

        if(goalVecSum>1.0) {
            /// check if the goal vector does not weakly dominate the weight vector
            tribool res = Tigon::weakDominance(normGoalVector,wvec);
            QCOMPARE(res.value==tribool::true_value, false);
        }
        else {
            /// check if the weight vector does not weakly dominate the goal vector
            tribool res = Tigon::weakDominance(wvec,normGoalVector);
            QCOMPARE(res.value==tribool::true_value, false);
        }

        //cout << "weights[" << i+1 << "]: ";
        int vecSize = wvec.size();
        for(int j=0; j<vecSize; j++) {
            double value = wvec[j];
            cout << value << " ";
        }
        cout << endl;
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_goalAboveSimplexLatticeObjectiveReduction() {

    int M=4;
    int populationSize = 210;
    int dVecSize=12;
    int oVecSize=M;

    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    Tigon::ProblemDefinitionStatus status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    prob->problem()->redefineObjOptimizationType(0,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(1,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(2,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(4,OptimizationType::Minimization);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    RandomInit*           init      = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    WeightVectorInit* wvector = new WeightVectorInit(evaluator);

    TVector <double> goals;
    goals << 0.3 << 0.3 << 0.3 << 0.3;
    TVector<ElementType> types(M, RealType);

    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    /// last objective is redundant
    TVector<bool> essentialObjs;
    essentialObjs << true << true << true << false;
    prob->problem()->defineEssentialObjectives(essentialObjs);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector <double> normGoalVector(M);
    for(int i=0;i<M;i++) {
        normGoalVector[i]=goals[i];
    }

    TVector<double> ideal = IElementVecToRealVec(wvector->idealVec());
    TVector<double> nadir = IElementVecToRealVec(wvector->nadirVec());
    normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

    // calculate the goal vector sum based on the components that have been set
    double goalVecSum=0.0;
    for(int i=0; i<M; i++) {
        if(!areDoublesEqual(goals[i],Tigon::Lowest) ) {
            goalVecSum += normGoalVector[i];
        }
    }

    // modify the goal vector based on goalVecSum
    for(int i=0; i<M; i++) {
        if(areDoublesEqual(goals[i],Tigon::Lowest)) {
            if(goalVecSum>1.0) {
                normGoalVector[i] = 1.0;
            }
            else {
                normGoalVector[i] = 0.0;
            }
        }
    }

    int N = oSet->size();
    TVector <double> wvec;
    cout << "weights: " << endl;
    for(int i=0; i<N; i++) {
        wvec = oSet->at(i)->weightingVec();

        if(goalVecSum>1.0) {
            /// check if the goal vector does not weakly dominate the weight vector
            tribool res = Tigon::weakDominance(normGoalVector,wvec);
            QCOMPARE(res.value==tribool::true_value, false);
        }
        else {
            /// check if the weight vector does not weakly dominate the goal vector
            tribool res = Tigon::weakDominance(wvec,normGoalVector);
            QCOMPARE(res.value==tribool::true_value, false);
        }

        //cout << "weights[" << i+1 << "]: ";
        int vecSize = wvec.size();
        for(int j=0; j<vecSize; j++) {
            double value = wvec[j];
            cout << value << " ";
        }
        cout << endl;
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_moead::test_goalBelowSimplexLattice() {

    int M=3;
    int populationSize = 210;
    int dVecSize=12;
    int oVecSize=3;

    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    Tigon::ProblemDefinitionStatus status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    prob->problem()->redefineObjOptimizationType(0,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(1,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(2,OptimizationType::Minimization);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    RandomInit*           init      = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    WeightVectorInit* wvector = new WeightVectorInit(evaluator);

    TVector <double> goals;
    goals << 0.15 << 0.6 << Tigon::Lowest;
    TVector<ElementType> types(3, RealType);

    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector <double> normGoalVector(M);
    for(int i=0;i<M;i++) {
        normGoalVector[i]=goals[i];
    }

    TVector<double> ideal = IElementVecToRealVec(wvector->idealVec());
    TVector<double> nadir = IElementVecToRealVec(wvector->nadirVec());
    normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

    // calculate the goal vector sum based on the components that have been set
    double goalVecSum=0.0;
    for(int i=0; i<M; i++) {
        if(!areDoublesEqual(goals[i],Tigon::Lowest) ) {
            goalVecSum += normGoalVector[i];
        }
    }

    // modify the goal vector based on goalVecSum
    for(int i=0; i<M; i++) {
        if(areDoublesEqual(goals[i],Tigon::Lowest)) {
            if(goalVecSum>1.0) {
                normGoalVector[i] = 1.0;
            }
            else {
                normGoalVector[i] = 0.0;
            }
        }
    }

    int N = oSet->size();
    TVector <double> wvec;
    cout << "weights: " << endl;
    for(int i=0; i<N; i++) {
        wvec = oSet->at(i)->weightingVec();

        if(goalVecSum>1.0) {
            /// check if the goal vector does not weakly dominate the weight vector
            tribool res = Tigon::weakDominance(normGoalVector,wvec);
            QCOMPARE(res.value==tribool::true_value, false);
        }
        else {
            /// check if the weight vector does not weakly dominate the goal vector
            tribool res = Tigon::weakDominance(wvec,normGoalVector);
            QCOMPARE(res.value==tribool::true_value, false);
        }

        //cout << "weights[" << i+1 << "]: ";
        int vecSize = wvec.size();
        for(int j=0; j<vecSize; j++) {
            double value = wvec[j];
            cout << value << " ";
        }
        cout << endl;
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;

}

void tst_moead::test_goalAboveSimplexLattice() {

    int M=3;
    int populationSize = 210;
    int dVecSize=12;
    int oVecSize=3;

    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    Tigon::ProblemDefinitionStatus status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    prob->problem()->redefineObjOptimizationType(0,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(1,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(2,OptimizationType::Minimization);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    RandomInit*           init      = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    WeightVectorInit* wvector = new WeightVectorInit(evaluator);

    TVector <double> goals;
    goals << 2.30 << 0.6 << Tigon::Lowest;
    TVector<ElementType> types(3, RealType);

    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector <double> normGoalVector(M);
    for(int i=0;i<M;i++) {
        normGoalVector[i]=goals[i];
    }

    TVector<double> ideal = IElementVecToRealVec(wvector->idealVec());
    TVector<double> nadir = IElementVecToRealVec(wvector->nadirVec());
    normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

    // calculate the goal vector sum based on the components that have been set
    double goalVecSum=0.0;
    for(int i=0; i<M; i++) {
        if(!areDoublesEqual(goals[i],Tigon::Lowest) ) {
            goalVecSum += normGoalVector[i];
        }
    }

    // modify the goal vector based on goalVecSum
    for(int i=0; i<M; i++) {
        if(areDoublesEqual(goals[i],Tigon::Lowest)) {
            if(goalVecSum>1.0) {
                normGoalVector[i] = 1.0;
            }
            else {
                normGoalVector[i] = 0.0;
            }
        }
    }

    int N = oSet->size();
    TVector <double> wvec;
    cout << "weights: " << endl;
    for(int i=0; i<N; i++) {
        wvec = oSet->at(i)->weightingVec();

        if(goalVecSum>1.0) {
            /// check if the goal vector does not weakly dominate the weight vector
            tribool res = Tigon::weakDominance(normGoalVector,wvec);
            QCOMPARE(res.value==tribool::true_value, false);
        }
        else {
            /// check if the weight vector does not weakly dominate the goal vector
            tribool res = Tigon::weakDominance(wvec,normGoalVector);
            QCOMPARE(res.value==tribool::true_value, false);
        }

        //cout << "weights[" << i+1 << "]: ";
        int vecSize = wvec.size();
        for(int j=0; j<vecSize; j++) {
            double value = wvec[j];
            cout << value << " ";
        }
        cout << endl;
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;

}

void tst_moead::test_goalMaximization() {

    int M=3;
    int populationSize = 210;
    int dVecSize=12;
    int oVecSize=3;

    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    Tigon::ProblemDefinitionStatus status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    prob->problem()->redefineObjOptimizationType(0,OptimizationType::Maximization);
    prob->problem()->redefineObjOptimizationType(1,OptimizationType::Maximization);
    prob->problem()->redefineObjOptimizationType(2,OptimizationType::Maximization);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    RandomInit*           init      = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    WeightVectorInit* wvector = new WeightVectorInit(evaluator);

    TVector <double> goals;
    goals << 0.1 << -0.6 << Tigon::Lowest;
    TVector<ElementType> types(3, RealType);

    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector <double> normGoalVector(M);
    for(int i=0;i<M;i++) {
        normGoalVector[i]=goals[i];
    }

    TVector<double> ideal = IElementVecToRealVec(wvector->idealVec());
    TVector<double> nadir = IElementVecToRealVec(wvector->nadirVec());
    normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

    // calculate the goal vector sum based on the components that have been set
    double goalVecSum=0.0;
    for(int i=0; i<M; i++) {
        if(!areDoublesEqual(goals[i],Tigon::Lowest) ) {
            goalVecSum += normGoalVector[i];
        }
    }

    // modify the goal vector based on goalVecSum
    for(int i=0; i<M; i++) {
        if(areDoublesEqual(goals[i],Tigon::Lowest)) {
            if(goalVecSum>1.0) {
                normGoalVector[i] = 1.0;
            }
            else {
                normGoalVector[i] = 0.0;
            }
        }
    }

    int N = oSet->size();
    TVector <double> wvec;
    cout << "weights: " << endl;
    for(int i=0; i<N; i++) {
        wvec = oSet->at(i)->weightingVec();

        if(goalVecSum>1.0) {
            /// check if the goal vector does not weakly dominate the weight vector
            tribool res = Tigon::weakDominance(normGoalVector,wvec);
            QCOMPARE(res.value==tribool::true_value, false);
        }
        else {
            /// check if the weight vector does not weakly dominate the goal vector
            tribool res = Tigon::weakDominance(wvec,normGoalVector);
            QCOMPARE(res.value==tribool::true_value, false);
        }

        //cout << "weights[" << i+1 << "]: ";
        int vecSize = wvec.size();
        for(int j=0; j<vecSize; j++) {
            double value = wvec[j];
            cout << value << " ";
        }
        cout << endl;
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;

}

void tst_moead::test_goalVectorsNotSet() {

    int M=3;
    int populationSize = 210;
    int dVecSize=12;
    int oVecSize=3;

    TRAND.defineSeed(465654);

    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    //IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    IFunctionSPtr func = IFunctionSPtr(new DTLZ2);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    Tigon::ProblemDefinitionStatus status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    prob->problem()->redefineObjOptimizationType(0,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(1,OptimizationType::Minimization);
    prob->problem()->redefineObjOptimizationType(2,OptimizationType::Minimization);

    status = prob->problem()->processProblemDefinition();
    qDebug() << status;
    QCOMPARE(status == Tigon::FullyDefined, true);

    RandomInit*           init      = new RandomInit(prob);
    Evaluator* evaluator = new Evaluator(init);
    WeightVectorInit* wvector = new WeightVectorInit(evaluator);

    TVector <double> goals;
    goals << Tigon::Lowest << Tigon::Lowest << Tigon::Lowest;
    TVector<ElementType> types(3, RealType);

    prob->defineGoalVec(createIElementSPtrVector(goals, types));

    init->addOutputTag(Tigon::TForNeighbourhoods);
    init->TP_defineSetSize(populationSize);

    wvector->evaluate();

    ISet* oSet = wvector->outputSet(0);

    TVector <double> normGoalVector(M);
    for(int i=0;i<M;i++) {
        normGoalVector[i]=goals[i];
    }

    TVector<double> ideal = IElementVecToRealVec(wvector->idealVec());
    TVector<double> nadir = IElementVecToRealVec(wvector->nadirVec());
    normaliseToUnitBoxInMemory(normGoalVector, ideal, nadir);

    // calculate the goal vector sum based on the components that have been set
    double goalVecSum=0.0;
    for(int i=0; i<M; i++) {
        if(!areDoublesEqual(goals[i],Tigon::Lowest) ) {
            goalVecSum += normGoalVector[i];
        }
    }

    // modify the goal vector based on goalVecSum
    for(int i=0; i<M; i++) {
        if(areDoublesEqual(goals[i],Tigon::Lowest)) {
            if(goalVecSum>1.0) {
                normGoalVector[i] = 1.0;
            }
            else {
                normGoalVector[i] = 0.0;
            }
        }
    }

    int N = oSet->size();
    TVector <double> wvec;
    cout << "weights: " << endl;
    for(int i=0; i<N; i++) {
        wvec = oSet->at(i)->weightingVec();

        if(goalVecSum>1.0) {
            /// check if the goal vector does not weakly dominate the weight vector
            tribool res = Tigon::weakDominance(normGoalVector,wvec);
            QCOMPARE(res.value==tribool::true_value, false);
        }
        else {
            /// check if the weight vector does not weakly dominate the goal vector
            tribool res = Tigon::weakDominance(wvec,normGoalVector);
            QCOMPARE(res.value==tribool::true_value, false);
        }

        //cout << "weights[" << i+1 << "]: ";
        int vecSize = wvec.size();
        for(int j=0; j<vecSize; j++) {
            double value = wvec[j];
            cout << value << " ";
        }
        cout << endl;
    }

    delete wvector;
    delete evaluator;
    delete init;
    delete prob;
    delete base;

}



#endif

QTEST_GUILESS_MAIN(tst_moead)

#include "tst_moead.moc"
