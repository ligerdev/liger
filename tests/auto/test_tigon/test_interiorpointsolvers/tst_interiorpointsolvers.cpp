/****************************************************************************
*****************************************************************************
**
** Copyright (C) 2012-2014 The University of Sheffield (www.sheffield.ac.uk)
** Copyright (C) 2012-2014 Ioannis Giagkiozis (www.sheffield.ac.uk)
**
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

#include <QString>
#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>
#include <eigen/bench/BenchTimer.h>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

#include <QSharedPointer>

using namespace std;
using namespace Tigon::Representation;

class tst_interiorpointsolvers : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_InteriorPointQPSolver_Initialisation();
    void test_InteriorPointQPSolver_Evaluation();
    void test_InteriorPointQPSolver_Direction();
    void test_InteriorPointQPSolver_Algorithm_DefineProblemBeforeAlg();
    void test_InteriorPointQPSolver_Algorithm_DefineProblemAfterAlg();
    void test_InteriorPointQPSolver_RandomTest();
};

void generateQPTestProblem1(Tigon::Operators::ProblemGenerator* pg)
{
    /*
     * Test problem 1:
     * min 0.5x1^2 + 0.5x2^2
     * s.t. x1+x2 = 1, x1>=0; x2>=0.
     */

    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;

    bool success = pg->addVariable(RealType,QString("x1"),QString("x1"),
                                     -qInf(),+qInf(),QString("none"));
    success = pg->addVariable(RealType,QString("x2"),QString("x2"),
                                -qInf(),+qInf(),QString("none"));
    success = pg->addObjective(RealType,QString("y"),
                                 QString("QP: 0.5*x\'Qx+f\'x"),
                                 QString("none"),
                                 Tigon::Minimization,0);

    QVector<QVector<qreal> > QVec(2);
    QVector<qreal> cVec(2);
    QVec[0].append(1);
    QVec[0].append(0);
    QVec[1].append(0);
    QVec[1].append(1);
    cVec[0] = 0;
    cVec[1] = 0;

    LinearConstraintsData* equality = new LinearConstraintsData();
    LinearConstraintsData* inequality = new LinearConstraintsData();
    equality->setNVar(pg->decisionVecSize());
    QMap<int,qreal> ruleEq;
    ruleEq[0] = 1;
    ruleEq[1] = 1;
    equality->addLinearConstraint(ruleEq,1.0);
    inequality->setNVar(pg->decisionVecSize());
    QMap<int,qreal> ruleIneq;;
    ruleIneq[0] = -1.0; ruleIneq[1] = 0.0;
    inequality->addLinearConstraint(ruleIneq,0.0);
    ruleIneq.clear();
    ruleIneq[0] = 0.0; ruleIneq[1] = -1.0;
    inequality->addLinearConstraint(ruleIneq,0.0);

    // Setup numImappings identical problems
    int numImappings = 3;
    pg->setOptimizationSetSize(numImappings);
    for (int k = 0; k < numImappings; k++) {
        pg->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->setNVar(2);
        pg->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->defineQuadraticCoeff(QVec);
        pg->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->defineLinearCoeff(cVec);
        pg->optimizationSet()[k]
                ->assignLinearEqConstraints(equality);
        pg->optimizationSet()[k]
                ->assignLinearIneqConstraints(inequality);
    }
}

