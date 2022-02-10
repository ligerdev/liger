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
#include <tigon/Tigon.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

//TESTED_COMPONENT=src/libs/tigon/....
class tst_paramconnect : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_formulateSimpleProblem();
    void test_formulateComplexProblem();
    void test_IMappingEvaluation();
};

void inline TCOMPARE(tribool actual, tribool expected)
{
    bool flag;

    if(actual.value == expected.value) {
        flag = true;
    } else {
        flag = false;
    }

    QCOMPARE(flag, true);
}

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

ProblemSPtr testProblem()
{
    int nInFunc1 = 5;
    int nOutFunc1 = 4;
    int nInFunc2 = 4;
    int nOutFunc2 = 2;

    IFunctionSPtr func1 = IFunctionSPtr(new ExampleFunction);
    IFunctionSPtr func2 = IFunctionSPtr(new ExampleFunction);

    func1->TP_defineNInputs(nInFunc1);
    func1->TP_defineNOutputs(nOutFunc1);

    func2->TP_defineNInputs(nInFunc2);
    func2->TP_defineNOutputs(nOutFunc2);

    TVector<ElementProperties> inPrpts1 = func1->inputPrpts();
    TVector<ElementProperties> outPrpts1 = func1->outputPrpts();
    TVector<ElementProperties> inPrpts2 = func2->inputPrpts();
    TVector<ElementProperties> outPrpts2 = func2->outputPrpts();

    // Change the names of the inputs and outputs for func1
    ElementPropertiesFactory* elemFact = ElementPropertiesFactory::instance();
    for(int i=0; i<nInFunc1; i++) {
        TString x = "x" + std::to_string(i+1);
        inPrpts1[i] = elemFact->createElementProperties(i, x, x);
    }
    func1->defineInputPrpts(inPrpts1);

    for(int i=0; i<nOutFunc1; i++) {
        TString f = "f" + std::to_string(i+1);
        outPrpts1[i] = elemFact->createElementProperties(i, f, f);
    }
    func1->defineOutputPrpts(outPrpts1);

    // Change the names of the inputs and outputs for func2
    inPrpts2[0] = elemFact->createElementProperties(0, "x1", "x1");
    inPrpts2[1] = elemFact->createElementProperties(1, "l2", "l2");
    inPrpts2[2] = elemFact->createElementProperties(2, "f3", "f3");
    inPrpts2[3] = elemFact->createElementProperties(3, "f4", "f4");
    func2->defineInputPrpts(inPrpts2);

    for(int i=0; i<nOutFunc2; i++) {
        TString fs = "fs" + std::to_string(i+1);
        outPrpts2[i] = elemFact->createElementProperties(i, fs, fs);
    }
    func2->defineOutputPrpts(outPrpts2);

    TVector<int> paramIdx;
    paramIdx << 2 << 3;

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func1);
    prob->appendFunction(func2, paramIdx);
    prob->processProblemDefinition();

    return prob;
}

