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
#include <iomanip>

#include <tigon/Tigon.h>

#include <boost/math/constants/constants.hpp>
using namespace boost::math::constants;

#define TST_CTP_SOLUTIONS
#define TST_CTP_MAPPINGS

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;

class tst_ctp : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_CTP_SOLUTIONS
    void test_check_solutions_CTP1();
    void test_check_solutions_CTP2();
    void test_check_solutions_CTP3();
    void test_check_solutions_CTP4();
    void test_check_solutions_CTP5();
    void test_check_solutions_CTP6();
    void test_check_solutions_CTP7();
#endif

#ifdef TST_CTP_MAPPINGS
    void test_check_mappings_CTP1();
    void test_check_mappings_CTP2();
    void test_check_mappings_CTP3();
    void test_check_mappings_CTP4();
    void test_check_mappings_CTP5();
    void test_check_mappings_CTP6();
    void test_check_mappings_CTP7();
#endif

};


void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(size_t i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

double df2(double x1, double x2) {
    return 1.0+x2-x1;
}

#ifdef TST_CTP_SOLUTIONS
void tst_ctp::test_check_solutions_CTP1()
{
    /// Define the problem
    IFunctionSPtr func(new CTP1);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2 << 3;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(2, 0.0);
    TVector<ElementType> types(2, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();


    IMappingSPtr imap(new IMapping(prob));

    /// Pareto-optimal decision-vector
    /// The solution is found in an unconstraint
    /// part of the Pareto-optimal region
    double x1=0.2;
    double x2=0.0;
    imap->defineDecisionVar(0,IElement(x1));
    imap->defineDecisionVar(1,IElement(x2));
    QCOMPARE(imap->isEvaluated(), false);
    int nFuncsEvaluated = imap->evaluate();
    QCOMPARE(nFuncsEvaluated, 1);
    QCOMPARE(imap->isEvaluated(), true);
    QCOMPARE(imap->isFeasible(), true);
    TVector<double> cVec = imap->doubleConstraintVec();
    QCOMPARE(cVec[0]>threshVec[0], false);
    QCOMPARE(cVec[1]>threshVec[1], false);

    /// Infeasible Pareto-optimal decision-vector
    /// for first constraint only
    x1=0.4;
    x2=0.0;
    imap->defineDecisionVar(0,IElement(x1));
    imap->defineDecisionVar(1,IElement(x2));
    imap->evaluate();
    QCOMPARE(imap->isFeasible(), false);
    cVec = imap->doubleConstraintVec();
    QCOMPARE(cVec[0]>threshVec[0], true);
    QCOMPARE(cVec[1]>threshVec[1], false);


    /// Infeasible Pareto-optimal decision-vector
    /// for both constraints
    x1=0.8;
    x2=0.0;
    imap->defineDecisionVar(0,IElement(x1));
    imap->defineDecisionVar(1,IElement(x2));
    imap->evaluate();
    QCOMPARE(imap->isFeasible(), false);
    cVec = imap->doubleConstraintVec();
    QCOMPARE(cVec[0]>threshVec[0], true);
    QCOMPARE(cVec[1]>threshVec[1], true);
}

void tst_ctp::test_check_solutions_CTP2()
{
    cout.precision(13);

    double x1, x2, term1, term2, diff;
    TVector<TVector<double>> pof;
    TVector<double> cVec;

    /// Define the problem
    IFunctionSPtr func(new CTP2);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    double tvalue=Tigon::Epsilon;
    TVector<double> threshVec(1, tvalue);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    IMappingSPtr imap(new IMapping(prob));

    /// Brute-force the Pareto-optimal solutions
    /// for CTP2 it is easy to brute-force POF solutions

    /// Problem parameters
    double theta = -0.2*pi<double>();
    double a = 0.2;
    double b = 10.0;
    double c = 1.0;
    double d = 6.0;
    double e = 1.0;

    int nsol=100;
    for(int i=0; i<nsol; i++) {
        x1 = (double)i/((double)nsol-1);
        x2 = x1*std::sin(theta)/std::cos(theta) + e-1.0+x1;

        term1=std::cos(theta)*(df2(x1,x2)-e) - std::sin(theta)*x1;
        term2=std::sin(theta)*(df2(x1,x2)-e) + std::cos(theta)*x1;
        term2=b*pi<double>()*std::pow(term2,c);
        term2=std::abs(std::sin(term2));
        term2=a*std::pow(term2,d);

        diff=std::abs(term1-term2);
        if( (diff<tvalue) || (term1>term2) ) {
            TVector<double> sol;
            sol << x1 << x2;
            pof.push_back(sol);
        }
    }

    /// Pareto-optimal solutions
    for(auto sol : pof) {
        x1=sol.at(0);
        x2=sol.at(1);

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        QCOMPARE(imap->isEvaluated(), false);
        imap->evaluate();
        QCOMPARE(imap->isEvaluated(), true);
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), true);
        QCOMPARE(cVec[0]>threshVec[0], false);
    }

    /// Infeasible solutions that would be
    /// Pareto-optimal if constraint is removed
    nsol=100;
    for(int i=0; i<nsol; i++) {
        x1=((double)i+1)/(double)nsol; // skip zero
        x2=0.0;

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        imap->evaluate();
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), false);
        QCOMPARE(cVec[0]>threshVec[0], true);
    }
}

