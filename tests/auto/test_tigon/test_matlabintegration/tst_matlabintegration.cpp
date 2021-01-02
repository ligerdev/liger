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

#include <iostream>
#include <tigon/Tigon.h>

#include<matlabplugin/Representation/Functions/Matlab/MatlabFunction.h>
#include<matlabplugin/Utils/MatlabPool.h>
#include<matlabplugin/Utils/MatlabEngine.h>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

class tst_matlabintegration : public QObject
{
    Q_OBJECT

public:

private slots:
    void initTestCase();
    void test_definePath();
    void test_matlabEngine();
    void test_matlabWrapper1();
    void test_matlabProperties();
    void test_matlabWrapper2();
    void test_twoFunctions();
    void test_badCommand();
    void test_dtlz1();
    void test_mergeAndMix();
    void test_loadMatlabFunctionFromProblemGenerator();
    void test_loadMatlabFunctionFromWorkflow();
    void test_matlabFunctionEvaluation();
    void test_matlabFunctionBatchEvaluation();
    void test_matlabEvaluatorCountError();
    void test_matlabEvaluator();
    void test_cleanup();

private:
    TString m_pluginPath;
    TString m_testBinPath;
};

void tst_matlabintegration::initTestCase()
{
    m_testBinPath = QCoreApplication::applicationDirPath().toStdString();

    QDir cpath = QCoreApplication::applicationDirPath();
    cpath.cdUp();
    cpath.cdUp();
#ifdef __APPLE__
    cpath.cd("bin/Liger.app/Contents/TigonPlugIns");
#else
    cpath.cd("lib/tigon/plugins");
#endif

    m_pluginPath = cpath.path().toStdString();

    cout << m_pluginPath  << endl;
    PluginManager::instance()->definePath(m_pluginPath);
    PluginManager::instance()->loadPlugins();

    TStringList funcs = TigonFunctionFactory::instance()->registeredFunctions();
    QCOMPARE(vectorContains(funcs, TString("Tigon::Representation::MatlabFunction")), true);
}

void tst_matlabintegration::test_definePath()
{
    // Check nonexisting path
    TString path("/home/path/that/does/not/exist.m");
    IFunction* func = TigonFunctionFactory::instance()->createFunction(
                "Tigon::Representation::MatlabFunction");

    QCOMPARE(func->className(), TString("Tigon::Representation::MatlabFunction"));

    func->definePath(path);
    QCOMPARE(func->path().empty(), true);
    QCOMPARE(static_cast<MatlabFunction*>(func)->command().empty(), true);

    // Check existing path
    path = m_testBinPath + "/test_properties_parsing.m";
    func->definePath(path);

    // Check fucntion properties
    QCOMPARE(func->path(), path);
    QCOMPARE(static_cast<MatlabFunction*>(func)->command(), TString("test_properties_parsing"));
    QCOMPARE(func->type(), FunctionType::Matlab);
    QCOMPARE(func->name(), TString("TestHeader"));
    QCOMPARE(func->description(), TString("Matlab Header Test function"));

    // Check input properties
    QCOMPARE(func->inputPrpts().size() == 2, true);

    QCOMPARE(func->inputPrpts().at(0).name(), TString("a"));
    QCOMPARE(func->inputPrpts().at(1).name(), TString("b"));
    QCOMPARE(func->inputPrpts().at(0).description(), TString("a value"));
    QCOMPARE(func->inputPrpts().at(1).description(), TString("another value"));
    QCOMPARE(func->inputPrpts().at(0).type(), RealType);
    QCOMPARE(func->inputPrpts().at(1).type(), RealType);

    QCOMPARE(func->boxConstraints()->lowerBound(0).value(), 0.0);
    QCOMPARE(func->boxConstraints()->lowerBound(1).value(), 0.0);
    QCOMPARE(func->boxConstraints()->upperBound(0).value(), 1.0);
    QCOMPARE(func->boxConstraints()->upperBound(1).value(), 1.0);

    // Check output properties
    QCOMPARE(func->outputPrpts().size() == 2, true);

    QCOMPARE(func->outputPrpts().at(0).name(), TString("out1"));
    QCOMPARE(func->outputPrpts().at(1).name(), TString("out2"));
    QCOMPARE(func->outputPrpts().at(0).description(), TString("The first output"));
    QCOMPARE(func->outputPrpts().at(1).description(), TString("The second output"));
    QCOMPARE(func->outputPrpts().at(0).type(), RealType);
    QCOMPARE(func->outputPrpts().at(1).type(), RealType);

    delete func;
}

