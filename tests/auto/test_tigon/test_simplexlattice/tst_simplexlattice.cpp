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
#include <tigon/Tigon.h>

#include "wp4data.h"

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_simplexlattice : public QObject
{
    Q_OBJECT

public:

private slots:
    // Simplex lattice
    void test_simplexLatticeSize();
    void test_simplexLatticeRecursive();
    void test_simplexLatticeIterative();

    // Preferred simplex lattice
    void test_preferredSimplexLattice();
    void test_preferredSimplexLatticeWithObjReduction();
    void test_preferredSimplexLatticeWP4Data();
};


void dispVector(const TVector<double>& vec, string sep = "\t",
                string endLine = "\n")
{
    for(size_t i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

Tigon::Representation::ProblemSPtr createProblem(int dN, int oN,
                                                 Tigon::ElementType type)
{
    TVector<Tigon::ElementType> dTypeVec;
    TVector<Tigon::ElementType> oTypeVec;

    for(int i=0; i<dN; i++){
        dTypeVec.push_back(type);
    }

    for(int i=0; i<oN; i++){
        oTypeVec.push_back(type);
    }

    IFunctionSPtr func = IFunctionSPtr(new ExampleFunction);
    func->TP_defineNInputs(dN);
    func->TP_defineNOutputs(oN);
    TVector<IFunctionSPtr> fvec;
    fvec.push_back(func);

    TVector<int> fMap;
    for(int i=0; i<dN; i++) {
        fMap.push_back(i);
    }
    TVector<TVector<int> > f2dMap(1, fMap);

    fMap.clear();
    for(int i=0; i<oN; i++) {
        fMap.push_back(i);
    }
    TVector<TVector<int> > f2oMap(1, fMap);
    TVector<TVector<int> > f2pMap(1, TVector<int>());
    TVector<TVector<int> > f2cMap(1, TVector<int>());
    TVector<TVector<int> > f2uMap(1, TVector<int>());

    ProblemSPtr prob(new Problem);
    prob->defineDVecTypes(dTypeVec);
    prob->defineOVecTypes(oTypeVec);
    prob->defineFunctionVec(fvec);
    prob->defineF2dMap(f2dMap);
    prob->defineF2oMap(f2oMap);
    prob->defineF2pMap(f2pMap);
    prob->defineF2cMap(f2cMap);
    prob->defineF2uMap(f2uMap);
    prob->processProblemDefinition();

    qDebug() << "Problem Status: " << prob->isProblemDefined();

    return prob;
}

void tst_simplexlattice::test_simplexLatticeSize()
{
    int k = 3;
    int h = 13;

    double ex = factorial<double>(k+h-1) /
            (factorial<double>(h) * factorial<double>(k-1));

    double rs = simplexLatticeSize(h, k);
    QCOMPARE(rs, ex);
}

void tst_simplexlattice::test_simplexLatticeRecursive()
{
    int k = 20;
    int h = 5;

    double ex = factorial<double>(h+k-1) /
            factorial<double>(k-1) / factorial<double>(h);

    TVector<TVector<double> > simplex = simplexLatticeRecursive(h, k);
    double rs(simplex.size());
    QCOMPARE(rs, ex);
    for(auto row : simplex) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
    }

    h = 4;
    k = 3;
    int s = 8;
    simplex = simplexLatticeRecursive(h, k, s);
    cout << "Testing Simplex Lattice:" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << k <<
            ", s (sum of each vector) = " << s << endl;
    for(auto row : simplex) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,(double)s),true);
        for(auto elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

void tst_simplexlattice::test_simplexLatticeIterative()
{
    int k = 20;
    int h = 5;

    double ex = factorial<double>(h+k-1) /
            factorial<double>(k-1) / factorial<double>(h);

    TVector<TVector<double> > simplex = simplexLatticeIterative(h, k);
    double rs(simplex.size());
    QCOMPARE(rs, ex);
    for(auto row : simplex) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
    }

    h = 4;
    k = 3;
    simplex = simplexLatticeIterative(h, k);
    cout << "Testing Simplex Lattice:" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << k << endl;
    for(auto row : simplex) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        for(auto elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

void tst_simplexlattice::test_preferredSimplexLattice()
{
    int k = 3;
    int h = 10;

    TVector<TVector<double>> originalSimplex = simplexLatticeIterative(h,k);
    cout << "Original simplex lattice:" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << k << endl;
    for(auto row : originalSimplex) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        for(auto elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }

    // goal vector on top of simplex
    double ub = 0.5;
    TVector<double> goalTopSimplex(k, ub);
    TVector<TVector<double>> prefSimplex1 =
            preferredSimplexLattice(h, k, goalTopSimplex);

    cout << "Goal vector on top of simplex:" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << k << endl;
    for(auto row : prefSimplex1) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        /// check if the weight vector weakly dominates the goal vector
        tribool res = Tigon::weakDominance(row, goalTopSimplex);
        QCOMPARE(res.value==tribool::true_value, true);
        for(auto elem : row) {
            cout << elem << " ";
            QCOMPARE(bool(elem<=ub),true);
        }
        cout << endl;
    }

    // goal vector below the simplex
    double lb = 0.2;
    TVector<double> goalBelowSimplex(k, lb);
    TVector<TVector<double>> prefSimplex2 =
            preferredSimplexLattice(h, k, goalBelowSimplex);

    cout << "Goal vector below the simplex:" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << k << endl;
    for(auto row : prefSimplex2) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        /// check if the goal vector weakly dominates the weight vector
        tribool res = Tigon::weakDominance(goalBelowSimplex, row);
        QCOMPARE(res.value==tribool::true_value, true);
        for(auto elem : row) {
            cout << elem << " ";
            QCOMPARE(bool(elem>=lb),true);
        }
        cout << endl;
    }
}