void tst_ctp::test_check_solutions_CTP3()
{
    cout.precision(13);

    double x1, x2;
    int nsol;
    TVector<double> cVec;

    /// Define the problem
    IFunctionSPtr func(new CTP3);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    /// tvalue is the precision (error) used to brute-force POF solutions
    /// in a computational feasible time
    double tvalue = 0.000044;
    TVector<double> threshVec(1, tvalue);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    IMappingSPtr imap(new IMapping(prob));

    /// Solutions found by brute-force
    TVector<TVector<double>> pof(13);
    pof[0]  << 0.0 << 0.0;
    pof[1]  << 0.08090170080902 << 0.0221231745833;
    pof[2]  << 0.161803401618 << 0.0442463491666;
    pof[3]  << 0.2427051024271 << 0.0663695237499;
    pof[4]  << 0.3236067932361 << 0.08849269559863;
    pof[5]  << 0.4045084940451 << 0.1106158701819;
    pof[6]  << 0.4854101948541 << 0.1327390447652;
    pof[7]  << 0.5663118956631 << 0.1548622193485;
    pof[8]  << 0.6472135964721 << 0.1769853939318;
    pof[9]  << 0.7281152972812 << 0.1991085685151;
    pof[10] << 0.8090169980902 << 0.2212317430984;
    pof[11] << 0.8899186888992 << 0.2433549149472;
    pof[12] << 0.9708203897082 << 0.2654780895305;


    /*
    /// Brute-force the Pareto-optimal solutions

    /// Problem parameters
    double theta = -0.2*pi<double>();
    double a = 0.1;
    double b = 10.0;
    double c = 1.0;
    double d = 0.5;
    double e = 1.0;

    double term1, term2, diff;
    nsize=100000000;
    cout << endl;
    for(int i=0; i<nsize; i++) {
        x1 = (double)i/((double)nsize-1);
        x2 = x1*std::sin(theta)/std::cos(theta) + e-1.0+x1;

        term1=std::cos(theta)*(df2(x1,x2)-e) - std::sin(theta)*x1;
        term2=std::sin(theta)*(df2(x1,x2)-e) + std::cos(theta)*x1;
        term2=b*pi<double>()*std::pow(term2,c);
        term2=std::abs(std::sin(term2));
        term2=a*std::pow(term2,d);

        diff=std::abs(term1-term2);
        if( (diff<tvalue) || (term1>term2) ) {
            TVector<double> sol;
            sol << x1 << x2;
            pof.append(sol);
            cout << x1 << " " << x2 << endl;
        }
    }
    */


    /// Pareto-optimal solutions
    for(auto sol : pof) {
        x1=sol.at(0);
        x2=sol.at(1);

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        QCOMPARE(imap->isEvaluated(), false);
        imap->evaluate();
        QCOMPARE(imap->isEvaluated(), true);
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), true);
        QCOMPARE(cVec[0]>threshVec[0], false);
    }

    /// Infeasible solutions that would be
    /// Pareto-optimal if constraint is removed
    nsol=100;
    for(int i=0; i<nsol; i++) {
        x1=((double)i+1)/(double)nsol; // skip zero
        x2=0.0;

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        imap->evaluate();
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), false);
        QCOMPARE(cVec[0]>threshVec[0], true);
    }
}