void generateQPTestProblem2(Tigon::Operators::ProblemGenerator* pg)
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;

    /*
     * Test problem 2:
     * min 0.5x1^2+x2^2-x1x2-2x1-6x2
     * subject to
     * x1 + 2x2 = 1
     * x1 + x2 ≤ 2
     * –x1 + 2x2 ≤ 2
     * 2x1 + x2 ≤ 3
     * 0 ≤ x1, 0 ≤ x2.
     */
    cout << "Create test problems" << endl;
    LinearConstraintsData* equality = new LinearConstraintsData();
    LinearConstraintsData* inequality = new LinearConstraintsData();
    bool success = pg->addVariable(RealType,QString("x1"),QString("x1"),
                                   -qInf(),+qInf(),QString("none"));
    success = pg->addVariable(RealType,QString("x2"),QString("x2"),
                              -qInf(),+qInf(),QString("none"));
    success = pg->addObjective(RealType,QString("y"),
                               QString("QP: 0.5*x\'Qx+c\'x"),
                               QString("none"),
                               Tigon::Minimization,0);
    equality->setNVar(pg->decisionVecSize());
    QMap<int,qreal> ruleEq;
    ruleEq[0] = 1;
    ruleEq[1] = 2;
    equality->addLinearConstraint(ruleEq,1.0);
    inequality->setNVar(pg->decisionVecSize());
    QMap<int,qreal> ruleIneq;
    ruleIneq[0] = 1.0; ruleIneq[1] = 1.0;
    inequality->addLinearConstraint(ruleIneq,2.0);
    ruleIneq.clear();
    ruleIneq[0] = -1.0; ruleIneq[1] = 2.0;
    inequality->addLinearConstraint(ruleIneq,2.0);
    ruleIneq.clear();
    ruleIneq[0] = 2.0; ruleIneq[1] = 1.0;
    inequality->addLinearConstraint(ruleIneq,3.0);
    ruleIneq.clear();
    ruleIneq[0] = -1.0; ruleIneq[1] = 0.0;
    inequality->addLinearConstraint(ruleIneq,0.0);
    ruleIneq.clear();
    ruleIneq[0] = 0.0; ruleIneq[1] = -1.0;
    inequality->addLinearConstraint(ruleIneq,0.0);
    ruleIneq.clear();

    QVector<QVector<qreal> > Q(2);
    Q[0].append(1.0);  Q[0].append(-1.0);
    Q[1].append(-1.0); Q[1].append(2.0);
    QVector<qreal> c(2);
    c[0] = -2; c[1] = -6;

    // Setup K identical problems, K = numImappings;
    int numImappings = 3;
    pg->setOptimizationSetSize(numImappings);
    for (int k = 0; k < numImappings; k++) {
        pg->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->setNVar(pg->decisionVecSize());
        pg->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->defineQuadraticCoeff(Q);
        pg->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->defineLinearCoeff(c);
        pg->optimizationSet()[k]
                ->assignLinearEqConstraints(equality);
        pg->optimizationSet()[k]
                ->assignLinearIneqConstraints(inequality);
    }
}

/* Randomly generate a feasible qudratic programming problem*/
void generateRandomQP(int n, int m_a, int m_c,
                      TMatrixReal&  Q,
                      TCVectorReal& c,
                      TMatrixReal&  A,
                      TCVectorReal& b,
                      TMatrixReal&  H,
                      TCVectorReal& d)
{
    using namespace std;

    // Normal distribution generator
    // TODO: update the random number generator with the Tigon RNG.
    typedef boost::mt19937                     ENG;    // Mersenne Twister
    typedef boost::normal_distribution<double> DIST;   // Normal Distribution
    typedef boost::variate_generator<ENG,DIST> GEN;    // Variate generator
    ENG  eng;
    eng.seed(time(NULL));
    DIST dist(0,1);   // mean = 0; sigma = 1;
    GEN  gen(eng,dist);

    Q.resize(n,n);
    while(true) {
        Q = TMatrixReal::Random(n,n);
        FullPivLU<TMatrixReal> lu_decomp(Q);
        if(lu_decomp.rank() == n) {
            break;
        }
    }
    Q = Q*Q;

    A.resize(m_a,n);
    for(int i = 0; i < m_a; i++) {
        for(int j = 0; j < n; j++) {
            A(i,j) = gen();
        }
    }
    H.resize(m_c,n);
    for(int i = 0; i < m_c; i++) {
        for(int j = 0; j < n; j++) {
            H(i,j) = gen();
        }
    }

    TCVectorReal x = TCVectorReal::Random(n);
    TCVectorReal y = TCVectorReal::Random(m_a);
    TCVectorReal z = TCVectorReal::Random(m_c).cwiseAbs();
    TCVectorReal s = TCVectorReal::Random(m_c).cwiseAbs();
    b = A*x;
    d = H*x + s;
    c = A.transpose()*y - H.transpose()*z - Q*x;
    cout << "Random Problem generated. n = "
         << n   << ", m_a = "
         << m_a << ", m_c = "
         << m_c << endl;
}