void tst_simplexlattice::test_preferredSimplexLatticeWithObjReduction()
{
    int k = 4;
    int h = 6;

    TVector<TVector<double>> originalSimplex = simplexLatticeIterative(h,k);
    cout << "Original simplex lattice:" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << k << endl;
    cout << "Number of weight vectors: " << originalSimplex.size() << endl;
    for(auto row : originalSimplex) {
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        for(auto elem : row) {
            cout << elem << " ";
        }
        cout << endl;
    }

    TVector<bool> essentialObjs(k,true);
    essentialObjs[k-2]=false;  // third component is redundant

    // goal vector on top of simplex
    double ub=0.5;
    TVector<double> goalTopSimplex(k, ub);

    // create a subvector from the original goal based on redundant components
    TVector<double> subGoalT = goalTopSimplex;
    subGoalT.erase(std::remove_if(subGoalT.begin(),subGoalT.end(),
                 [&essentialObjs,&subGoalT](const double& d){
                    return !essentialObjs[&d - &*subGoalT.begin()];}),
                 subGoalT.end());

    TVector<TVector<double>> prefSimplex1 =
            preferredSimplexLatticeRedundantComponents(h, k,
                                                   goalTopSimplex,
                                                   essentialObjs);
    cout << "Goal vector on top of simplex (objective reduction):" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << vectorCount(essentialObjs, true) <<
            endl;
    cout << "Number of weight vectors: " << prefSimplex1.size() << endl;
    for(auto row : prefSimplex1) {
        QCOMPARE(row[k-2],0.0);
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);

        // create a subvector from the original row based on redundant
        // components
        TVector<double> subrow = row;
        subrow.erase(std::remove_if(subrow.begin(),subrow.end(),
                     [&essentialObjs,&subrow](const double& d){
                        return !essentialObjs[&d - &*subrow.begin()];}),
                     subrow.end());

        /// check if the weight vector weakly dominates the goal vector
        tribool res = Tigon::weakDominance(subrow, subGoalT);
        QCOMPARE(res.value==tribool::true_value, true);
        for(auto elem : row) {
            QCOMPARE(bool(elem<ub+Epsilon),true);
            cout << elem << " ";
        }
        cout << endl;
    }

    // goal vector below the simplex
    double lb = 0.2;
    TVector<double> goalBelowSimplex(k,lb);

    // create a subvector from the original goal based on redundant components
    TVector<double> subGoalB = goalBelowSimplex;
    subGoalB.erase(std::remove_if(subGoalB.begin(),subGoalB.end(),
                 [&essentialObjs,&subGoalB](const double& d){
                    return !essentialObjs[&d - &*subGoalB.begin()];}),
                 subGoalB.end());

    TVector<TVector<double>> prefSimplex2 =
            preferredSimplexLatticeRedundantComponents(h, k,
                                                   goalBelowSimplex,
                                                   essentialObjs);
    cout << "Goal vector below the simplex (objective reduction):" << endl <<
            "h (no. of intervals) = " << h <<
            ", k (no. of dimensions) = " << vectorCount(essentialObjs, true) <<
            endl;
    cout << "Number of weight vectors: " << prefSimplex2.size() << endl;
    for(auto row : prefSimplex2) {
        QCOMPARE(row[k-2],0.0);
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);

        // create a subvector from the original row based on redundant
        // components
        TVector<double> subrow = row;
        subrow.erase(std::remove_if(subrow.begin(),subrow.end(),
                     [&essentialObjs,&subrow](const double& d){
                        return !essentialObjs[&d - &*subrow.begin()];}),
                     subrow.end());

        /// check if the goal vector weakly dominates the weight vector
        tribool res = Tigon::weakDominance(subGoalB, subrow);
        QCOMPARE(res.value==tribool::true_value, true);
        for(auto elem : row) {
            if(elem>0.0) {
                QCOMPARE(bool(elem>lb-Epsilon),true);
            }
            cout << elem << " ";
        }
        cout << endl;
    }
}