void tst_ctp::test_check_solutions_CTP4()
{
    cout.precision(13);

    double x1, x2;
    int nsize;
    TVector<double> cVec;

    /// Define the problem
    IFunctionSPtr func(new CTP4);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    /// tvalue is the precision (error) used to brute-force POF solutions
    /// in a computational feasible time
    double tvalue = 0.00105;
    TVector<double> threshVec(1, tvalue);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    IMappingSPtr imap(new IMapping(prob));

    /// Solutions found by brute-force
    TVector<TVector<double>> pof(13);
    pof[0]  << 0.0 << 0.0;
    pof[1]  << 0.08090170809017 << 0.02212317657439;
    pof[2]  << 0.1618034161803 << 0.04424635314877;
    pof[3]  << 0.2427051242705 << 0.06636952972316;
    pof[4]  << 0.3236068323607 << 0.08849270629755;
    pof[5]  << 0.4045085404509 << 0.1106158828719;
    pof[6]  << 0.485410148541 << 0.1327390321006;
    pof[7]  << 0.5663118566312 << 0.154862208675;
    pof[8]  << 0.6472135647214 << 0.1769853852493;
    pof[9]  << 0.7281152728115 << 0.1991085618237;
    pof[10]  << 0.8090169809017 << 0.2212317383981;
    pof[11]  << 0.8899186889919 << 0.2433549149725;
    pof[12]  << 0.970820397082 << 0.2654780915469;

    /*
    /// Brute-force the Pareto-optimal solutions

    /// Problem parameters
    double theta = -0.2*pi<double>();
    double a = 0.75;
    double b = 10.0;
    double c = 1.0;
    double d = 0.5;
    double e = 1.0;

    double term1, term2, diff;
    nsize=10000000;
    cout << endl;
    for(int i=0; i<nsize; i++) {
        x1 = (double)i/((double)nsize-1);
        x2 = x1*std::sin(theta)/std::cos(theta) + e-1.0+x1;

        term1=std::cos(theta)*(df2(x1,x2)-e) - std::sin(theta)*x1;
        term2=std::sin(theta)*(df2(x1,x2)-e) + std::cos(theta)*x1;
        term2=b*pi<double>()*std::pow(term2,c);
        term2=std::abs(std::sin(term2));
        term2=a*std::pow(term2,d);

        diff=std::abs(term1-term2);
        if( (diff<tvalue) || (term1>=term2) ) {
            TVector<double> sol;
            sol << x1 << x2;
            pof.append(sol);
            cout << x1 << " " << x2 << endl;
        }
    }
    */


    /// Pareto-optimal solutions
    for(int i=0; i<pof.size(); i++) {
        x1=pof[i].at(0);
        x2=pof[i].at(1);

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        QCOMPARE(imap->isEvaluated(), false);
        imap->evaluate();
        QCOMPARE(imap->isEvaluated(), true);
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), true);
        QCOMPARE(cVec[0]>threshVec[0], false);
    }

    /// Infeasible solutions that would be
    /// Pareto-optimal if constraint is removed
    nsize=100;
    for(int i=0; i<nsize; i++) {
        x1=((double)i+1)/(double)nsize; // skip zero
        x2=0.0;

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        imap->evaluate();
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), false);
        QCOMPARE(cVec[0]>threshVec[0], true);
    }
}