void tst_interiorpointsolvers::test_InteriorPointQPSolver_Initialisation()
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;
    using namespace std;

    PSetBase*              base = new PSetBase();
    ProblemGenerator*      ext1 = new ProblemGenerator(base);

    generateQPTestProblem1(ext1);

    cout << "IPM QP Initialisation" << endl;
    IPMQPInitialisation* ext2 = new IPMQPInitialisation(ext1);
    ext2->evaluate();

    QVector<qreal> ini_x(2);
    ini_x[0] = 2.0;
    ini_x[1] = 2.0;
    QVector<qreal> ini_y(1);
    ini_y[0] = 0;
    QVector<qreal> ini_z(2);
    ini_z[0] = 2.0;
    ini_z[1] = 2.0;
    QVector<qreal> ini_s(2);
    ini_s[0] = 2.0;
    ini_s[1] = 2.0;
    // x
    for(int i=0; i<ext2->decisionVecSize(); i++) {
        QCOMPARE(ini_x[i],
                 ext2->optimizationSet()[0]
                ->inputVec()[i]
                ->value().toDouble());
    }
    // y
    for(int i=0; i<1; i++) {
         QCOMPARE(ini_y[i],
                  ext2->genericSet()[0]
                 ->inputVec()[i]
                 ->value().toDouble());
    }
    // z
    for(int i=1; i<1+2; i++) {
        QCOMPARE(ini_z[i-1],
                 ext2->genericSet()[0]
                ->inputVec()[i]
                ->value().toDouble());
    }
    // s
    for(int i=3; i<1+2+2; i++) {
        QCOMPARE(ini_s[i-3],
                 ext2->genericSet()[0]
                ->inputVec()[i]
                ->value().toDouble());
    }
}


void tst_interiorpointsolvers::test_InteriorPointQPSolver_Evaluation()
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;
    using namespace std;

    PSetBase*              base = new PSetBase();
    ProblemGenerator*      ext1 = new ProblemGenerator(base);

    generateQPTestProblem1(ext1);

    IPMQPInitialisation* ext2 = new IPMQPInitialisation(ext1);
    IPMQPEvaluator* ext3 = new IPMQPEvaluator(ext2);
    ext3->evaluate();

    QVector<qreal> residuals;
    residuals.append(0.0);
    residuals.append(0.0);
    residuals.append(3.0);
    residuals.append(0.0);
    residuals.append(0.0);
    residuals.append(4.0);
    residuals.append(4.0);
    residuals.append(3.201562118716424); // the relative primal-dual residual
    int numImappings = ext3->optimizationSetSize();
    for(int k = 0; k < numImappings; k++) {
        for(int i = 0; i<ext3->genericSet()[k]->outputVec().size(); i++) {
            qreal res = ext3->genericSet()[k]
                    ->outputVec()[i]
                    ->value().toReal();
            QCOMPARE(res,residuals[i]);
        }
    }
}

void tst_interiorpointsolvers::test_InteriorPointQPSolver_Direction()
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;
    using namespace std;

    PSetBase* base = new PSetBase();
    ProblemGenerator* ext1 = new ProblemGenerator(base);
    generateQPTestProblem1(ext1);
    IPMQPInitialisation* ext2 = new IPMQPInitialisation(ext1);
    IPMQPEvaluator* ext3 = new IPMQPEvaluator(ext2);

    // Direction
    cout << "IPM QP Direction" << endl;
    IPMQPDirection* ext4 = new IPMQPDirection(ext3);
    ext4->evaluate();
    QVector<qreal> new_x;
    QVector<qreal> new_yzs;
    new_x.append(0.500750000000000);
    new_x.append(0.500750000000000);
    new_yzs.append(-0.637930506747969);
    new_yzs.append(1.138680506747969);
    new_yzs.append(1.138680506747969);
    new_yzs.append(0.500750000000000);
    new_yzs.append(0.500750000000000);
    int numImappings = ext4->optimizationSetSize();
    for(int k = 0; k < numImappings; k++) {
        for(int i = 0; i < ext4->decisionVecSize(); i++) {
            QCOMPARE(ext4->optimizationSet()[k]
                    ->inputVec()[i]
                    ->value().toReal(),
                    new_x[i]);
        }
        for(int i = 0; i < ext4->genericSet()[k]
            ->inputVec().size(); i++) {
            QCOMPARE(ext4->genericSet()[k]
                    ->inputVec()[i]
                    ->value().toReal(),
                    new_yzs[i]);
        }
    }

}