void tst_matlabintegration::test_matlabEngine()
{
    MatlabEngine* eng = MatlabPool::getInstance().aquireEngine();

    {

        cout << "Testing single value access functions..." << endl;

        eng->placeVariable("TestInt", 5);
        int testint=0;
        eng->getWorkspaceVariable("TestInt", testint);
        QCOMPARE(testint, 5);

        eng->placeVariable("TestReal", 10.25);
        double testreal = 0.0;
        eng->getWorkspaceVariable("TestReal", testreal);
        QCOMPARE(testreal, 10.25);

        eng->placeVariable("TestBool", true);
        bool testbool = false;
        eng->getWorkspaceVariable("TestBool", testbool);
        QCOMPARE(testbool, true);

        eng->placeVariable("TestString", TString("test"));  //Must explicitly type the string literal otherwise the compiler default to the bool version
        TString teststring = "";
        eng->getWorkspaceVariable("TestString", teststring);
        QCOMPARE(teststring, TString("test"));

        TComplex testin;
        testin.real(10.0);
        testin.imag(5.0);
        eng->placeVariable("TestComplex", testin);
        TComplex testcomplex;
        eng->getWorkspaceVariable("TestComplex", testcomplex);
        QCOMPARE(testcomplex.real(), testin.real());
        QCOMPARE(testcomplex.imag(), testin.imag());
    }

    {

        cout << "Testing engine evaluation..." << endl;
        cout << "Evaluating: etest = 1 + 2" << endl;

        bool engState = eng->evaluateString("etest = 1 + 2");
        QCOMPARE(engState, true);

        int engOut = 0;
        eng->getWorkspaceVariable("etest", engOut);
        QCOMPARE(engOut, 3);

        cout << "Testing Error handling..." << endl;

        engState = eng->evaluateString("error('Test')");
        QCOMPARE(engState, false);
    }

    {
        // Tests loosley defined variables are pulled out of MATLAB with the correct type.
        // Variables defined by textual commands will not have the strict type structure imposed by
        // MatlabEngine placeVariable functions.

        cout << "Testing loosely defined variables..." << endl;
        cout << "Writing: a=1" << endl;

        //        bool engState = eng->evaluateString("a = 1");

        //        QCOMPARE(engState, true);

        eng->placeVariable("a", 1.0);
        //mxArray* amx = NULL;
        //eng->getWorkspaceVariable("a", amx);

        double a = 0.0;
        eng->getWorkspaceVariable("a", a);

        cout << "Reading as a double: a = " << a << endl;

        QCOMPARE(a, 1.0);

        int inta = 0;
        eng->getWorkspaceVariable("a", inta);

        cout << "Reading as an int: a = " << inta << endl;

        QCOMPARE(inta, 1);

        bool boola = false;
        eng->getWorkspaceVariable("a", boola);

        cout << "Reading as a bool: a = " << boola << endl;

        QCOMPARE(boola, true);

        TComplex cmplxa(0,0);
        eng->getWorkspaceVariable("a", cmplxa);

        cout << "Reading as a complex: a = " << cmplxa.real() << "+" << cmplxa.imag() << "j" << endl;

        QCOMPARE(cmplxa, TComplex(1,0));
    }

    {

        cout << "Testing vector access functions..." << endl;
        cout << "writing to MATLAB workspace: [0,1,2,3,4,5]" << endl;

        eng->evaluateString("testVec=[0,1,2,3,4,5]");
        TVector<double> vecReal;
        TVector<int> vecInt;
        TVector<bool> vecBool;
        TVector<TComplex> vecComplex;

        TVector<double> testReal;
        testReal << 0 << 1 << 2 << 3 << 4 << 5;
        TVector<int> testInt;
        testInt << 0 << 1 << 2 << 3 << 4 << 5;
        TVector<bool> testBool;
        testBool << false << true << true << true << true << true;
        TVector<TComplex> testComplex;
        testComplex << TComplex(1,1) << TComplex(2,2) << TComplex(3,3);

        eng->getWorkspaceVariable("testVec", vecReal);
        QCOMPARE(vecReal, testReal);

        cout << "Real vector from workspace: ";
        cout << vecReal;

        eng->placeVectorColumn("vecRealCol", testReal); // place the test vector onto the workspace
        eng->getWorkspaceVariable("vecRealCol", vecReal); // try to retreive it intact
        QCOMPARE(vecReal, testReal);
        eng->placeVectorRow("vecRealRow", testReal);
        eng->getWorkspaceVariable("vecRealRow", vecReal);
        QCOMPARE(vecReal, testReal);

        eng->getWorkspaceVariable("testVec", vecInt);
        QCOMPARE(vecInt, testInt);

        cout << "Int vector from workspace: ";
        cout << vecInt;

        eng->placeVectorColumn("vecIntCol", testInt);
        eng->getWorkspaceVariable("vecIntCol", vecInt);
        QCOMPARE(vecInt, testInt);
        eng->placeVectorRow("vecIntRow", testInt);
        eng->getWorkspaceVariable("vecIntRow", vecInt);
        QCOMPARE(vecInt, testInt);

        eng->getWorkspaceVariable("testVec", vecBool);
        QCOMPARE(vecBool, testBool);

        cout << "Bool vector from workspace: ";
        cout << vecBool;

        eng->placeVectorColumn("vecBoolCol", testBool);
        eng->getWorkspaceVariable("vecBoolCol", vecBool);
        QCOMPARE(vecBool, testBool);
        eng->placeVectorRow("vecBoolRow", testBool);
        eng->getWorkspaceVariable("vecBoolRow", vecBool);
        QCOMPARE(vecBool, testBool);

        eng->placeVectorColumn("vecComplexCol", testComplex);
        eng->getWorkspaceVariable("vecComplexCol", vecComplex);
        QCOMPARE(vecComplex, testComplex);
        eng->placeVectorRow("vecComplexRow", testComplex);
        eng->getWorkspaceVariable("vecComplexRow", vecComplex);
        QCOMPARE(vecComplex, testComplex);

        eng->resetEngine();
    }

    {

        cout << "Tesing matrix access functions..." << endl;
        cout << "writing to MATLAB workspace: [0 1 2 3; 4 5 6 7; 8 9 10 11]" << endl;
        cout << "Testing matrix access from Text input" << endl;

        eng->evaluateString("testMat = [0 1 2 3; 4 5 6 7; 8 9 10 11]");
        TVector<TVector<int>> matInt;
        TVector<TVector<double>> matReal;
        TVector<TVector<int>> matIntTest;
        TVector<TVector<double>> matRealTest;

        matIntTest.resize(3);
        matRealTest.resize(3);

        matIntTest[0] << 0 << 1 << 2 << 3;
        matIntTest[1] << 4 << 5 << 6 << 7;
        matIntTest[2] << 8 << 9 << 10 << 11;

        matRealTest[0] << 0 << 1 << 2 << 3;
        matRealTest[1] << 4 << 5 << 6 << 7;
        matRealTest[2] << 8 << 9 << 10 << 11;

        TVector<int> dim = eng->getWorkspaceVariable("testMat", matReal);
        QCOMPARE(matReal, matRealTest);

        cout << "Real matrix from MATLAB:" << endl;
        for(int r = 0; r < dim[0]; r++) {
            for(int c = 0; c < dim[1]; c++) {
                cout << matReal[r][c] << "\t";
            }
            cout << endl;
        }
        cout << endl;

        dim = eng->getWorkspaceVariable("testMat", matInt);
        QCOMPARE(matInt, matIntTest);

        cout << "Integer matrix from MATLAB:" << endl;
        for(int r = 0; r < dim[0]; r++) {
            for(int c = 0; c < dim[1]; c++) {
                cout << matInt[r][c] << "\t";
            }
            cout << endl;
        }
        cout << endl;

        cout << "Testing input/output of matrices..." << endl;

        TVector<TVector<double>> matRealIn;
        TVector<TVector<double>> matRealOut;
        matRealIn.resize(3);
        matRealIn[0] << 0 << 1 << 2 << 3 << 4;
        matRealIn[1] << 5 << 6 << 7 << 8 << 9;
        matRealIn[2] << 10 << 11 << 12 << 13 << 14;

        eng->placeMatrix("testMatInput", matRealIn);
        dim = eng->getWorkspaceVariable("testMatInput", matRealOut);
        QCOMPARE(matRealOut, matRealIn);

        TVector<TVector<bool>> matBoolIn;
        TVector<TVector<bool>> matBoolOut;
        matBoolIn.resize(2);
        matBoolIn[0] << true << false;
        matBoolIn[1] << false << true;

        eng->placeMatrix("testMatInput", matBoolIn);
        eng->getWorkspaceVariable("testMatInput", matBoolOut);
        QCOMPARE(matBoolIn, matBoolOut);

        TVector<TVector<TComplex>> matComplexIn;
        TVector<TVector<TComplex>> matComplexOut;
        matComplexIn.resize(2);
        matComplexIn[0] << TComplex(0,0) << TComplex(1,1) << TComplex(2,2);
        matComplexIn[1] << TComplex(3,3) << TComplex(4,4) << TComplex(5,5);

        eng->placeMatrix("testComplexMat", matComplexIn);
        eng->getWorkspaceVariable("testComplexMat", matComplexOut);
        QCOMPARE(matComplexIn, matComplexOut);
        eng->resetEngine();
    }

    // Clean up
    MatlabPool::getInstance().releaseEngine(eng);
}

