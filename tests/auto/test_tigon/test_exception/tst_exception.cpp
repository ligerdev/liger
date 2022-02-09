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

#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>

#define TST_EXCEPTION
#define WORK_FLOW
#define ALGORITHM

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_exception : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_EXCEPTION
    /* Add the necessary tests */
    void testPopulationSize();
    void testDecisionVariables();
    void testObjectiveFunctions();
    void test_DTLZ1_Problem();
    void test_DTLZ2_Problem();
    void test_outOfOutputBoundError();
#endif
};


void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(int i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

#ifdef TST_EXCEPTION

/// Exception handling for IInitialisation property
void tst_exception::testPopulationSize()
{
    int dVecSize = 5;
    int oVecSize = 3;
    int populationSize = -1;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    TString className = init->className();

    // invalid population size in IInitialisation::TP_defineSetSize(int sz)
    try {
        init->TP_defineSetSize(populationSize);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = className + ": " + DomainException;
        QCOMPARE(query,ans);
    }

    delete init;
    delete prob;
    delete base;
}

/// Exception handling for IMapping public functions
void tst_exception::testDecisionVariables()
{
    int dVecSize = 5;
    int oVecSize = 3;
    int populationSize = 20;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    init->evaluate();
    ISet* oSet = init->outputSet(0);
    IMappingSPtr imap = oSet->at(0); // first solution

    // invalid variable index in IMapping::doubleDecisionVar(int var)
    try {
        imap->doubleDecisionVar(-1);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // invalid variable index in IMapping::setDecisionVar(int idx, const IElement &val)
    IElement var = 0.1;
    try {
        imap->defineDecisionVar(-1,var);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // invalid variable index in IMapping::setDecisionVar(int idx, IElementSPtr val)
    IElementSPtr elemCorrect = imap->decisionVar(0);
    try {
        imap->defineDecisionVar(-1,elemCorrect);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // null IElementSPtr in IMapping::setDecisionVar(int idx, IElementSPtr val)
    IElementSPtr elemInvalid;
    try {
        imap->defineDecisionVar(0,elemInvalid);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + NullPointerException;

        QCOMPARE(query,ans);
    }

    // invalid decision vector size in IMapping::setDecisionVec(TVector<IElement> dvec)
    TVector<IElement> dvec_ielement(200);
    try {
        imap->defineDecisionVec(dvec_ielement);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // invalid decision vector size in IMapping::setDecisionVec(TVector<IElementSPtr> dvec)
    TVector<IElementSPtr> dvec_ilementsprt(200);
    try {
        imap->defineDecisionVec(dvec_ilementsprt);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    delete init;
    delete prob;
    delete base;
}

void tst_exception::testObjectiveFunctions()
{
    int dVecSize = 5;
    int oVecSize = 3;
    int populationSize = 20;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func = IFunctionSPtr(new DTLZ1);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator*       evaluator = new Evaluator(init);
    evaluator->TP_defineParallel(false);

    evaluator->evaluate();
    ISet* oSet = evaluator->outputSet(0);
    IMappingSPtr imap = oSet->at(0); // first solution

    // invalid variable index in IMapping::doubleObjectiveVar(int var)
    try {
        imap->doubleObjectiveVar(-1);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // invalid variable index in IMapping::defineObjectiveVar(int idx, const IElement &val)
    IElement var = 0.1;
    try {
        imap->defineObjectiveVar(-1,var);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // invalid variable index in IMapping::defineObjectiveVar(int idx, IElementSPtr val)
    IElementSPtr elemCorrect = imap->decisionVar(0);
    try {
        imap->defineObjectiveVar(-1,elemCorrect);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // null IElementSPtr in IMapping::defineObjectiveVar(int idx, IElementSPtr val)
    IElementSPtr elemInvalid;
    try {
        imap->defineObjectiveVar(0,elemInvalid);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + NullPointerException;

        QCOMPARE(query,ans);
    }

    // wrong ElementType in IMapping::defineObjectiveVar(int idx, IElementSPtr val)
    IElementSPtr elemInvalid2(new IElement(NominalType, 8));
    try {
        imap->defineObjectiveVar(0,elemInvalid2);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + InvalidTypeException;

        QCOMPARE(query,ans);
    }

    // invalid decision vector size in IMapping::defineObjectiveVec(TVector<IElement> ovec)
    TVector<IElement> ovec_ielement(200);
    try {
        imap->defineObjectiveVec(ovec_ielement);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    // invalid decision vector size in IMapping::defineObjectiveVec(TVector<IElementSPtr> ovec)
    TVector<IElementSPtr> ovec_ilementsprt(200);
    try {
        imap->defineObjectiveVec(ovec_ilementsprt);
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = IMapping::className() + ": " + DomainException;

        QCOMPARE(query,ans);
    }

    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

// Constraints in IMapping

/// Exception handling specific for test problems
void tst_exception::test_DTLZ1_Problem()
{
    int dVecSize = 1;
    int oVecSize = 3;
    int populationSize = 20;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    DTLZ1* prn = new DTLZ1();
    TString problemClassName = prn->className();
    IFunctionSPtr func = IFunctionSPtr(prn);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator*       evaluator = new Evaluator(init);
    evaluator->TP_defineParallel(false);

    // invalid number of decision variables in DTLZ1::evaluate(...)
    try {
        evaluator->evaluate();
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = problemClassName + ": " + IncorrectProblemFormatException;

        QCOMPARE(query,ans);
    }

    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_exception::test_DTLZ2_Problem()
{
    int dVecSize = 1;
    int oVecSize = 3;
    int populationSize = 20;

    PSetBase*     base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    DTLZ2* prn = new DTLZ2();
    TString problemClassName = prn->className();
    IFunctionSPtr func = IFunctionSPtr(prn);
    func->TP_defineNInputs(dVecSize);
    func->TP_defineNOutputs(oVecSize);
    prob->appendFunction(func);

    RandomInit*           init      = new RandomInit(prob);
    init->TP_defineSetSize(populationSize);

    Evaluator*       evaluator = new Evaluator(init);
    evaluator->TP_defineParallel(false);

    // invalid number of decision variables in DTLZ2::evaluate(...)
    try {
        evaluator->evaluate();
    }
    catch(TException &e) {
        TString query = TString(e.what());
        TString ans = problemClassName + ": " + IncorrectProblemFormatException;

        QCOMPARE(query,ans);
    }

    delete evaluator;
    delete init;
    delete prob;
    delete base;
}

void tst_exception::test_outOfOutputBoundError()
{
    const int lb = 0;
    const int ub = 9;

    // Define a test function
    class TstFunc : public IFunction
    {
    public:
        TstFunc()
        {
            TP_defineNInputs(1);
            TP_defineNOutputs(2);
            TString desc("Some evaluations are out of bound");
            createFunctionProperties("Test Function", desc);
        }

        void evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs)
        {
            if(inputs.size() != 1 || outputs.size() != 2) {
                return;
            }

            outputs[0]->defineValue(*inputs[0] + 1);
            outputs[1]->defineValue(*inputs[0] - 1);
        }

        void defineInputPrpts()
        {
            TStringList          varNames;
            TStringList          varDescriptions;
            TVector<ElementType> typeVec(TP_nInputs(), IntegerType);
            TStringList          varUnits;
            TVector<IElement>    lowerBounds(TP_nInputs(), IElement(IntegerType, lb));
            TVector<IElement>    upperBounds(TP_nInputs(), IElement(IntegerType, ub));

            for(int i = 0; i < TP_nInputs(); i++) {
                varNames.push_back("Input_Var_" + std::to_string(i));
                varDescriptions.push_back("Input_VarDesc_" + std::to_string(i));
                varUnits.push_back("");
            }

            createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                                  lowerBounds, upperBounds);
        }

        void defineOutputPrpts()
        {
            TStringList          varNames;
            TStringList          varDescriptions;
            TVector<ElementType> typeVec(TP_nOutputs(), IntegerType);
            TStringList          varUnits;
            TVector<OptimizationType>
                                 optTypeVec(TP_nOutputs(), Minimization);

            for(int i = 0; i < TP_nOutputs(); i++) {
                varNames.push_back("Output_Var_" + std::to_string(i));
                varDescriptions.push_back("Output_VarDesc_" + std::to_string(i));
                varUnits.push_back("");
            }
            createOutputProperties(varNames, varDescriptions, typeVec, varUnits,
                                   optTypeVec);
        }
    };

    // Define a test function
    class TstFunc2 : public IFunction
    {
    public:
        TstFunc2()
        {
            TP_defineNInputs(1);
            TP_defineNOutputs(2);
            TString desc("Some evaluations are out of bound 2");
            createFunctionProperties("Test Function2", desc);
        }

        void evaluate(const TVector<IElementSPtr> &inputs, const TVector<IElementSPtr> &outputs)
        {
            if(inputs.size() != 1 || outputs.size() != 2) {
                return;
            }

            outputs[0]->defineValue(*inputs[0] + 2);
            outputs[1]->defineValue(*inputs[0] - 2);
        }

        void defineInputPrpts()
        {
            TStringList          varNames;
            TStringList          varDescriptions;
            TVector<ElementType> typeVec(TP_nInputs(), IntegerType);
            TStringList          varUnits;
            TVector<IElement>    lowerBounds(TP_nInputs(), IElement(IntegerType, lb));
            TVector<IElement>    upperBounds(TP_nInputs(), IElement(IntegerType, ub));

            for(int i = 0; i < TP_nInputs(); i++) {
                varNames.push_back("Input_Var_" + std::to_string(i));
                varDescriptions.push_back("Input_VarDesc_" + std::to_string(i));
                varUnits.push_back("");
            }

            createInputProperties(varNames, varDescriptions, typeVec, varUnits,
                                  lowerBounds, upperBounds);
        }

        void defineOutputPrpts()
        {
            TStringList          varNames;
            TStringList          varDescriptions;
            TVector<ElementType> typeVec(TP_nOutputs(), IntegerType);
            TStringList          varUnits;
            TVector<OptimizationType>
                                 optTypeVec(TP_nOutputs(), Minimization);

            for(int i = 0; i < TP_nOutputs(); i++) {
                varNames.push_back("Output2_Var_" + std::to_string(i));
                varDescriptions.push_back("Output2_VarDesc_" + std::to_string(i));
                varUnits.push_back("");
            }
            createOutputProperties(varNames, varDescriptions, typeVec, varUnits,
                                   optTypeVec);
        }
    };

    IFunctionSPtr func(new TstFunc);
    TVector<IElementSPtr> inputs  = createIElementSPtrVector(1, IntegerType);
    TVector<IElementSPtr> outputs = createIElementSPtrVector(2, IntegerType);

    // Valid output
    inputs[0]->defineValue(lb + 1);
    func->evaluate(inputs, outputs);

    /// Test the exception handling in Evaluator
    bool caughtExeption(false);
    TString noError("No exception raised");
    TVector<TVector<double> > dVecs;
    for(int val = lb; val <= ub; val++) {
        dVecs.push_back(TVector<double>(1, (double)val));
    }

    {
        PSetBase*        base = new PSetBase;
        IFormulation*    prob = new IFormulation(base);
        UserDefinedInit* init = new UserDefinedInit(prob);
        Evaluator*       eval = new Evaluator(init);

        prob->appendFunction(func);

        // First only generate valid outputs
        TString errMsg(noError);
        init->defineInitialDVecs(mid(dVecs, 1, dVecs.size()-2));
        try{
            eval->evaluate();
        } catch(TException &e) {
            caughtExeption = true;
            errMsg = TString(e.what());
        }

        ISet* solSet = eval->outputSet(0);
        QCOMPARE(solSet == nullptr, false);
        QCOMPARE(solSet->size() == ub - lb - 1, true);
        QCOMPARE(caughtExeption, false);
        QCOMPARE(errMsg, noError);

        delete eval;
        delete init;
        delete prob;
        delete base;
    }


    {
        PSetBase*        base = new PSetBase;
        IFormulation*    prob = new IFormulation(base);
        UserDefinedInit* init = new UserDefinedInit(prob);
        Evaluator*       eval = new Evaluator(init);

        prob->appendFunction(func);

        // Now generate an invalid output
        TString errMsg(noError);
        init->defineInitialDVecs(dVecs);
        try{
            eval->evaluate();
        } catch(TException &e) {
            caughtExeption = true;
            errMsg = TString(e.what());
        }
        ISet* solSet = eval->outputSet(0);
        QCOMPARE(solSet == nullptr, false);
        QCOMPARE(solSet->size(), ub - lb + 1);

        delete eval;
        delete init;
        delete prob;
        delete base;
    }
}

#endif

QTEST_GUILESS_MAIN(tst_exception)

#include "tst_exception.moc"