void tst_ctp::test_check_solutions_CTP5()
{
    cout.precision(13);

    double x1, x2;
    int nsize;
    TVector<double> cVec;

    /// Define the problem
    IFunctionSPtr func(new CTP5);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    /// tvalue is the precision (error) used to brute-force POF solutions
    /// in a computational feasible time
    double tvalue = 0.000001;
    TVector<double> threshVec(1, tvalue);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    IMappingSPtr imap(new IMapping(prob));

    /// Solutions found by brute-force
    TVector<TVector<double>> pof(15);
    pof[0]  << 0.0 << 0.0;
    pof[1]  << 1.0000001e-07 << 2.734574994735e-08;
    pof[2]  << 2.0000002e-07 << 5.46914998947e-08;
    pof[3]  << 3.0000003e-07 << 8.203724984205e-08;
    pof[4]  << 4.0000004e-07 << 1.093829997894e-07;
    pof[5]  << 5.0000005e-07 << 1.367287496257e-07;
    pof[6]  << 6.0000006e-07 << 1.640744995731e-07;
    pof[7]  << 7.0000007e-07 << 1.914202495204e-07;
    pof[8]  << 8.0000008e-07 << 2.187659994678e-07;
    pof[9]  << 9.0000009e-07 << 2.461117494151e-07;
    pof[10] << 1.0000001e-06 << 2.734574993625e-07;
    pof[11] << 1.10000011e-06 << 3.008032493098e-07;
    pof[12] << 1.20000012e-06 << 3.281489992572e-07;
    pof[13] << 1.30000013e-06 << 3.554947490935e-07;
    pof[14] << 1.40000014e-06 << 3.828404990408e-07;

    /*
    /// Brute-force the Pareto-optimal solutions

    /// Problem parameters
    double theta = -0.2*pi<double>();
    double a = 0.1;
    double b = 10.0;
    double c = 2.0;
    double d = 0.5;
    double e = 1.0;

    double term1, term2, diff;
    nsize=10000000;
    cout << endl;
    for(int i=0; i<nsize; i++) {
        x1 = (double)i/((double)nsize-1);
        x2 = x1*std::sin(theta)/std::cos(theta) + e-1.0+x1;

        term1=std::cos(theta)*(df2(x1,x2)-e) - std::sin(theta)*x1;
        term2=std::sin(theta)*(df2(x1,x2)-e) + std::cos(theta)*x1;
        term2=b*pi<double>()*std::pow(term2,c);
        term2=std::abs(std::sin(term2));
        term2=a*std::pow(term2,d);

        diff=std::abs(term1-term2);
        if( (diff<tvalue) || (term1>term2) ) {
            TVector<double> sol;
            sol << x1 << x2;
            pof.append(sol);
            cout << x1 << " " << x2 << endl;
        }
    }
    */

    /// Pareto-optimal solutions
    for(auto sol : pof) {
        x1=sol.at(0);
        x2=sol.at(1);

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        QCOMPARE(imap->isEvaluated(), false);
        imap->evaluate();
        QCOMPARE(imap->isEvaluated(), true);
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), true);
        QCOMPARE(cVec[0]>threshVec[0], false);
    }

    /// Infeasible solutions that would be
    /// Pareto-optimal if constraint is removed
    nsize=100;
    for(int i=0; i<nsize; i++) {
        x1=((double)i+1)/(double)nsize; // skip zero
        x2=0.0;

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        imap->evaluate();
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), false);
        QCOMPARE(cVec[0]>threshVec[0], true);
    }
}