void tst_interiorpointsolvers::test_InteriorPointQPSolver_Algorithm_DefineProblemBeforeAlg()
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;
    using namespace Tigon::Algorithms;
    using namespace std;

    PSetBase*              base = new PSetBase();
    ProblemGenerator*      pg = new ProblemGenerator(base);
    generateQPTestProblem2(pg);
    IPMQPAlgorithm* ipm  = new IPMQPAlgorithm(pg);

    cout << "Run algorithm - main loop" << endl;
    // Algorithm
    qreal residual;
    for(int i = 0; i < 50; i++) {
        ipm->evaluate();
        ipm->incrementIteration();
        bool terminate = true;
        cout << "Iter: " << ipm->currentIteration() << ",  ";
        int numImappings = ipm->optimizationSetSize();
        for(int k = 0; k < numImappings; k++) {
            residual = ipm->genericSet()[k]
                ->outputVec().last()->value().toReal();
             cout << "Res[" << k <<"]: "
                  << setprecision(4) << scientific
                  << residual << ", ";
             if(residual > 1e-08) {
                 terminate = false;
             }
        }
        cout << endl;

        if(terminate) {
            break;
        }
    }
    cout << "optimal solution: " << endl;
    cout << "x = ( ";
    for(int i = 0; i < ipm->decisionVecSize(); i++) {
        cout << scientific
             << setprecision(2)
             << ipm->optimizationSet()[0]
                ->inputVec()[i]
                ->value().toReal()
             << " ";
    }
    cout << ")" << endl;

    cout << "optimal fval: "
         << scientific
         << setprecision(2)
         << ipm->optimizationSet()[0]
            ->outputVec()[0]
            ->value().toReal()
            << endl;

    delete ipm;
}

void tst_interiorpointsolvers::test_InteriorPointQPSolver_Algorithm_DefineProblemAfterAlg()
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;
    using namespace Tigon::Algorithms;
    using namespace std;

    PSetBase*              base = new PSetBase();
    IPMQPAlgorithm*        ipm = new IPMQPAlgorithm(base);
    ProblemGenerator*      pg = new ProblemGenerator(ipm);
    generateQPTestProblem2(pg);
    int numImappings = pg->optimizationSetSize();

    cout << "Run algorithm - main loop" << endl;
    // Algorithm
    qreal residual;
    for(int i = 0; i < 50; i++) {
        pg->evaluate();
        pg->incrementIteration();
        bool terminate = true;
        cout << "Iter: " << ipm->currentIteration() << ",  ";
        for(int k = 0; k < numImappings; k++) {
            residual = pg->genericSet()[k]
                ->outputVec().last()->value().toReal();
             cout << "Res[" << k <<"]: "
                  << setprecision(4) << scientific
                  << residual << ", ";
             if(residual > 1e-08) {
                 terminate = false;
             }
        }
        cout << endl;

        if(terminate) {
            break;
        }
    }
    cout << "optimal solution: " << endl;
    cout << "x = ( ";
    for(int i = 0; i < pg->decisionVecSize(); i++) {
        cout << scientific
             << setprecision(2)
             << pg->optimizationSet()[0]
                ->inputVec()[i]
                ->value().toReal()
             << " ";
    }
    cout << ")" << endl;

    cout << "optimal fval: "
         << scientific
         << setprecision(2)
         << pg->optimizationSet()[0]
            ->outputVec()[0]
            ->value().toReal()
            << endl;

    delete pg;
}