void tst_matlabintegration::test_matlabWrapper1()
{
    TVector<IElementSPtr> inputs = createIElementSPtrVector(2, ElementType::RealType);
    inputs[0]->defineValue(1);
    inputs[1]->defineValue(5);
    TVector<IElementSPtr> outputs = createIElementSPtrVector(2, ElementType::RealType);

    MatlabFunction* func = new MatlabFunction();
    func->definePath(m_testBinPath + "/test1_matlabTigonWrapper.m");

    // Test the evaluation of the function
    func->evaluate(inputs, outputs);

    QCOMPARE(outputs[0]->value<int>(), 6); // Input1 + Input2
    QCOMPARE(outputs[1]->value<int>(), 2); // Input1 + 1

    TVector<ElementProperties> inputProperties = func->inputPrpts();
    TVector<ElementProperties> outputProperties = func->outputPrpts();

    QCOMPARE(inputProperties.size() == 2, true);
    QCOMPARE(outputProperties.size() == 2, true);

    cout << "Input Properties: " << endl;
    for(int i = 0; i < inputProperties.size(); i++) {
        cout << "Variable " << std::to_string(i) << endl;
        cout << "Name: " << inputProperties[i].name() << endl;
        cout << "Description: " << inputProperties[i].description() << endl;
        cout << "Unit: " << inputProperties[i].unit() << endl << endl;
    }

    cout << endl << "Output Properties: " << endl;
    for(int i = 0; i < outputProperties.size(); i++) {
        cout << "Variable " << std::to_string(i) << endl;
        cout << "Name: " << outputProperties[i].name() << endl;
        cout << "Description: " << outputProperties[i].description() << endl;
        cout << "Unit: " << outputProperties[i].unit() << endl << endl;
    }

    delete func;
}