void tst_ctp::test_check_solutions_CTP6()
{
    cout.precision(13);

    double x1, x2;
    int nsize;
    TVector<double> cVec;

    /// Define the problem
    IFunctionSPtr func(new CTP6);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    IMappingSPtr imap(new IMapping(prob));

    /// Solutions found by NSGA-II
    TVector<TVector<double>> pof(16);
    pof[0]  << 1.0      << 0.88127;
    pof[1]  << 0.93294  << 0.99973;
    pof[2]  << 0.93294  << 0.99973;
    pof[3]  << 1.0      << 0.88127;
    pof[4]  << 0.96789  << 0.93792;
    pof[5]  << 0.93448  << 0.9971;
    pof[6]  << 0.94648  << 0.97585;
    pof[7]  << 0.93964  << 0.98878;
    pof[8]  << 0.94972  << 0.97071;
    pof[9]  << 0.96209  << 0.9486;
    pof[10] << 0.96003  << 0.95184;
    pof[11] << 0.9983   << 0.88524;
    pof[12] << 0.9763   << 0.92324;
    pof[13] << 0.99509  << 0.88993;
    pof[14] << 0.93529  << 0.99622;
    pof[15] << 0.96869  << 0.93792;

    /// "Good" approximation of the Pareto-optimal solutions
    for(auto sol : pof) {
        x1=sol.at(0);
        x2=sol.at(1);

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        QCOMPARE(imap->isEvaluated(), false);
        imap->evaluate();
        QCOMPARE(imap->isEvaluated(), true);
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), true);
        QCOMPARE(cVec[0]>threshVec[0], false);
    }

    /// Infeasible solutions that would be
    /// Pareto-optimal if constraint is removed
    nsize=100;
    for(int i=0; i<nsize; i++) {
        x1=((double)i+1)/(double)nsize; // skip zero
        x2=0.0;

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        imap->evaluate();
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), false);
        QCOMPARE(cVec[0]>threshVec[0], true);
    }
}

void tst_ctp::test_check_solutions_CTP7()
{
    cout.precision(13);

    double x1, x2;
    int nsize;
    TVector<double> cVec;

    /// Define the problem
    IFunctionSPtr func(new CTP7);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    IMappingSPtr imap(new IMapping(prob));

    /// Solutions found by NSGA-II
    TVector<TVector<double>> pof(16);

    pof[0]   << 1.0        << 2.3761e-12;
    pof[1]   << 9.0574e-06 << 0.044876;
    pof[2]   << 9.0574e-06 << 0.044876;
    pof[3]   << 0.10557    << 6.272e-17;
    pof[4]   << 1.4648e-05 << 0.044755;
    pof[5]   << 0.9878     << 9.6529e-17;
    pof[6]   << 0.861      << 2.1031e-18;
    pof[7]   << 0.9878     << 9.6529e-17;
    pof[8]   << 0.81025    << 5.3087e-18;
    pof[9]   << 0.68819    << 1.8203e-17;
    pof[10]  << 0.861      << 1.7722e-18;
    pof[11]  << 0.63359    << 8.9208e-17;
    pof[12]  << 0.51489    << 7.5361e-17;
    pof[13]  << 0.81025    << 5.3087e-18;
    pof[14]  << 0.68819    << 1.8229e-17;
    pof[15]  << 0.63359    << 8.983e-17;

    /// "Good" approximation of the Pareto-optimal solutions
    for(auto sol : pof) {
        x1=sol.at(0);
        x2=sol.at(1);

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        QCOMPARE(imap->isEvaluated(), false);
        imap->evaluate();
        QCOMPARE(imap->isEvaluated(), true);
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), true);
        QCOMPARE(cVec[0]>threshVec[0], false);
    }

    /// Infeasible solutions that would be
    /// Pareto-optimal if constraint is removed
    nsize=100;
    for(int i=0; i<nsize; i++) {
        x1=0.05*((double)i)/((double)nsize-1.0);
        x2=0.0;

        imap->defineDecisionVar(0,IElement(x1));
        imap->defineDecisionVar(1,IElement(x2));
        imap->evaluate();
        cVec = imap->doubleConstraintVec();
        QCOMPARE(imap->isFeasible(), false);
        QCOMPARE(cVec[0]>threshVec[0], true);
    }
}
#endif

#ifdef TST_CTP_MAPPINGS