void tst_simplexlattice::test_preferredSimplexLatticeWP4Data()
{
    /// *******************
    /// 0: Load the dataset
    /// *******************

    TVector<TVector<double>> dataset = engineDOE;
    TVector<double> rawGoal = engineGoal;
    TVector<OptimizationType> optType = engineOptType;
    int h = 3; // simplex lattice precision

    // number of dimensions of the goal vector must match
    // the number of columns in the dataset
    QCOMPARE(rawGoal.size(),dataset[0].size());

    int N  = dataset.size();
    int oN = rawGoal.size();

    cout << "Solutions properties" << endl;
    cout << "Number of solutions: " << N << endl;
    cout << "Number of objectives: " << oN << endl;

    ProblemSPtr prob = createProblem(1, oN, Tigon::RealType);

    for(int i=0; i<oN; i++) {
        prob->redefineObjOptimizationType(i,optType[i]);
    }
    prob->processProblemDefinition();

    for(int i=0; i<oN; i++) {
        switch(optType[i]) {
        case Maximization:
            rawGoal[i] = (-1.0)*rawGoal[i];
            for(int j=0; j<N; j++) {
                dataset[j][i] = (-1.0)*dataset[j][i];
            }
            break;
        case Minimization:
        default:
            break;
        }
    }

    /// *******************************
    /// 1. Identify a non-dominated set
    /// *******************************

    TVector<IMappingSPtr> sSet;
    for(int i=0; i<N; i++) {
        sSet.push_back(IMappingSPtr(new IMapping(prob)));

        IElement dVal(RealType, 0.0);

        sSet.at(i)->defineDecisionVar(0, dVal);
        for(int j=0; j<oN; j++) {
            IElement oVal(RealType, dataset[i][j]);
            sSet.at(i)->defineObjectiveVar(j, oVal);
        }
        sSet.at(i)->defineEvaluated();
        sSet.at(i)->defineOptimizationMapping(true);
    }

    bool weakDominance = true;
    TVector<TVector<IMappingSPtr>> ranks = nonDominanceSort(sSet,weakDominance);
    TVector<IMappingSPtr> front = ranks.at(0);

    cout << "Number of non-dominated solutions: " << front.size() << endl;

    /// *********************************
    /// 2. Get the nadir and ideal points
    /// *********************************

    PSetBase*     base = new PSetBase();
    IFormulation* form = new IFormulation(base);
    form->defineProblem(prob);

    base->defineKeepArchive(true);
    base->defineNadirVec(front.at(0)->objectiveVec());
    base->defineIdealVec(front.at(0)->objectiveVec());
    for(size_t i=1; i<front.size(); i++) {
        base->updateIdealNadirVec(front.at(i));
    }
    TVector<double> rsNadir = IElementVecToRealVec(base->nadirVec());
    cout << "rsNadir: " << rsNadir << endl;
    TVector<double> rsIdeal = IElementVecToRealVec(base->idealVec());
    cout << "rsIdeal: " << rsIdeal << endl;

    TVector<double> nadir = front.at(0)->doubleObjectiveVec();
    TVector<double> ideal = front.at(0)->doubleObjectiveVec();
    for(size_t i=1; i<front.size(); i++) {
        TVector<double> solution = front.at(i)->doubleObjectiveVec();
        for(int j=0; j<oN; j++) {
            if(solution[j]>nadir[j]) {
                nadir[j]=solution[j];
            }
            if(solution[j]<ideal[j]) {
                ideal[j]=solution[j];
            }
        }
    }
    cout << "Nadir: " << nadir << endl;
    cout << "Ideal: " << rsIdeal << endl;

    for(int i=0; i<oN; i++) {
        QCOMPARE(areDoublesEqual(rsNadir[i],nadir[i]),true);
        QCOMPARE(areDoublesEqual(rsIdeal[i],ideal[i]),true);
    }

    /// *********************************
    /// 3. Normalise the goal for simplex
    /// *********************************

    cout << "Goal vector" << endl;
    cout << "Original: " << rawGoal << endl;
    TVector<double> normGoal =
            normaliseForSimplexLattice(rawGoal, ideal, nadir);
    cout << "Normalised: " << normGoal << endl;

    /// *************************
    /// 5. Get the weight vectors
    /// *************************

    TVector<TVector<double>> w =
            preferredSimplexLattice(h, oN, normGoal);

    cout << "Weight vectors" << endl;
    cout << "Precision (h): " << h << endl;
    cout << "Number: " << w.size() << endl;

    for(auto row: w) {
        // check the weight vectors are valid
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        for(auto elem: row) {
            cout << elem << " ";
        }
        cout << endl;
    }

    TVector<TVector<double>> w2 =
            preferredSimplexLatticeCrowding(h, oN, normGoal, 5);

    cout << "Weight vectors" << endl;
    cout << "Precision (h): " << h << endl;
    cout << "Number: " << w2.size() << endl;

    for(auto row: w2) {
        // check the weight vectors are valid
        double sum = std::accumulate(row.cbegin(),row.cend(),0.0);
        QCOMPARE(areDoublesEqual(sum,1.0),true);
        for(auto elem: row) {
            cout << elem << " ";
        }
        cout << endl;
    }
}

QTEST_GUILESS_MAIN(tst_simplexlattice)

#include "tst_simplexlattice.moc"