void tst_matlabintegration::test_matlabProperties()
{

    // This test aims to confirm the correct passing of parameter properties from matlab wrapper scripts to Tigon.

    {

        cout << "Testing properties: test2_matlabTigonWrapper" << endl << endl;

        MatlabFunction* fun = new MatlabFunction();
        QCOMPARE(fun->command(), TString(""));

        fun->definePath(m_testBinPath + "/test2_matlabTigonWrapper.m");
        QCOMPARE(fun->command(), TString("test2_matlabTigonWrapper"));

        TVector<ElementProperties> inputProperties = fun->inputPrpts();
        TVector<ElementProperties> outputProperties = fun->outputPrpts();

        // Test the property size, does it match the properties in the matlab file and at the two measures consistent?
        QCOMPARE(inputProperties.size() == 2, true);
        QCOMPARE(outputProperties.size() == 2, true);
        QCOMPARE(fun->TP_nInputs(), 2);
        QCOMPARE(fun->TP_nOutputs(), 2);

        TStringList InputNames;
        InputNames << "intIn" << "realIn";
        TStringList OutputNames;
        OutputNames << "intOut" << "realOut";

        TStringList descriptions;
        descriptions << "Integer" << "Real";
        TStringList units;
        units << "mm" << "mm";
        TVector<ElementType> types;
        types << ElementType::IntegerType << ElementType::RealType;
        TVector<double> upperBounds;
        TVector<double> lowerBounds;
        upperBounds << 1000 << 30.6;
        lowerBounds << -100 << -20;

        // Test input and output propeties match those in the M-file.
        for(int i=0; i<fun->TP_nInputs(); i++) {
            QCOMPARE(inputProperties[i].name(), InputNames[i]);
            QCOMPARE(inputProperties[i].description(), descriptions[i]);
            QCOMPARE(inputProperties[i].unit(), units[i]);
            QCOMPARE(inputProperties[i].type(), types[i]);
        }

        for(int i=0; i<fun->TP_nOutputs(); i++) {
            QCOMPARE(outputProperties[i].name(), OutputNames[i]);
            QCOMPARE(outputProperties[i].description(), descriptions[i]);
            QCOMPARE(outputProperties[i].unit(), units[i]);
            QCOMPARE(outputProperties[i].type(), types[i]);
        }

        //Test that the bounding box is constrcuted properly.
        for(int i=0; i<fun->TP_nInputs(); i++) {
            QCOMPARE(upperBounds[i], fun->boxConstraints()->upperBound(i).value<double>());
            QCOMPARE(lowerBounds[i], fun->boxConstraints()->lowerBound(i).value<double>());
        }


        // Write the properties to the console
        cout << "Input Properties: " << endl;
        for(int i = 0; i < inputProperties.size(); i++) {
            cout << "Variable " << std::to_string(i) << endl;
            cout << "Name: " << inputProperties[i].name() << endl;
            cout << "Description: " << inputProperties[i].description() << endl;
            cout << "Unit: " << inputProperties[i].unit() << endl;
            cout << "Upper Bound: " << fun->boxConstraints()->upperBound(i).value<double>() << endl;
            cout << "Lower Bound: " << fun->boxConstraints()->lowerBound(i).value<double>() << endl << endl;
        }

        cout << endl << "Output Properties: " << endl;
        for(int i = 0; i < outputProperties.size(); i++) {
            cout << "Variable " << std::to_string(i) << endl;
            cout << "Name: " << outputProperties[i].name() << endl;
            cout << "Description: " << outputProperties[i].description() << endl;
            cout << "Unit: " << outputProperties[i].unit() << endl << endl;
        }

        delete fun;
    }

    {

        cout << "Testing properties: test1_matlabTigonWrapper" << endl << endl;

        MatlabFunction* func = new MatlabFunction();
        func->definePath(m_testBinPath + "/test1_matlabTigonWrapper.m");
        QCOMPARE(func->command(), TString("test1_matlabTigonWrapper"));

        TVector<ElementProperties> inputProperties = func->inputPrpts();
        TVector<ElementProperties> outputProperties = func->outputPrpts();

        QCOMPARE(inputProperties.size() == 2, true);
        QCOMPARE(outputProperties.size() == 2, true);
        QCOMPARE(func->TP_nInputs(), 2);
        QCOMPARE(func->TP_nOutputs(), 2);

        TStringList InputNames;
        InputNames << "a" << "b";
        TStringList InputDescriptions;
        InputDescriptions << "A value" << "Another value";
        TStringList InputUnits;
        InputUnits << "mm" << "mm";

        TVector<ElementType> types;
        types << ElementType::RealType << ElementType::RealType;

        TStringList OutputNames;
        OutputNames << "out1" << "out2";
        TStringList OutputDescriptions;
        OutputDescriptions << "The first output" << "The second output";
        TStringList OutputUnits;
        OutputUnits << "mm" << "mm";

        for(int i=0; i<func->TP_nInputs(); i++) {
            QCOMPARE(inputProperties[i].name(), InputNames[i]);
            QCOMPARE(inputProperties[i].description(), InputDescriptions[i]);
            QCOMPARE(inputProperties[i].unit(), InputUnits[i]);
            QCOMPARE(inputProperties[i].type(), types[i]);
        }

        for(int i=0; i<func->TP_nOutputs();i++) {
            QCOMPARE(outputProperties[i].name(), OutputNames[i]);
            QCOMPARE(outputProperties[i].description(), OutputDescriptions[i]);
            QCOMPARE(outputProperties[i].unit(), OutputUnits[i]);
            QCOMPARE(outputProperties[i].type(), types[i]);
        }


        // Write the properties to the console
        cout << "Input Properties: " << endl;
        for(int i = 0; i < inputProperties.size(); i++) {
            cout << "Variable " << std::to_string(i) << endl;
            cout << "Name: " << inputProperties[i].name() << endl;
            cout << "Description: " << inputProperties[i].description() << endl;
            cout << "Unit: " << inputProperties[i].unit() << endl << endl;
        }

        cout << endl << "Output Properties: " << endl;
        for(int i = 0; i < outputProperties.size(); i++) {
            cout << "Variable " << std::to_string(i) << endl;
            cout << "Name: " << outputProperties[i].name() << endl;
            cout << "Description: " << outputProperties[i].description() << endl;
            cout << "Unit: " << outputProperties[i].unit() << endl << endl;
        }

        delete func;
    }

}