void tst_ctp::test_check_mappings_CTP1()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP1);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2 << 3;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(2, 0.0);
    TVector<ElementType> types(2, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP1
    TVector<double> a, b;
    a << 0.858 << 0.728;
    b << 0.541 << 0.295;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 2, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2) * std::exp(-x1/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double c1_expected = f2_expected - a[0]*std::exp(-b[0]*f1_expected);
        double c1_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c1_expected,c1_obtained), true);

        double c2_expected = f2_expected - a[1]*std::exp(-b[1]*f1_expected);
        double c2_obtained = (-1.0)*cVec[1];
        QCOMPARE(areDoublesEqual(c2_expected,c2_obtained), true);

        if( (c1_expected<0) || (c2_expected<0) ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

void tst_ctp::test_check_mappings_CTP2()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP2);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP2
    double theta = -0.2*pi<double>();
    double a = 0.2;
    double b = 10.0;
    double c = 1.0;
    double d = 6.0;
    double e = 1.0;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 1, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2)*(1.0-f1_expected/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double exp1 = std::cos(theta)*(f2_expected-e) - std::sin(theta)*f1_expected;
        double exp2 = std::sin(theta)*(f2_expected-e) + std::cos(theta)*f1_expected;
        exp2 = b*pi<double>()*std::pow(exp2,c);
        exp2 = std::abs(std::sin(exp2));
        exp2 = a*std::pow(exp2,d);

        double c_expected = exp1-exp2;
        double c_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c_expected,c_obtained), true);

        if( c_expected<0 ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

void tst_ctp::test_check_mappings_CTP3()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP3);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP3
    double theta = -0.2*pi<double>();
    double a = 0.1;
    double b = 10.0;
    double c = 1.0;
    double d = 0.5;
    double e = 1.0;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 1, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2)*(1.0-f1_expected/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double exp1 = std::cos(theta)*(f2_expected-e) - std::sin(theta)*f1_expected;
        double exp2 = std::sin(theta)*(f2_expected-e) + std::cos(theta)*f1_expected;
        exp2 = b*pi<double>()*std::pow(exp2,c);
        exp2 = std::abs(std::sin(exp2));
        exp2 = a*std::pow(exp2,d);

        double c_expected = exp1-exp2;
        double c_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c_expected,c_obtained), true);

        if( c_expected<0 ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

void tst_ctp::test_check_mappings_CTP4()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP4);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP4
    double theta = -0.2*pi<double>();
    double a = 0.75;
    double b = 10.0;
    double c = 1.0;
    double d = 0.5;
    double e = 1.0;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 1, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2)*(1.0-f1_expected/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double exp1 = std::cos(theta)*(f2_expected-e) - std::sin(theta)*f1_expected;
        double exp2 = std::sin(theta)*(f2_expected-e) + std::cos(theta)*f1_expected;
        exp2 = b*pi<double>()*std::pow(exp2,c);
        exp2 = std::abs(std::sin(exp2));
        exp2 = a*std::pow(exp2,d);

        double c_expected = exp1-exp2;
        double c_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c_expected,c_obtained), true);

        if( c_expected<0 ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

void tst_ctp::test_check_mappings_CTP5()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP5);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP5
    double theta = -0.2*pi<double>();
    double a = 0.1;
    double b = 10.0;
    double c = 2.0;
    double d = 0.5;
    double e = 1.0;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 1, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2)*(1.0-f1_expected/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double exp1 = std::cos(theta)*(f2_expected-e) - std::sin(theta)*f1_expected;
        double exp2 = std::sin(theta)*(f2_expected-e) + std::cos(theta)*f1_expected;
        exp2 = b*pi<double>()*std::pow(exp2,c);
        exp2 = std::abs(std::sin(exp2));
        exp2 = a*std::pow(exp2,d);

        double c_expected = exp1-exp2;
        double c_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c_expected,c_obtained), true);

        if( c_expected<0 ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

void tst_ctp::test_check_mappings_CTP6()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP6);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP6
    double theta = 0.1*pi<double>();
    double a = 40.0;
    double b = 0.5;
    double c = 1.0;
    double d = 2.0;
    double e = -2.0;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 1, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2)*(1.0-f1_expected/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double exp1 = std::cos(theta)*(f2_expected-e) - std::sin(theta)*f1_expected;
        double exp2 = std::sin(theta)*(f2_expected-e) + std::cos(theta)*f1_expected;
        exp2 = b*pi<double>()*std::pow(exp2,c);
        exp2 = std::abs(std::sin(exp2));
        exp2 = a*std::pow(exp2,d);

        double c_expected = exp1-exp2;
        double c_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c_expected,c_obtained), true);

        if( c_expected<0 ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

void tst_ctp::test_check_mappings_CTP7()
{
    int nSolutions = 1000;

    /// Define the problem
    IFunctionSPtr func(new CTP7);
    TVector<int> paramInd, objInd, cnstrInd;
    objInd << 0 << 1;
    cnstrInd << 2;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, paramInd, cnstrInd, objInd);
    prob->processProblemDefinition();

    TVector<double> threshVec(1, 0.0);
    TVector<ElementType> types(1, RealType);
    TVector<IElementSPtr> thVec = createIElementSPtrVector(threshVec, types);
    prob->defineThresholdVector(thVec);
    prob->processProblemDefinition();

    /// check if there is two variables bounds
    QCOMPARE(prob->boxConstraints()->lowerBounds().size() == 2, true);
    QCOMPARE(prob->boxConstraints()->upperBounds().size() == 2, true);

    PSetBase*          base = new PSetBase;
    IFormulation*      form = new IFormulation(base);
    RandomInit*        init = new RandomInit(form);
    Evaluator*         eval = new Evaluator(init);

    form->defineProblem(prob);
    init->TP_defineSetSize(nSolutions);
    eval->evaluate();

    ISet* solSet = eval->outputSet(0);
    QCOMPARE(solSet==nullptr, false);
    QCOMPARE(solSet->size(), nSolutions);

    /// constraint parameters given in Deb's EMO paper for CTP7
    double theta = -0.05*pi<double>();
    double a = 40.0;
    double b = 5.0;
    double c = 1.0;
    double d = 6.0;
    double e = 0.0;

    for(int i=0; i<nSolutions; i++) {
        QCOMPARE(solSet->at(i)->isEvaluated(), true);

        TVector<double> dVec = solSet->at(i)->doubleDecisionVec();
        QCOMPARE(dVec.size() == 2, true); // check the number of decision variables
        TVector<double> fVec = solSet->at(i)->doubleObjectiveVec();
        QCOMPARE(fVec.size() == 2, true); // check the number of objectives
        TVector<double> cVec = solSet->at(i)->doubleConstraintVec();
        QCOMPARE(cVec.size() == 1, true); // check the number of constraints

        double x1 = dVec[0];
        double x2 = dVec[1];

        double f1_expected = x1;
        double f1_obtained = fVec.at(0);
        QCOMPARE(areDoublesEqual(f1_expected, f1_obtained), true);

        double f2_expected = (1.0+x2)*(1.0-f1_expected/(1.0+x2));
        double f2_obtained = fVec.at(1);
        QCOMPARE(areDoublesEqual(f2_expected, f2_obtained), true);

        double exp1 = std::cos(theta)*(f2_expected-e) - std::sin(theta)*f1_expected;
        double exp2 = std::sin(theta)*(f2_expected-e) + std::cos(theta)*f1_expected;
        exp2 = b*pi<double>()*std::pow(exp2,c);
        exp2 = std::abs(std::sin(exp2));
        exp2 = a*std::pow(exp2,d);

        double c_expected = exp1-exp2;
        double c_obtained = (-1.0)*cVec[0];
        QCOMPARE(areDoublesEqual(c_expected,c_obtained), true);

        if( c_expected<0 ) {
            // solution is expected to be infeasible
            QCOMPARE(solSet->at(i)->isFeasible(), false);
        }
        else {
            // solution is expected to be feasible
            QCOMPARE(solSet->at(i)->isFeasible(), true);
        }
    }
}

#endif

QTEST_GUILESS_MAIN(tst_ctp)

#include "tst_ctp.moc"