void tst_interiorpointsolvers::test_InteriorPointQPSolver_RandomTest()
{
    using namespace Tigon;
    using namespace Tigon::Representation;
    using namespace Tigon::Operators;
    using namespace Tigon::Algorithms;
    using namespace std;

    int n   = 30;
    int m_a = 25;
    int m_c = 25;
    int numImappings = 5;

    PSetBase*              base = new PSetBase();
    ProblemGenerator*      prob = new ProblemGenerator(base);
    bool success;
    // Add objective
    success = prob->addObjective(RealType,QString("y"),
                                 QString("QP: 0.5*x\'Qx+c\'x"),
                                 QString("none"),
                                 Tigon::Minimization,0);
    // Add variables
    for(int i = 0; i < n; i++ ) {
        success = prob
                ->addVariable(RealType,
                            QString("x"+QString::number(i)),
                            QString("x"+QString::number(i)),
                            -qInf(),+qInf(),QString("none"));
    }

    // Random matrices set up
    prob->setOptimizationSetSize(numImappings);
    TMatrixReal  Q, A, H;
    TCVectorReal c, b, d;
    for(int k = 0; k < numImappings; k++) {
        generateRandomQP(n,m_a,m_c,Q,c,A,b,H,d);

        // set obj
        QVector<QVector<qreal> > QVec(n);
        QVector<qreal> cVec(n);
        for (int i = 0; i< n; i++) {
            cVec[i] = c(i);
            for(int j = 0; j < n; j++) {
                QVec[i].append(Q(i,j));
            }
        }

        // Add linear equality and inequality
        LinearConstraintsData* equality = new LinearConstraintsData();
        LinearConstraintsData* inequality = new LinearConstraintsData();
        QMap<int,qreal> ruleEq;
        QMap<int,qreal> ruleIneq;
        equality->setNVar(n);
        inequality->setNVar(n);
        for(int i = 0; i < m_a; i++) {
            for(int j = 0; j < n; j++) {
                ruleEq[j] = A(i,j);
            }
            equality->addLinearConstraint(ruleEq,b(i));
            ruleEq.clear();
        }
        for(int i = 0; i < m_c; i++) {
            for(int j = 0; j < n; j++) {
                ruleIneq[j] = H(i,j);
            }
            inequality->addLinearConstraint(ruleIneq,d(i));
            ruleIneq.clear();
        }

        prob->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->setNVar(n);
        prob->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->defineQuadraticCoeff(QVec);
        prob->optimizationSet()[k]
                ->quadraticObjectiveData()
                ->defineLinearCoeff(cVec);
        prob->optimizationSet()[k]
                ->assignLinearEqConstraints(equality);
        prob->optimizationSet()[k]
                ->assignLinearIneqConstraints(inequality);
    }

    IPMQPAlgorithm*  ipm = new IPMQPAlgorithm(prob);

    // ======== Algorithm ========
    cout << "Run the alg to solve the randomly generated problem " << endl;
    qreal acc = 1e-09;
    for(int k = 0; k < numImappings; k++) {
        ipm->optimizationSet()[k]
                ->setAccuracy(acc);
    }
    int maxIter = 100;
    BenchTimer timer;
    timer.start();

    QVector<qreal> residual(ipm->decisionVecSize());
    for(int i = 0; i < maxIter; i++) {
        ipm->evaluate();
        ipm->incrementIteration();
        bool terminate = true;
        cout << "Iter: " << ipm->currentIteration() << ",  ";
        for(int k = 0; k < numImappings; k++) {
            residual[k] = ipm->genericSet()[k]
                    ->outputVec().last()->value().toReal();
            cout << "Res[" << k <<"]: "
                 << setprecision(4) << scientific
                 << residual[k] << ", ";
            if(residual[k] > acc) {
                terminate = false;
            }
        }
        cout << endl;
        if(terminate) {
            break;
        }
    }
    timer.stop();
    cout << "Terminated. Tol =  " << acc << endl;
    cout << "CPU time = "
         << setprecision(2)
         << timer.value()
         << " seconds"
         << endl;

    for(int k = 0; k < numImappings; k++) {
    cout << "optimal fval[ " << k
         << " ]: "
         << scientific
         << setprecision(2)
         << ipm->optimizationSet()[k]
            ->outputVec()[0]
            ->value().toReal()
         << endl;
    }
}

QTEST_APPLESS_MAIN(tst_interiorpointsolvers)

#include "tst_interiorpointsolvers.moc"