void tst_matlabintegration::test_matlabWrapper2()
{

    // Test the second wrapper test function

    MatlabFunction* func = new MatlabFunction();
    func->definePath(m_testBinPath + "/test2_matlabTigonWrapper.m");

    IElementSPtr in1(new IElement(ElementType::IntegerType, 1));
    IElementSPtr in2(new IElement(ElementType::RealType, 2.0));

    IElementSPtr out1(new IElement(ElementType::IntegerType));
    IElementSPtr out2(new IElement(ElementType::RealType));

    TVector<IElementSPtr> inputElem;
    inputElem << in1 << in2;
    TVector<IElementSPtr> outputElem;
    outputElem << out1 << out2;

    func->evaluate(inputElem, outputElem);

    cout << "Testing Inputs" << endl;

    QCOMPARE(inputElem[0]->value<int>(), 1);
    QCOMPARE(inputElem[1]->value<double>(), 2.0);

    cout << "Testing Outputs" << endl;

    QCOMPARE(outputElem[0]->value<int>(), 1);
    QCOMPARE(outputElem[1]->value<double>(), 2.0);

    delete func;
}

void tst_matlabintegration::test_twoFunctions()
{

    // Ensure functionality is unchanged when two differnet matlab functions are in scope
    MatlabFunction* func1 = new MatlabFunction();
    MatlabFunction* func2 = new MatlabFunction();

    func1->definePath(m_testBinPath + "/test1_matlabTigonWrapper.m");
    func2->definePath(m_testBinPath + "/test2_matlabTigonWrapper.m");

    //**************** Test func1's properties ************************//

    cout << "Function 1: Testing Properties" << endl;

    QCOMPARE(func1->command(), TString("test1_matlabTigonWrapper"));

    TVector<ElementProperties> inputProperties1 = func1->inputPrpts();
    TVector<ElementProperties> outputProperties1 = func1->outputPrpts();

    QCOMPARE(inputProperties1.size() == 2, true);
    QCOMPARE(outputProperties1.size() == 2, true);
    QCOMPARE(func1->TP_nInputs(), 2);
    QCOMPARE(func1->TP_nOutputs(), 2);

    TStringList InputNames1;
    InputNames1 << "a" << "b";
    TStringList InputDescriptions1;
    InputDescriptions1 << "A value" << "Another value";
    TStringList InputUnits1;
    InputUnits1 << "mm" << "mm";

    TVector<ElementType> types1;
    types1 << ElementType::RealType << ElementType::RealType;

    TStringList OutputNames1;
    OutputNames1 << "out1" << "out2";
    TStringList OutputDescriptions1;
    OutputDescriptions1 << "The first output" << "The second output";
    TStringList OutputUnits1;
    OutputUnits1 << "mm" << "mm";

    for(int i=0; i<func1->TP_nInputs(); i++) {
        QCOMPARE(inputProperties1[i].name(), InputNames1[i]);
        QCOMPARE(inputProperties1[i].description(), InputDescriptions1[i]);
        QCOMPARE(inputProperties1[i].unit(), InputUnits1[i]);
        QCOMPARE(inputProperties1[i].type(), types1[i]);
    }

    for(int i=0; i<func1->TP_nOutputs();i++) {
        QCOMPARE(outputProperties1[i].name(), OutputNames1[i]);
        QCOMPARE(outputProperties1[i].description(), OutputDescriptions1[i]);
        QCOMPARE(outputProperties1[i].unit(), OutputUnits1[i]);
        QCOMPARE(outputProperties1[i].type(), types1[i]);
    }

    //**************** Test func2's properties ************************//

    cout << "Function 2: Testing Properties" << endl;

    QCOMPARE(func2->command(), TString("test2_matlabTigonWrapper"));

    TVector<ElementProperties> inputProperties2 = func2->inputPrpts();
    TVector<ElementProperties> outputProperties2 = func2->outputPrpts();

    QCOMPARE(inputProperties2.size() == 2, true);
    QCOMPARE(outputProperties2.size() == 2, true);
    QCOMPARE(func2->TP_nInputs(), 2);
    QCOMPARE(func2->TP_nOutputs(), 2);

    TStringList InputNames2;
    InputNames2 << "intIn" << "realIn";
    TStringList OutputNames2;
    OutputNames2 << "intOut" << "realOut";

    TStringList descriptions2;
    descriptions2 << "Integer" << "Real";
    TStringList units2;
    units2 << "mm" << "mm";
    TVector<ElementType> types2;
    types2 << ElementType::IntegerType << ElementType::RealType;

    for(int i=0; i<func2->TP_nInputs(); i++) {
        QCOMPARE(inputProperties2[i].name(), InputNames2[i]);
        QCOMPARE(inputProperties2[i].description(), descriptions2[i]);
        QCOMPARE(inputProperties2[i].unit(), units2[i]);
        QCOMPARE(inputProperties2[i].type(), types2[i]);
    }

    for(int i=0; i<func2->TP_nOutputs();i++) {
        QCOMPARE(outputProperties2[i].name(), OutputNames2[i]);
        QCOMPARE(outputProperties2[i].description(), descriptions2[i]);
        QCOMPARE(outputProperties2[i].unit(), units2[i]);
        QCOMPARE(outputProperties2[i].type(), types2[i]);
    }

    //**************** Test func1's evaluation ************************//

    cout << "Function 1: Testing Evaluation" << endl;

    TVector<double> inputReal1;
    TVector<double> outputReal1;
    inputReal1 << 2.5 << 7;
    outputReal1 << 0 << 0;

    TVector<IElementSPtr>inputElem1 =  createIElementSPtrVector(2, ElementType::RealType);
    inputElem1[0]->defineValue(2.5);
    inputElem1[1]->defineValue(7);
    TVector<IElementSPtr>outputElem1 = createIElementSPtrVector(2, ElementType::RealType);

    func1->evaluate(inputElem1, outputElem1);

    cout << "Testing inputs unchanged...";

    QCOMPARE(inputElem1[0]->value<double>(), 2.5);
    QCOMPARE(inputElem1[1]->value<double>(), 7.0);

    cout << "Testing outputs..." << endl;

    QCOMPARE(outputElem1[0]->value<double>(), 9.5); // Input1 + Input2
    QCOMPARE(outputElem1[1]->value<double>(), 3.5); // Input1 + 1

    //**************** Test func2's evaluation ************************//

    cout << "Function 2: Testing Evaluation" << endl;

    IElementSPtr in1(new IElement(ElementType::IntegerType, 100));

    IElementSPtr in2(new IElement(ElementType::RealType, 20.25));

    IElementSPtr out1(new IElement(ElementType::IntegerType));
    IElementSPtr out2(new IElement(ElementType::RealType));

    TVector<IElementSPtr> inputElem2;
    inputElem2 << in1 << in2;
    TVector<IElementSPtr> outputElem2;
    outputElem2 << out1 << out2;

    func2->evaluate(inputElem2, outputElem2);

    cout << "Testing inputs unchanged...";

    QCOMPARE(inputElem2[0]->value<int>(), 100);
    QCOMPARE(inputElem2[1]->value<double>(), 20.25);

    cout << "Testing outputs..." << endl;

    QCOMPARE(outputElem2[0]->value<int>(), 100);
    QCOMPARE(outputElem2[1]->value<double>(), 20.25);

    delete func1;
    delete func2;
}