ProblemSPtr testProblem2()
{
    int nInFunc1 = 5;
    int nOutFunc1 = 5;
    int nInFunc2 = 6;
    int nOutFunc2 = 2;

    IFunctionSPtr func1 = IFunctionSPtr(new ExampleFunction);
    IFunctionSPtr func2 = IFunctionSPtr(new ExampleFunction);

    func1->TP_defineNInputs(nInFunc1);
    func1->TP_defineNOutputs(nOutFunc1);

    func2->TP_defineNInputs(nInFunc2);
    func2->TP_defineNOutputs(nOutFunc2);

    TVector<ElementProperties> inPrpts1 = func1->inputPrpts();
    TVector<ElementProperties> outPrpts1 = func1->outputPrpts();
    TVector<ElementProperties> inPrpts2 = func2->inputPrpts();
    TVector<ElementProperties> outPrpts2 = func2->outputPrpts();

    // Change the names of the inputs and outputs for func1
    ElementPropertiesFactory* elemFact = ElementPropertiesFactory::instance();
    for(int i=0; i<nInFunc1; i++) {
        TString x = "x" + std::to_string(i+1);
        inPrpts1[i] = elemFact->createElementProperties(i, x, x);
    }
    func1->defineInputPrpts(inPrpts1);

    for(int i=0; i<nOutFunc1; i++) {
        TString f = "f" + std::to_string(i+1);
        outPrpts1[i] = elemFact->createElementProperties(i, f, f);
    }
    func1->defineOutputPrpts(outPrpts1);

    // Change the names of the inputs and outputs for func2
    inPrpts2[0] = elemFact->createElementProperties(0, "x1", "x1");
    inPrpts2[1] = elemFact->createElementProperties(1, "l2", "l2");
    inPrpts2[2] = elemFact->createElementProperties(2, "f2", "f2");
    inPrpts2[3] = elemFact->createElementProperties(3, "f3", "f3");
    inPrpts2[4] = elemFact->createElementProperties(4, "f4", "f4");
    inPrpts2[5] = elemFact->createElementProperties(5, "f5", "f5");
    func2->defineInputPrpts(inPrpts2);

    for(int i=0; i<nOutFunc2; i++) {
        TString y = "fs" + std::to_string(i+1);
        outPrpts2[i] = elemFact->createElementProperties(i, y, y);
    }
    func2->defineOutputPrpts(outPrpts2);

    TVector<int> constrIdx{2,3};
    TVector<int> objIdx{0,1,2};
    TVector<int> paramIdx{2,3,4,5};

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func1, TVector<int>{}, constrIdx, objIdx);
    prob->appendFunction(func2, paramIdx);
    prob->processProblemDefinition();

    // Maximize f2
    prob->redefineObjOptimizationType(1, OptimizationType::Maximization);
    prob->processProblemDefinition();

    return prob;
}

void tst_paramconnect::test_formulateSimpleProblem()
{
    ProblemSPtr prob = testProblem();
    ProblemDefinitionStatus stat = prob->isProblemDefined();
    QCOMPARE(stat, FullyDefined);

    TVector<bool> areParametersConnected = prob->paramConnectIsConnected();
    for(bool connect : areParametersConnected) {
        QCOMPARE(connect, false);
    }

    TVector<bool> extParam(areParametersConnected.size(), false);
    extParam[0] = true;
    prob->defineExternalParameters(extParam);
    stat = prob->processProblemDefinition();
    QCOMPARE(stat, FullyDefined);

    QCOMPARE(prob->paramConnectIsConnected(0), true);
    QCOMPARE(prob->paramConnectOutputIdx(0)==2, true); // third objective
    QCOMPARE(prob->paramConnectOutputType(0)==OutputType::Objective, true);
    QCOMPARE(prob->paramConnectIsConnected(1), false);

    extParam[1] = true;
    prob->defineExternalParameters(extParam);
    stat = prob->processProblemDefinition();
    QCOMPARE(stat, FullyDefined);

    QCOMPARE(prob->paramConnectIsConnected(0), true);
    QCOMPARE(prob->paramConnectOutputIdx(0)==2, true); // f3
    QCOMPARE(prob->paramConnectOutputType(0)==OutputType::Objective, true);

    QCOMPARE(prob->paramConnectIsConnected(1), true);
    QCOMPARE(prob->paramConnectOutputIdx(1)==3, true); // f4
    QCOMPARE(prob->paramConnectOutputType(1)==OutputType::Objective, true);
}