void tst_matlabintegration::test_badCommand()
{
    MatlabFunction* func = new MatlabFunction();

    cout << "Testing non-existing command..." << endl;

    func->defineCommand("JUNKCOMMADN1234");
    QCOMPARE(func->inputPrpts().empty(), true);
    QCOMPARE(func->outputPrpts().empty(), true);
    QCOMPARE(func->TP_nInputs(), 0);
    QCOMPARE(func->TP_nOutputs(), 0);

    cout << "Testing existing command, not in wrapper..." << endl;

    func->defineCommand("qr");
    QCOMPARE(func->inputPrpts().empty(), true);
    QCOMPARE(func->outputPrpts().empty(), true);
    QCOMPARE(func->TP_nInputs(), 0);
    QCOMPARE(func->TP_nOutputs(), 0);

    delete func;
}

void tst_matlabintegration::test_dtlz1()
{
    int maxIter = 2;

    IFunctionSPtr func = IFunctionSPtr(new MatlabFunction);
    func->definePath(m_testBinPath + "/DTLZ1.m");

    TVector<ElementProperties> inputProperties  = func->inputPrpts();
    TVector<ElementProperties> outputProperties = func->outputPrpts();

    QCOMPARE(inputProperties.size() == 5, true);
    QCOMPARE(outputProperties.size() == 2, true);

    OptimizationLinearFlow* flow = new OptimizationLinearFlow;
    PSetBase* base = new PSetBase();
    flow->appendNode(base);

    IFormulation* prob = new IFormulation(flow->currentNode());
    prob->appendFunction(func);
    flow->appendNode(prob);

    BoxConstraintsDataSPtr box = prob->problem()->boxConstraints();

    QCOMPARE(box->size(), 5);
    for(int i=0; i<box->size(); i++) {
        QCOMPARE(box->lowerBound(i).value(), 0.0);
        QCOMPARE(box->upperBound(i).value(), 1.0);
    }

    RandomInit* init = new RandomInit(flow->currentNode());
    init->TP_defineSetSize(100);
    flow->appendNode(init);

    Evaluator* eval = new Evaluator(flow->currentNode());
    flow->appendNode(eval);

    NSGAII* alg = new NSGAII(flow->currentNode());
    flow->appendNode(alg);

    for(int i=0; i<maxIter; i++) {
        flow->evaluate();
        if(i % 10 == 0) {
            cout << "Iter [" << i << "]" << endl;
        }
    }

    ISet* pop = flow->finalNode()->allSets().at(0);
    cout << "sol = " << pop;

    flow->clearFlow();
    delete flow;
}

void tst_matlabintegration::test_mergeAndMix()
{

    IFormulation prob;
    IFunctionSPtr func1 = IFunctionSPtr(new DTLZ2);
    func1->TP_defineNInputs(7);
    func1->TP_defineNOutputs(3);

    IFunction* pfunc = new MatlabFunction();
    pfunc->definePath(m_testBinPath + "/DTLZ1.m");
    IFunctionSPtr func2 = IFunctionSPtr(pfunc);

    prob.appendFunction(func1);
    prob.appendFunction(func2);
    QCOMPARE(prob.problem()->processProblemDefinition(),
             ProblemDefinitionStatus::FullyDefined);
    QCOMPARE(prob.problem()->dPrpts().size() == 12, true);
    QCOMPARE(prob.problem()->oPrpts().size() == 5, true);
}

void tst_matlabintegration::test_loadMatlabFunctionFromProblemGenerator()
{

    ProblemGenerator pg;
    TString funcTypeStr(Tigon::FuncTypeStrMatlab);
    TString funcPath = m_testBinPath + "/" + "DTLZ1.m";
    IFunctionSPtr matFunc = pg.createFunction(funcTypeStr, funcPath);
    QCOMPARE(bool(matFunc), true);
    QCOMPARE(matFunc->inputPrpts().size() == 5, true);
    QCOMPARE(matFunc->outputPrpts().size() == 2, true);
    QCOMPARE(matFunc->functionProperties().name(), TString("DTLZ1"));
    QCOMPARE(matFunc->functionProperties().description(), TString("DTLZ1 Matlab Version"));

    matFunc = IFunctionSPtr(ProblemGenerator::createFunctionByName("Tigon::Representation::MatlabFunction",
                                                                 funcPath));
    QCOMPARE(bool(matFunc), true);
    QCOMPARE(matFunc->inputPrpts().size() == 5, true);
    QCOMPARE(matFunc->outputPrpts().size() == 2, true);
    QCOMPARE(matFunc->functionProperties().name(), TString("DTLZ1"));
    QCOMPARE(matFunc->functionProperties().description(), TString("DTLZ1 Matlab Version"));
    for(int i=0; i<matFunc->boxConstraints()->lowerBounds().size(); i++) {
        QCOMPARE(matFunc->boxConstraints()->lowerBound(i).value(), 0.0);
    }

    for(int i=0; i<matFunc->boxConstraints()->upperBounds().size(); i++) {
        QCOMPARE(matFunc->boxConstraints()->upperBound(i).value(), 1.0);
    }
}

void tst_matlabintegration::test_loadMatlabFunctionFromWorkflow()
{
    TigonEngine* eng = TigonEngine::instance();
    TString workflowFile =  m_testBinPath+ "/test_matlabfunction_workflow.lgr";

    QFile file(QString::fromStdString(workflowFile));
    if(file.open(QIODevice::ReadWrite)) {
        QTextStream tstream(&file);
        QString content = tstream.readAll();
        if(content.indexOf("####REPLACE_ME####") > -1) {
            content.replace("####REPLACE_ME####", QString::fromStdString(m_testBinPath));
            file.resize(0);
            tstream << content;
        }
        file.close();
    } else {
        QFAIL("Workflow cannot open");
    }

    eng->loadFlowFromFile(workflowFile);
    for(int i=0; i<eng->currentFlow()->size(); i++) {
        IPSet* op = eng->currentFlow()->node(i);
        cout << "* Operator: "<< op->className() << endl;
        if(op->className() == TString("Tigon::Operators::RandomInit")) {
            RandomInit* init = dynamic_cast<RandomInit*>(op);
            if(init) {
                cout <<"OptimizationSetSize: " << init->TP_setSize() << endl;
                QCOMPARE(init->TP_setSize(), 100);
            }
            continue;
        }

        if(op->className() == TString("Tigon::Operators::ProblemGenerator")) {
            ProblemGenerator* pg = dynamic_cast<ProblemGenerator*>(op);
            if(pg) {
                cout << "Function Vector Size: " << pg->functionVec().size() << endl;
                cout << "Function[0]: " << pg->functionVec()[0]->name() << endl;
                QCOMPARE(pg->functionVec()[0]->name(), TString("DTLZ1"));
            }
            continue;
        }
    }

    cout << "* maxIter: " << eng->currentFlow()->maxIteration() << endl;

    while(!eng->currentFlow()->isTerminate()) {
        eng->currentFlow()->evaluate();
        eng->currentFlow()->incrementIteration();
        cout << "Iter " << eng->currentFlow()->currentIteration() << endl;;
    }

    QCOMPARE(eng->currentFlow()->currentIteration(),
             eng->currentFlow()->maxIteration());

    ISet* pop = eng->currentFlow()->finalNode()->allSets().at(0);
    cout << "sol = " << pop;
}

void tst_matlabintegration::test_matlabFunctionEvaluation()
{
    TRAND.defineSeed(19273729);

    MatlabFunction* pfunc = new MatlabFunction();
    pfunc->definePath(m_testBinPath + "/DTLZ1.m");

    TVector<IElementSPtr> inputs;
    TVector<IElementSPtr> outputs;
    inputs.reserve(pfunc->inputPrpts().size());

    outputs.push_back(IElementSPtr(new IElement(Tigon::RealType)));
    outputs.push_back(IElementSPtr(new IElement(Tigon::RealType)));

    for(int i=0; i<pfunc->inputPrpts().size(); i++) {
        inputs.push_back(IElementSPtr(new IElement(TRAND.randUni())));
    }
    pfunc->evaluate(inputs, outputs);
    QCOMPARE((outputs[0]->value() - 177.4568)/177.4568 < 1e-05, true);
    QCOMPARE((outputs[1]->value() - 99.8680)/99.8680 < 1e-05, true);
}