void tst_paramconnect::test_formulateComplexProblem()
{
    ProblemSPtr prob = testProblem2();
    ProblemDefinitionStatus stat = prob->isProblemDefined();
    QCOMPARE(stat, FullyDefined);

    TVector<bool> areParametersConnected = prob->paramConnectIsConnected();
    for(bool connect : areParametersConnected) {
        QCOMPARE(connect, false);
    }

    TVector<bool> extParam(areParametersConnected.size(), true);
    prob->defineExternalParameters(extParam);
    stat = prob->processProblemDefinition();
    QCOMPARE(stat, FullyDefined);

    // Objective
    QCOMPARE(prob->paramConnectIsConnected(0), true);
    QCOMPARE(prob->paramConnectOutputIdx(0)==1, true); // f2
    QCOMPARE(prob->paramConnectOutputType(0)==OutputType::Objective, true);
    QCOMPARE(prob->paramConnectIsMaximization(0), true);

    // Objective-Constraint (Objective takes precedence)
    QCOMPARE(prob->paramConnectIsConnected(1), true);
    QCOMPARE(prob->paramConnectOutputIdx(1)==2, true); // f3
    QCOMPARE(prob->paramConnectOutputType(1)==OutputType::Objective, true);
    QCOMPARE(prob->paramConnectIsMaximization(1), false);

    // Constraint
    QCOMPARE(prob->paramConnectIsConnected(2), true);
    QCOMPARE(prob->paramConnectOutputIdx(2)==1, true); // f4
    QCOMPARE(prob->paramConnectOutputType(2)==OutputType::Constraint, true);
    QCOMPARE(prob->paramConnectIsMaximization(2), false);

    // Unused
    QCOMPARE(prob->paramConnectIsConnected(3), true);
    QCOMPARE(prob->paramConnectOutputIdx(3)==0, true); // f5
    QCOMPARE(prob->paramConnectOutputType(3)==OutputType::Unused, true);
    QCOMPARE(prob->paramConnectIsMaximization(3), false);
}

void tst_paramconnect::test_IMappingEvaluation()
{
    {
        ProblemSPtr prob = testProblem();
        ProblemDefinitionStatus stat = prob->isProblemDefined();
        QCOMPARE(stat, FullyDefined);

        TVector<bool> extParam(prob->paramConnectIsConnected().size(), true);
        prob->defineExternalParameters(extParam);
        stat = prob->processProblemDefinition();
        QCOMPARE(stat, FullyDefined);

        IMapping imap(prob);
        for(size_t i=0; i<imap.decisionVec().size(); i++) {
            imap.defineDecisionVar(i, IElement(i*0.1));
        }
        imap.evaluate();
        TVector<IElementSPtr> param = imap.parameterVec();
        TVector<bool> paramConnect = prob->paramConnectIsConnected();

        for(size_t i=0; i<paramConnect.size(); i++) {
            if(prob->paramConnectIsConnected(i)) {
                auto idx = prob->paramConnectOutputIdx(i);
                switch(prob->paramConnectOutputType(i)) {
                case OutputType::Objective:
                    QCOMPARE(bool(*param[i] == *imap.objectiveVar(idx)), true);
                    break;
                case OutputType::Constraint:
                    QCOMPARE(bool(*param[i] == *imap.constraintVar(idx)), true);
                    break;
                case OutputType::Unused:
                    QCOMPARE(bool(*param[i] == *imap.unusedVar(idx)), true);
                    break;
                }
            }
        }
    }

    {
        ProblemSPtr prob = testProblem2();
        ProblemDefinitionStatus stat = prob->isProblemDefined();
        QCOMPARE(stat, FullyDefined);

        TVector<bool> extParam(prob->paramConnectIsConnected().size(), true);
        prob->defineExternalParameters(extParam);
        stat = prob->processProblemDefinition();
        QCOMPARE(stat, FullyDefined);

        IMapping imap(prob);
        for(size_t i=0; i<imap.decisionVec().size(); i++) {
            imap.defineDecisionVar(i, IElement(i*0.1));
        }
        imap.evaluate();
        TVector<IElementSPtr> param = imap.parameterVec();
        TVector<bool> paramConnect = prob->paramConnectIsConnected();

        for(size_t i=0; i<paramConnect.size(); i++) {
            if(prob->paramConnectIsConnected(i)) {
                auto idx = prob->paramConnectOutputIdx(i);
                switch(prob->paramConnectOutputType(i)) {
                case OutputType::Objective:
                    QCOMPARE(bool(*param[i] == *imap.objectiveVar(idx)), true);
                    break;
                case OutputType::Constraint:
                    QCOMPARE(bool(*param[i] == *imap.constraintVar(idx)), true);
                    break;
                case OutputType::Unused:
                    QCOMPARE(bool(*param[i] == *imap.unusedVar(idx)), true);
                    break;
                }
            }
        }
    }
}


QTEST_GUILESS_MAIN(tst_paramconnect)

#include "tst_paramconnect.moc"