void tst_matlabintegration::test_matlabFunctionBatchEvaluation()
{
    TRAND.defineSeed(19273729);

    MatlabFunction* pfunc = new MatlabFunction();
    pfunc->definePath(m_testBinPath + "/DTLZ1.m");

    int setSize = 3;
    TVector<TVector<double> > in(setSize);
    TVector<TVector<double> > out(setSize);
    in[0] << 0.582249164527227 << 0.264779026475630 << 0.939829470344921 << 0.639316961040108 << 0.543885933999639;
    in[1] << 0.540739337124410 << 0.318074075481059 << 0.645551874972524 << 0.544716110526763 << 0.721046620579811;
    in[2] << 0.869941032358007 << 0.119214541054191 << 0.479463224948888 << 0.647311480293128 << 0.522495305777102;

    out[0] << 215.196843593909 << 154.398952681117;
    out[1] << 144.764572036901 << 122.95142733304;
    out[2] << 190.270754929151 << 28.4460866174709;

    TVector<TVector<IElementSPtr> > inputs;
    TVector<TVector<IElementSPtr> > outputs;

    inputs.resize(setSize);
    outputs.resize(setSize);
    for(int i=0; i<setSize; i++) {
        inputs[i].reserve(pfunc->inputPrpts().size());
        for(int j=0; j<pfunc->inputPrpts().size(); j++) {
            inputs[i].push_back(IElementSPtr(new IElement(in[i][j])));
        }

        outputs[i].reserve(pfunc->outputPrpts().size());
        for(int j=0; j<pfunc->outputPrpts().size(); j++) {
            outputs[i].push_back(IElementSPtr(new IElement(Tigon::RealType)));
        }
    }

    pfunc->batchEvaluate(inputs, outputs);
    for(int i=0; i<out.size(); i++) {
        for(int j=0; j<out[i].size(); j++) {
            QCOMPARE(areDoublesEqual(out[i][j], outputs[i][j]->value<double>()), true);
        }
        cout <<  copyIElementVecFromPointers(outputs[i]);
    }
}

void tst_matlabintegration::test_matlabEvaluatorCountError()
{
    PSetBase* base = new PSetBase();
    IFormulation* prob = new IFormulation(base);

    IFunctionSPtr func1 = IFunctionSPtr(new DTLZ1);
    func1->TP_defineNInputs(5);
    func1->TP_defineNOutputs(2);

    prob->appendFunction(func1);

    IFunctionSPtr func2 = IFunctionSPtr(new MatlabFunction);
    func2->definePath(m_testBinPath + "/DTLZ1.m");
    prob->appendFunction(func2);

    prob->problem()->processProblemDefinition();

    RandomInit* init = new RandomInit(prob);
    init->TP_defineSetSize(10);

    BatchEvaluator* eval = new BatchEvaluator(init);
    try {
        eval->evaluateNode();
        QCOMPARE(false, true);
    } catch(const TException& e) {
        QCOMPARE(TString(e.what()),
                 TString("Tigon::Operators::BatchEvaluator: "
                         + BATCH_EVAL_COUNT_ERROR));
    }

    delete base;
    delete prob;
    delete init;
    delete eval;
}

void tst_matlabintegration::test_matlabEvaluator()
{
    int maxIter = 2;
    int setSize = 3;

    TRAND.defineSeed(110103);

    IFunctionSPtr func = IFunctionSPtr(new MatlabFunction);
    func->definePath(m_testBinPath + "/DTLZ1.m");

    OptimizationLinearFlow flow;
    PSetBase* base = new PSetBase();
    flow.appendNode(base);

    IFormulation* prob = new IFormulation(flow.currentNode());
    prob->appendFunction(func);
    flow.appendNode(prob);

    RandomInit* init = new RandomInit(flow.currentNode());
    init->TP_defineSetSize(setSize);
    flow.appendNode(init);

    BatchEvaluator* eval = new BatchEvaluator(flow.currentNode());
    flow.appendNode(eval);

    NSGAII* alg = new NSGAII(flow.currentNode());
    flow.appendNode(alg);

    for(int i=0; i<maxIter; i++) {
        flow.evaluate();
    }

    ISet* pop = flow.finalNode()->allSets().at(0);
    cout << "sol = " << pop;

    QCOMPARE(areDoublesEqual(pop->at(0)->doubleObjectiveVec()[0], 79.2085), true);
    QCOMPARE(areDoublesEqual(pop->at(0)->doubleObjectiveVec()[1], 7.24776), true);
    QCOMPARE(areDoublesEqual(pop->at(1)->doubleObjectiveVec()[0], 79.2085), true);
    QCOMPARE(areDoublesEqual(pop->at(1)->doubleObjectiveVec()[1], 7.24776), true);
    QCOMPARE(areDoublesEqual(pop->at(2)->doubleObjectiveVec()[0], 79.2085), true);
    QCOMPARE(areDoublesEqual(pop->at(2)->doubleObjectiveVec()[1], 7.24776), true);

    flow.clearFlow();
}

void tst_matlabintegration::test_cleanup()
{
    //Release any resources used in the test
    cout << "Reset Matlab to previous status" << endl;
    MatlabEngine* eng = MatlabPool::getInstance().aquireEngine();
    QCOMPARE(eng != nullptr, true);

    MatlabPool::getInstance().releaseEngine(eng);

    // This line is required. MATLAB API falls over if it's not there and program hangs as its closing.
    QCOMPARE(MatlabPool::useMatlab(), true);
    MatlabPool::getInstance().emptyPool();
    QCOMPARE(MatlabPool::useMatlab(), false);
}

QTEST_GUILESS_MAIN(tst_matlabintegration)

#include "tst_matlabintegration.moc"
