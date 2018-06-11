/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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

#include <iostream>
#include <math.h>
#include <iomanip>

#include <tigon/Tigon.h>
#include <eigen/bench/BenchTimer.h>

#include <tigon/Core/TString.h>
#include <tigon/Core/TObject.h>

#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/math/special_functions/factorials.hpp>

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;

#define TEST_ALL_BASE_OPERATIONS

class tst_baseoperations : public QObject
{
    Q_OBJECT

public:

private slots:
    void test_TString();
    void test_TString_Conversion();

    /// Property system
    void test_TObject_className();
    void test_TObject_addProperties();
    void test_TObject_getSetProperties();

#ifdef TEST_ALL_BASE_OPERATIONS
    void test_IElement_Constructors();
    void test_IElement_value();
    void test_IElement_dist();
    void test_IElement_IDGeneration();
    void test_IElement_IDGenerationLarge();
    void test_OutputStreamOperators();
    void test_RealTypeOperations();
    void test_IntegerTypeOperations();
    void test_OrdinalTypeOperations();
    void test_NominalTypeOperations();
    void test_TypeMixedOperations();

    void test_Problem_constructors();
    void test_Problem_problemProperties();
    void test_Problem_vectorProperties();
    void test_Problem_appendFunc();
    void test_Problem_unusedOutputs();
    void test_Problem_defineFuncAndMaps();
    void test_Problem_definitionStatus();
    void test_ProblemFormulationMultiFunctions();
    void test_UncertaintyMappingEvaluationOutOfBounds();

    void test_IMapping_constructorsDistructor();

    void test_TypeIMappingOperations();

    void test_TypeIdentification();
    void test_AlgorithmCreation();

    void test_ISet_constructorDestructor();
    void test_ISet_constructorDestructorFromSet();
    void test_ISet_constructorDestructorwithTags();
    void test_ISet_all();
    void test_ISet_IMappings();
    void test_ISet_IMap();
    void test_ISet_define_iMap();
    void test_ISet_define_set();
    void test_ISet_removeIdx();
    void test_ISet_removePtr();
    void test_ISet_replace();
    void test_ISet_shuffle();
    void test_ISet_clear();
    void test_ISet_addRemoveTags();
    void test_ISet_defineClearTags();

    void test_IPSetSets_allTags();
    void test_IPSetSets_tagExists();
    void test_IPSetSets_setsWithTag();
    void test_IPSetSets_setsWithTags();
    void test_IPSetSets_setWithTag();
    void test_IPSetSets_tagSet();
    void test_IPSetSets_removeTagFromSet();

    void test_IPSetSets_allSets();
    void test_IPSetSets_numberOfSets();
    void test_IPSetSets_clearSets();
    void test_IPSetSets_indexOfSet();
    void test_IPSetSets_appendSet();
    void test_IPSetSets_removeSet();
    void test_IPSetSets_appendToSet();

#endif
    void test_IPSet_multipack();
    void test_IPSet_multipack_customType();

#ifdef TEST_ALL_BASE_OPERATIONS

    void test_nonDominatedArchive();
    void test_nadir();
    void test_IPSet_IdealAndAntiIdeal();
    void test_IPSet_direction();

    void test_IFunction_ctor();
    void test_IFunction_defineNVariables();

    void test_IOperators_addOutputTag();
    void test_IOperators_TP_defineOutputTags();
    void test_IOperators_removeOutputTag();
    void test_IOperators_addInputTag();
    void test_IOperators_TP_defineInputTags();
    void test_IOperators_removeInputTag();
    void test_IOperators_appendOutputSet();
    void test_IOperators_nextOutputSet();
    void test_IOperators_removeOutputSet();
    void test_IOperators_clearOutputSets();
    void test_IOperators_acceptableTypes();

    void test_elementIDGenerator();
    void test_SharedDataExperiemnts();
    void test_IMapping();
#endif
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
    for(int i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

Tigon::Representation::ProblemSPtr createProblem(int dN, int oN, Tigon::ElementType type)
{
    ExampleFunction* func = new ExampleFunction;
    func->TP_defineNInputs(dN);
    func->TP_defineNOutputs(oN);
    func->redefineInputsType(type);
    func->redefineOutputsType(type);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(IFunctionSPtr(func));

    prob->processProblemDefinition();

    return prob;
}


Tigon::Representation::PSetBase* initIPSetTest() // A PSetBase with 10 IMappings
{
    int dN = 1;
    int oN = 2;

    ProblemSPtr prob = createProblem(dN, oN, IntegerType);

    PSetBase* base = new PSetBase();
    IFormulation* form = new IFormulation(base);

    form->defineProblem(prob);
    delete form;

    ISet* solset = base->appendSet(TString(""));

    // Create IMappings 0-9 in the first set
    for(int i=0; i<10; i++) {
        IMappingSPtr imap = base->createOptimizationMapping(solset);
        imap->defineDecisionVar(0, IElement(i));
    }

    /* append 4 more sets with different tags, some overlapping
     *
     * set index            tags             no. of mappings
     * ---------    ----------------------   ---------------
     *     0                                        10
     *     1        tag1                             1
     *     2              tag2  tag3                 2
     *     3                    tag3  tag4           3
     *     4                          tag4           4
     */

    solset = base->appendSet("tag1");
    base->createOptimizationMapping(solset);

    TStringList tags;
    tags << "tag2" << "tag3";
    solset = base->appendSet(tags);
    base->createOptimizationMapping(solset);
    base->createOptimizationMapping(solset);

    tags.erase(tags.begin());
    tags << "tag4";
    solset = base->appendSet(tags);
    base->createOptimizationMapping(solset);
    base->createOptimizationMapping(solset);
    base->createOptimizationMapping(solset);

    solset = base->appendSet("tag4");
    base->createOptimizationMapping(solset);
    base->createOptimizationMapping(solset);
    base->createOptimizationMapping(solset);
    base->createOptimizationMapping(solset);

    return base;
}

#ifdef TEST_ALL_BASE_OPERATIONS
void tst_baseoperations::test_TString()
{
    TStringList list;
    list.push_back("Hello");
    list.push_back("World!");
    TString str = join(list, ", ");
    bool res = str == TString("Hello, World!");
    QCOMPARE(res, true);

    list.push_back(str);
    int idx = indexOf(list,"World!");
    QCOMPARE(idx, 1);

    replaceAll(str, "o", "+");
    res = str == TString("Hell+, W+rld!");
    QCOMPARE(res, true);

    TStringList newList = split(str, ",");
    QCOMPARE(newList[0] == TString("Hell+"), true);
    QCOMPARE(newList[1] == TString(" W+rld!"), true);

    trim(newList[1]);
    QCOMPARE(newList[1] == TString("W+rld!"), true);
}

void tst_baseoperations::test_TString_Conversion()
{
    {
        TString s = "2";
        QCOMPARE(toInt(s), 2);
    }
    {
        TString s = "2.1";
        QCOMPARE(toDouble(s), 2.1);
    }

    {
        TString s = "true";
        QCOMPARE(toBool(s), true);
    }
    {
        TString s = "false";
        QCOMPARE(toBool(s), false);
    }
}

class TestTObject : public TObject
{
public:
    TestTObject(): m_int(-1), m_bool(false), m_double(1.0), m_string("default"){}
    void setProperty(const TString& name, const TString& value)
    {
        if(name == "PropInt") {
            m_int = toInt(value);
            return;
        }

        if(name == "PropDouble") {
            m_double = toDouble(value);
            return;
        }

        if(name == "PropBool") {
            m_bool = toBool(value);
            return;
        }

        if(name == "PropString") {
            m_string = value;
            return;
        }
    }
    TString propertyValue(const TString& name)
    {
        if(name == "PropInt") {
            return convertToString(m_int);
        }

        if(name == "PropDouble") {
            return convertToString(m_double);
        }

        if(name == "PropBool") {
            return convertToString(m_bool);
        }

        if(name == "PropString") {
            return m_string;
        }
        return "";
    }

public:
    void addProperty(const TString& name,
                     const TString& description,
                     TTypeHash type) {TObject::addProperty(name, description, type);}
    int m_int;
    bool m_bool;
    double m_double;
    TString m_string;
};

void tst_baseoperations::test_TObject_className()
{
    TestTObject obj;
    cout << obj.className() << endl;
    QEXPECT_FAIL("", "Will fix in the next release", Continue);
    QCOMPARE(obj.className() == "TestTObject", true);

    TestTObject* objptr = new TestTObject();
    cout << objptr->className() << endl;
    QEXPECT_FAIL("", "Will fix in the next release", Continue);
    QCOMPARE(objptr->className() == "TestTObject", true);
    delete objptr;

    TSharedPointer<TestTObject> objsptr = TSharedPointer<TestTObject>(new TestTObject());
    cout << objsptr->className() << endl;
    QEXPECT_FAIL("", "Will fix in the next release", Continue);
    QCOMPARE(objsptr->className() == "TestTObject", true);
}

void tst_baseoperations::test_TObject_addProperties()
{
    TStringList names({"PropInt", "PropDouble", "PropBool", "PropString"});
    TStringList des({"Property One", "Property Two", "Property Three", "Property Four"});
    TestTObject obj;
    obj.addProperty(names[0], des[0], typeid(int).hash_code());
    obj.addProperty(names[1], des[1], typeid(double).hash_code());
    obj.addProperty(names[2], des[2], typeid(bool).hash_code());
    obj.addProperty(names[3], des[3], typeid(TString).hash_code());

    QCOMPARE(obj.propertyNames()[0] == names[0], true);
    QCOMPARE(obj.propertyNames()[1] == names[1], true);
    QCOMPARE(obj.propertyNames()[2] == names[2], true);
    QCOMPARE(obj.propertyNames()[3] == names[3], true);

    QCOMPARE(obj.propertyDescriptions()[0] == des[0], true);
    QCOMPARE(obj.propertyDescriptions()[1] == des[1], true);
    QCOMPARE(obj.propertyDescriptions()[2] == des[2], true);
    QCOMPARE(obj.propertyDescriptions()[3] == des[3], true);

    QCOMPARE(obj.propertyTypes()[0] == typeid(int).hash_code(), true);
    QCOMPARE(obj.propertyTypes()[1] == typeid(double).hash_code(), true);
    QCOMPARE(obj.propertyTypes()[2] == typeid(bool).hash_code(), true);
    QCOMPARE(obj.propertyTypes()[3] == typeid(TString).hash_code(), true);
}

void tst_baseoperations::test_TObject_getSetProperties()
{
    TestTObject obj;

    QCOMPARE(obj.m_int, -1);
    QCOMPARE(obj.m_double, 1.0);
    QCOMPARE(obj.m_bool, false);
    QCOMPARE(obj.m_string == "default", true);

    QCOMPARE(toInt(obj.propertyValue("PropInt")), -1);
    QCOMPARE(toDouble(obj.propertyValue("PropDouble")), 1.0);
    QCOMPARE(toBool(obj.propertyValue("PropBool")), false);
    QCOMPARE(obj.propertyValue("PropString") == "default", true);

    obj.setProperty("PropInt", "123");
    obj.setProperty("PropDouble", "123.0009");
    obj.setProperty("PropBool", "true");
    obj.setProperty("PropString", "djdjdjdjdhwu");

    QCOMPARE(obj.m_int, 123);
    QCOMPARE(obj.m_double, 123.0009);
    QCOMPARE(obj.m_bool, true);
    QCOMPARE(obj.m_string == "djdjdjdjdhwu", true);

    QCOMPARE(obj.propertyValue("PropInt") == "123", true);
    cout << obj.propertyValue("PropDouble");
    QCOMPARE(obj.propertyValue("PropDouble") == "123.0009", true);
    QCOMPARE(obj.propertyValue("PropBool") == "true", true);
    QCOMPARE(obj.propertyValue("PropString") == "djdjdjdjdhwu", true);
}

void tst_baseoperations::test_IElement_Constructors()
{
    IElement defElem;
    IElement explicitReal(RealType, 3.6);
    IElement implicitReal(3.6);
    IElement explicitInt(IntegerType, 3.2);
    IElement implicitInt(3);
    IElement copyElem(explicitInt);
    IElement typedElem(OrdinalType);
    IElement typedNominal(NominalType);

    QCOMPARE(defElem.type(),      RealType);
    QCOMPARE(explicitReal.type(), RealType);
    QCOMPARE(implicitReal.type(), RealType);
    QCOMPARE(explicitInt.type(),  IntegerType);
    QCOMPARE(implicitInt.type(),  IntegerType);
    QCOMPARE(copyElem.type(),     IntegerType);
    QCOMPARE(typedElem.type(),    OrdinalType);
    QCOMPARE(typedNominal.type(), NominalType);

    QCOMPARE(defElem.dist() == nullptr, true);

    QCOMPARE(defElem.value<double>(),      0.0);
    QCOMPARE(explicitReal.value<double>(), 3.6);
    QCOMPARE(implicitReal.value<double>(), 3.6);
    QCOMPARE(explicitInt.value<double>(),  3.0);
    QCOMPARE(implicitInt.value<double>(),  3.0);
    QCOMPARE(copyElem.value<double>(),     3.0);
    QCOMPARE(typedElem.value<double>(),    0.0);
}

void tst_baseoperations::test_IElement_value()
{
    IElement realElem(3.6);

    double realVal = realElem.value<double>();
    QCOMPARE(realVal, 3.6);

    // WRONG: Converting to int returns the rounded value
    int intVal = realElem.value<int>();
    //QCOMPARE(intVal, 4);
    QCOMPARE(intVal, 3);

    // The previous conversion did not change the IElement
    realVal = realElem.value<double>();
    QCOMPARE(realVal, 3.6);

    // WRONG: Initialisation of an int with a double rounds the value
    IElement intElem(IntegerType, -123.7);
    //QCOMPARE(intElem.value<double>(), -124.0);
    QCOMPARE(intElem.value<double>(), -123.0);

    // WRONG: Changing the type of an element to an integer/ordinal/nominal rounds the value
    IElement nominalElem(realElem);
    nominalElem.defineType(NominalType);
    //QCOMPARE(nominalElem.value<double>(), 4.0);
    QCOMPARE(nominalElem.value<double>(), 3.0);
}

void tst_baseoperations::test_IElement_dist()
{
    IElement r;
    bool rs = r.dist() == nullptr;
    QCOMPARE(rs, true);

    IDistribution* uniDist = new UniformDistribution(5.0, 10.0);
    r.defineDist(uniDist);
    rs = r.dist() == nullptr;
    QCOMPARE(rs, false);

    cout << r.sample() << endl;

    r.defineDist(nullptr);
    rs = r.dist() == nullptr;
    QCOMPARE(rs, true);

    cout << r.sample() << endl;
}

void tst_baseoperations::test_IElement_IDGeneration()
{
    TString name1("x1");
    TString name2("x2");
    ElementProperties p1
            = ElementPropertiesFactory::instance()->createElementProperties(0, name1, name1);
    ElementProperties p2
            = ElementPropertiesFactory::instance()->createElementProperties(1, name2, name2);

    QCOMPARE(p1.ID() != p2.ID(), true);

    ElementProperties p3
            = ElementPropertiesFactory::instance()->createElementProperties(2, name1, name2);

    QCOMPARE(p1.ID() != p3.ID(), true);
}

void tst_baseoperations::test_IElement_IDGenerationLarge()
{
    int N = 1000; // Make it large
    TStringList names;
    TVector<ElementProperties> prpts;
    names.reserve(N);
    prpts.reserve(N);
    for (int i=0; i<N; i++) {
        TString name = "testIDvarName" + convertToString(i);
        names.push_back(name);
        ElementProperties p = ElementPropertiesFactory::instance()->createElementProperties(i, name, name);
        prpts.push_back(p);
    }

    // Tests
    for (int i=0; i<N; i++) {
        QCOMPARE(prpts.at(i).ID().empty(), false);
        TString iid = prpts.at(i).ID();

        /// \note Used to have a bug that prevets the ID to be copied.
        /// This test captures it.
        QCOMPARE(iid.empty(), false);
        for (int j=0; j<N; j++) {
            if (i!=j) {
                QCOMPARE(iid != prpts.at(j).ID(), true);
            }
        }
    }
}

void tst_baseoperations::test_OutputStreamOperators()
{
    IElement e1(1);
    IElement e2(1.5);

    std::cout << e1 << std::endl;
    std::cout << e2 << std::endl;
}

void tst_baseoperations::test_RealTypeOperations()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    // Negation
    {
        cout << "Testing RealType Negation ..." << endl;
        IElement v1(RealType, 1.12138);
        IElement v2 = -v1;
        QCOMPARE(v2.value<double>(),-1.12138);
    }
    // Multiplication
    {
        cout << "Testing RealType * RealType Multiplication ..." << endl;
        IElement v1(RealType, 2.0);
        IElement v2(RealType, 100.05);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 200.1);
    }

    {
        cout << "Testing RealType * qint64 Multiplication ..." << endl;
        IElement v1(RealType, 2.234);
        qint64 v2(34359738368);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 76759655514.112);
    }

    {
        cout << "Testing RealType * double Multiplication ..." << endl;
        IElement v1(RealType, 2.2);
        double v2(100.0);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 220.0);
    }

    {
        cout << "Testing qint64 * RealType Multiplication ..." << endl;
        qint64 v1(34359738368);
        IElement v2(RealType, 2.234);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 76759655514.112);
    }

    {
        cout << "Testing double * RealType Multiplication ..." << endl;
        double v1(100.0);
        IElement v2(RealType, 2.234);

        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 223.4);
    }

    // Division
    {
        cout << "Testing RealType/RealType Division ..." << endl;
        IElement v1(RealType, 30.45);
        IElement v2(RealType, 2.0);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 15.225);
    }

    {
        cout << "Testing RealType/qint64 Division ..." << endl;
        IElement v1(RealType, 3.356);
        qint64 v2(2);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 1.678);
    }

    {
        cout << "Testing RealType/double Division ..." << endl;
        IElement v1(RealType, 350.0);
        double v2(100.0);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 3.5);
    }

    {
        cout << "Testing qint64/RealType Division ..." << endl;
        qint64 v1(300);
        IElement  v2(10.0);
        IElement  rs = v1/v2;
        QCOMPARE(rs.value<double>(), 30.0);
    }

    {
        cout << "Testing double/RealType Division ..." << endl;
        double v1(300.0);
        IElement v2(RealType, 160.0);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 1.875);
    }

    // Division by Zero
    {
        cout << "Testing RealType Division by Zero ..." << endl;
        IElement v1(RealType, 300.456);
        IElement v2(RealType, 0.0);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), DivisionByZeroException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Addition
    {
        cout << "Testing RealType + RealType Addition ..." << endl;
        IElement v1(RealType, 2.5);
        IElement v2(RealType, 100.5);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 103.0);
    }

    {
        cout << "Testing RealType + qint64 Addition ..." << endl;
        IElement v1(RealType, 2.786);
        qint64 v2(100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.786);
    }

    {
        cout << "Testing RealType + double Addition ..." << endl;
        IElement v1(RealType,   2.765);
        double v2(100.123);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.888);
    }

    {
        cout << "Testing qint64 + RealType Addition ..." << endl;
        qint64 v1(2);
        IElement v2(RealType, 100.8765);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.8765);
    }

    {
        cout << "Testing double + RealType Addition ..." << endl;
        double v1(2.0);
        IElement v2(RealType, 100.25);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.25);
    }

    // Subtraction
    {
        cout << "Testing RealType - RealType Subtraction ..." << endl;
        IElement v1(RealType, 2.25);
        IElement v2(RealType, 100.0);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-97.75);
    }

    {
        cout << "Testing RealType - qint64 Subtraction ..." << endl;
        IElement v1(RealType, 2.25);
        qint64 v2(100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-97.75);
    }

    {
        cout << "Testing RealType - double Subtraction ..." << endl;
        IElement v1(RealType, 2.25);
        double v2(100.0);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-97.75);
    }

    {
        cout << "Testing qint64 - RealType Subtraction ..." << endl;
        qint64 v1(2);
        IElement v2(RealType, 100.25);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-98.25);
    }

    {
        cout << "Testing double - RealType Subtraction ..." << endl;
        double v1(2.25);
        IElement v2(RealType, 100.25);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-98.0);
    }

    // Relation <
    {
        cout << "Testing RealType < RealType Relation (true) ..." << endl;
        IElement v1(RealType, 2.26);
        IElement v2(RealType, 100.26);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType < RealType Relation (false) ..." << endl;
        IElement v1(RealType, 200.765);
        IElement v2(RealType, 100.876);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType < qint64 Relation (true) ..." << endl;
        IElement v1(RealType, 2.979);
        qint64 v2(100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType < qint64 Relation (false) ..." << endl;
        IElement v1(RealType, 200.898);
        qint64 v2(100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType < double Relation (true) ..." << endl;
        IElement v1(RealType, 2.989);
        double v2(100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType < double Relation (false) ..." << endl;
        IElement v1(RealType, 200.846);
        double v2(100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 < RealType Relation (true) ..." << endl;
        qint64 v1(2);
        IElement v2(RealType, 2.0001);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 < RealType Relation (false) ..." << endl;
        qint64 v1(200);
        IElement v2(RealType, 199.999);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double < RealType Relation (true) ..." << endl;
        double v1(2.76565);
        IElement v2(RealType, 2.76565009);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double < RealType Relation (false) ..." << endl;
        double v1(200.0);
        IElement v2(RealType, 100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    // Relation >
    {
        cout << "Testing RealType > RealType Relation (true) ..." << endl;
        IElement v1(RealType, 20.7);
        IElement v2(RealType, 20.667);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType > RealType Relation (false) ..." << endl;
        IElement v1(RealType, 2.09);
        IElement v2(RealType, 100.998);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType > qint64 Relation (true) ..." << endl;
        IElement v1(RealType, 2.09);
        qint64 v2(2);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType > qint64 Relation (false) ..." << endl;
        IElement v1(RealType, 99.99);
        qint64 v2(100);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType > double Relation (true) ..." << endl;
        IElement v1(RealType, 2.01);
        double v2(2.001);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType > double Relation (false) ..." << endl;
        IElement v1(RealType, 2.01);
        double v2(2.01);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    // Relation <=
    {
        cout << "Testing RealType <= RealType Relation (true, strictly) ..." << endl;
        IElement v1(RealType, 2.0);
        IElement v2(RealType, 100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= RealType Relation (true, equality) ..." << endl;
        IElement v1(RealType, 100.0009);
        IElement v2(RealType, 100.0009);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= RealType Relation (false) ..." << endl;
        IElement v1(RealType, 200.08);
        IElement v2(RealType, 100.57);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType <= qint64 Relation (true, strictly) ..." << endl;
        IElement v1(RealType, 99.999);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= qint64 Relation (true, equality) ..." << endl;
        IElement v1(RealType, 100.0);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= qint64 Relation (false) ..." << endl;
        IElement v1(RealType, 200.90);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType <= double Relation (true, strictly) ..." << endl;
        IElement v1(RealType, 2.90);
        double v2(100.09);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= double Relation (true, equality) ..." << endl;
        IElement v1(RealType, 100.0001);
        double v2(100.0001);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= double Relation (false) ..." << endl;
        IElement v1(RealType, 200.0909);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 <= RealType Relation (true, strictly) ..." << endl;
        qint64 v1(2);
        IElement v2(RealType, 100.25);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 <= RealType Relation (true, equality) ..." << endl;
        qint64 v1(100);
        IElement v2(RealType, 100.00);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 <= RealType Relation (false) ..." << endl;
        qint64 v1(200);
        IElement v2(RealType, 100.25);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double <= RealType Relation (true, strictly) ..." << endl;
        double v1(2.25);
        IElement v2(RealType, 100.25);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double <= RealType Relation (true, equality) ..." << endl;
        double v1(100.252525252525);
        IElement v2(RealType, 100.252525252525);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double <= RealType Relation (false) ..." << endl;
        double v1(200.2525);
        IElement v2(RealType, 100.2525);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    // Relation >=
    {
        cout << "Testing RealType >= RealType Relation (true, strictly) ..." << endl;
        IElement v1(RealType, 200.06);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= RealType Relation (true, equality) ..." << endl;
        IElement v1(RealType, 100.06);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= RealType Relation (false) ..." << endl;
        IElement v1(RealType, 2.06);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType >= qint64 Relation (true, strictly) ..." << endl;
        IElement v1(RealType, 200.06);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= qint64 Relation (true, equality) ..." << endl;
        IElement v1(RealType, 100.0);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= qint64 Relation (false) ..." << endl;
        IElement v1(RealType, 2.06);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType >= double Relation (true, strictly) ..." << endl;
        IElement v1(RealType, 200.06);
        double v2(100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= double Relation (true, equality) ..." << endl;
        IElement v1(RealType, 100.0);
        double v2(100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= double Relation (false) ..." << endl;
        IElement v1(RealType, 2.06);
        double v2(100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 >= RealType Relation (true, strictly) ..." << endl;
        qint64 v1(200);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 >= RealType Relation (true, equality) ..." << endl;
        qint64 v1(100);
        IElement v2(RealType, 100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 >= RealType Relation (false) ..." << endl;
        qint64 v1(100);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double >= RealType Relation (true, strictly) ..." << endl;
        double v1(200.0);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double >= RealType Relation (true, equality) ..." << endl;
        double v1(100.06);
        IElement v2(RealType, 100.06);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    // Relation ==
    {
        cout << "Testing RealType == RealType Relation (false)..." << endl;
        IElement v1(RealType, 2.0908);
        IElement v2(RealType, 100.0908);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType == RealType Relation (true)..." << endl;
        IElement v1(RealType, 100.0908);
        IElement v2(RealType, 100.0908);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType == qint64 Relation (false)..." << endl;
        IElement v1(RealType, 2.0908);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType == qint64 Relation (true)..." << endl;
        IElement v1(RealType, 100.0);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType == double Relation (false)..." << endl;
        IElement v1(RealType, 2.0908);
        double v2(100.0908);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType == double Relation (true)..." << endl;
        IElement v1(RealType, 100.0908);
        double v2(100.0908);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 == RealType Relation (true)..." << endl;
        qint64 v1(100);
        IElement v2(RealType, 100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double == RealType Relation (false)..." << endl;
        double v1(2.0);
        IElement v2(RealType, 100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double == RealType Relation (true)..." << endl;
        double v1(100.009);
        IElement v2(RealType, 100.009);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    // Relation !=
    {
        cout << "Testing RealType != RealType Relation (true)  ..." << endl;
        IElement v1(RealType, 2.08);
        IElement v2(RealType, 100.08);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType != RealType Relation (false) ..." << endl;
        IElement v1(RealType, 100.08);
        IElement v2(RealType, 100.08);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType != qint64 Relation (true)  ..." << endl;
        IElement v1(RealType, 2.08);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType != qint64 Relation (false) ..." << endl;
        IElement v1(RealType, 100.0);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType != double Relation (true)  ..." << endl;
        IElement v1(RealType, 2.08);
        double v2(100.08);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType != double Relation (false) ..." << endl;
        IElement v1(RealType, 100.0);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 != RealType Relation (true)  ..." << endl;
        qint64 v1(2);
        IElement v2(RealType, 100.08);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 != RealType Relation (false) ..." << endl;
        qint64 v1(100);
        IElement v2(RealType, 100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double != RealType Relation (true)  ..." << endl;
        double v1(2.0);
        IElement v2(RealType, 100.08);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double != RealType Relation (false) ..." << endl;
        double v1(100.08);
        IElement v2(RealType, 100.08);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }
}

void tst_baseoperations::test_IntegerTypeOperations()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    // Negation
    {
        cout << "Testing IntegerType Negation ..." << endl;
        IElement v1(IntegerType, 1);
        IElement v2 = -v1;
        QCOMPARE(v2.value<int>(),-1);
    }

    // Multiplication
    {
        cout << "Testing IntegerType * IntegerType Multiplication ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<int>(), 200);
    }

    {
        cout << "Testing IntegerType * qint64 Multiplication ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(200);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<int>(), 400);
    }

    {
        cout << "Testing IntegerType * double Multiplication ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 200.0);
    }

    {
        cout << "Testing qint64 * IntegerType Multiplication ..." << endl;
        qint64 v1(200);
        IElement v2(IntegerType, 2);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<int>(), 400);
    }

    {
        cout << "Testing double * IntegerType Multiplication ..." << endl;
        double v1(100.0);
        IElement v2(IntegerType, 2);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 200.0);
    }

    // Division
    {
        cout << "Testing IntegerType/IntegerType Division ..." << endl;
        IElement v1(IntegerType, 300);
        IElement v2(IntegerType, 100);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<int>(), 3);
    }

    {
        cout << "Testing IntegerType/qint64 Division ..." << endl;
        IElement v1(IntegerType, 300);
        qint64 v2(100);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<int>(), 3);
    }

    {
        cout << "Testing IntegerType/double Division ..." << endl;
        IElement v1(IntegerType, 300);
        double v2(100.0);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 3.0);
    }

    {
        cout << "Testing qint64/IntegerType Division ..." << endl;
        qint64 v1(300);
        IElement v2(IntegerType, 100);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<int>(), 3);
    }

    {
        cout << "Testing double/IntegerType Division ..." << endl;
        double v1(300.0);
        IElement v2(IntegerType, 100);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 3.0);
    }

    // Division by Zero
    {
        cout << "Testing IntegerType Division by Zero ..." << endl;
        IElement v1(IntegerType, 300);
        IElement v2(IntegerType, 0);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), DivisionByZeroException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Inexact Division
    {
        cout << "Testing IntegerType/IntegerType Inexact Division ..." << endl;
        IElement v1(IntegerType, 300);
        IElement v2(IntegerType, 101);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<int>(), 2);
    }

    // Addition
    {
        cout << "Testing IntegerType + IntegerType Addition ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<int>(), 102);
    }

    {
        cout << "Testing IntegerType + qint64 Addition ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<int>(), 102);
    }

    {
        cout << "Testing IntegerType + double Addition ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.0);
    }

    {
        cout << "Testing qint64 + IntegerType Addition ..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<int>(), 102);
    }

    {
        cout << "Testing double + IntegerType Addition ..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.0);
    }

    // Subtraction
    {
        cout << "Testing IntegerType - IntegerType Subtraction ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<int>(),-98);
    }

    {
        cout << "Testing IntegerType - qint64 Subtraction ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<int>(),-98);
    }

    {
        cout << "Testing IntegerType - double Subtraction ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-98.0);
    }

    {
        cout << "Testing qint64 - IntegerType Subtraction ..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<int>(),-98);
    }

    {
        cout << "Testing double - IntegerType Subtraction ..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-98.0);
    }

    // Relation <
    {
        cout << "Testing IntegerType < IntegerType Relation (true) ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType < IntegerType Relation (false) ..." << endl;
        IElement v1(IntegerType, 200);
        IElement v2(IntegerType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType < qint64 Relation (true) ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType < qint64 Relation (false) ..." << endl;
        IElement v1(IntegerType, 200);
        qint64 v2(100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType < double Relation (true) ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType < double Relation (false) ..." << endl;
        IElement v1(IntegerType, 200);
        double v2(100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 < IntegerType Relation (true) ..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 < IntegerType Relation (false) ..." << endl;
        qint64 v1(200);
        IElement v2(IntegerType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double < IntegerType Relation (true) ..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double < IntegerType Relation (false) ..." << endl;
        double v1(200.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    // Relation >
    {
        cout << "Testing IntegerType > IntegerType Relation (false) ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType > IntegerType Relation (true) ..." << endl;
        IElement v1(IntegerType, 20);
        IElement v2(IntegerType, 2);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType > qint64 Relation (false) ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType > qint64 Relation (true) ..." << endl;
        IElement v1(IntegerType, 20);
        qint64 v2(2);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType > double Relation (false) ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType > double Relation (true) ..." << endl;
        IElement v1(IntegerType, 20);
        double v2(2.0);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    // Relation <=
    {
        cout << "Testing IntegerType <= IntegerType Relation (true, strictly) ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= IntegerType Relation (true, equality) ..." << endl;
        IElement v1(IntegerType, 100);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= IntegerType Relation (false) ..." << endl;
        IElement v1(IntegerType, 200);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType <= qint64 Relation (true, strictly) ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= qint64 Relation (true, equality) ..." << endl;
        IElement v1(IntegerType, 100);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= qint64 Relation (false) ..." << endl;
        IElement v1(IntegerType, 200);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType <= double Relation (true, strictly) ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= double Relation (true, equality) ..." << endl;
        IElement v1(IntegerType, 100);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= double Relation (false) ..." << endl;
        IElement v1(IntegerType, 200);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 <= IntegerType Relation (true, strictly) ..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 <= IntegerType Relation (true, equality) ..." << endl;
        qint64 v1(100);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 <= IntegerType Relation (false) ..." << endl;
        qint64 v1(200);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double <= IntegerType Relation (true, strictly) ..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double <= IntegerType Relation (true, equality) ..." << endl;
        double v1(100.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double <= IntegerType Relation (false) ..." << endl;
        double v1(200);
        IElement v2(IntegerType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    // Relation >=
    {
        cout << "Testing IntegerType >= IntegerType Relation (true, strictly) ..." << endl;
        IElement v1(IntegerType, 200);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= IntegerType Relation (true, equality) ..." << endl;
        IElement v1(IntegerType, 100);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= IntegerType Relation (false) ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType >= qint64 Relation (true, strictly) ..." << endl;
        IElement v1(IntegerType, 200);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= qint64 Relation (true, equality) ..." << endl;
        IElement v1(IntegerType, 100);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= qint64 Relation (false) ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType >= double Relation (true, strictly) ..." << endl;
        IElement v1(IntegerType, 200);
        double v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= double Relation (true, equality) ..." << endl;
        IElement v1(IntegerType, 100);
        double v2(100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= double Relation (false) ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 >= IntegerType Relation (true, strictly) ..." << endl;
        qint64 v1(200);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 >= IntegerType Relation (true, equality) ..." << endl;
        qint64 v1(100);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 >= IntegerType Relation (false) ..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double >= IntegerType Relation (true, strictly) ..." << endl;
        double v1(200.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double >= IntegerType Relation (true, equality) ..." << endl;
        double v1(100.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double >= IntegerType Relation (false) ..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    // Relation ==
    {
        cout << "Testing IntegerType == IntegerType Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType == IntegerType Relation (true)..." << endl;
        IElement v1(IntegerType, 100);
        IElement v2(IntegerType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType == qint64 Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType == qint64 Relation (true)..." << endl;
        IElement v1(IntegerType, 100);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType == double Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType == double Relation (true)..." << endl;
        IElement v1(IntegerType, 100);
        double v2(100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 == IntegerType Relation (false)..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 == IntegerType Relation (true)..." << endl;
        qint64 v1(100);
        IElement v2(IntegerType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double == IntegerType Relation (false)..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double == IntegerType Relation (true)..." << endl;
        double v1(100.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    // Relation !=
    {
        cout << "Testing IntegerType != IntegerType Relation (true)  ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(IntegerType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType != IntegerType Relation (false) ..." << endl;
        IElement v1(IntegerType, 100);
        IElement v2(IntegerType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType != qint64 Relation (true)  ..." << endl;
        IElement v1(IntegerType, 2);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType != qint64 Relation (false) ..." << endl;
        IElement v1(IntegerType, 100);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType != double Relation (true)  ..." << endl;
        IElement v1(IntegerType, 2);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType != double Relation (false) ..." << endl;
        IElement v1(IntegerType, 100);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 != IntegerType Relation (true)  ..." << endl;
        qint64 v1(2);
        IElement v2(IntegerType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 != IntegerType Relation (false) ..." << endl;
        qint64 v1(100);
        IElement v2(IntegerType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double != IntegerType Relation (true)  ..." << endl;
        double v1(2.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double != IntegerType Relation (false) ..." << endl;
        double v1(100.0);
        IElement v2(IntegerType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }
}

void tst_baseoperations::test_OrdinalTypeOperations()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    // Negation
    {
        cout << "Testing OrdinalType Negation ..." << endl;
        IElement v1(OrdinalType, 1);
        IElement v2 = -v1;
        QCOMPARE(v2.value<int>(),-1);
    }

    // Multiplication
    {
        cout << "Testing OrdinalType * OrdinalType Multiplication ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing OrdinalType * qint64 Multiplication ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(200);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing OrdinalType * double Multiplication ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 * OrdinalType Multiplication ..." << endl;
        qint64 v1(200);
        IElement v2(OrdinalType, 2);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double * OrdinalType Multiplication ..." << endl;
        double v1(100.0);
        IElement v2(OrdinalType, 2);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Division
    {
        cout << "Testing OrdinalType/OrdinalType Division ..." << endl;
        IElement v1(OrdinalType, 300);
        IElement v2(OrdinalType, 100);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing OrdinalType/qint64 Division ..." << endl;
        IElement v1(OrdinalType, 300);
        qint64 v2(100);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing OrdinalType/double Division ..." << endl;
        IElement v1(OrdinalType, 300);
        double v2(100.0);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64/OrdinalType Division ..." << endl;
        qint64 v1(300);
        IElement v2(OrdinalType, 100);
        try {
            IElement rs = v1/v2;
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double/OrdinalType Division ..." << endl;
        double v1(300.0);
        IElement v2(OrdinalType, 100);
        try {
            IElement rs = v1/v2;
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidOrdinalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Division by Zero
    {
        cout << "Testing OrdinalType Division by Zero ..." << endl;
        IElement v1(OrdinalType, 300);
        IElement v2(OrdinalType, 0);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), DivisionByZeroException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Addition
    {
        cout << "Testing OrdinalType + OrdinalType Addition ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<int>(), 102);
    }

    {
        cout << "Testing OrdinalType + qint64 Addition ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<int>(), 102);
    }

    {
        cout << "Testing OrdinalType + double Addition ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.0);
    }

    {
        cout << "Testing qint64 + OrdinalType Addition ..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<int>(), 102);
    }

    {
        cout << "Testing double + OrdinalType Addition ..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 102.0);
    }

    // Subtraction
    {
        cout << "Testing OrdinalType - OrdinalType Subtraction ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<int>(),-98);
    }

    {
        cout << "Testing OrdinalType - qint64 Subtraction ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<int>(),-98);
    }

    {
        cout << "Testing OrdinalType - double Subtraction ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-98.0);
    }

    {
        cout << "Testing qint64 - OrdinalType Subtraction ..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<int>(),-98);
    }

    {
        cout << "Testing double - OrdinalType Subtraction ..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-98.0);
    }

    // Relation <
    {
        cout << "Testing OrdinalType < OrdinalType Relation (true) ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType < OrdinalType Relation (false) ..." << endl;
        IElement v1(OrdinalType, 200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType < qint64 Relation (true) ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType < qint64 Relation (false) ..." << endl;
        IElement v1(OrdinalType, 200);
        qint64 v2(100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType < double Relation (true) ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType < double Relation (false) ..." << endl;
        IElement v1(OrdinalType, 200);
        double v2(100.0);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 < OrdinalType Relation (true) ..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 < OrdinalType Relation (false) ..." << endl;
        qint64 v1(200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double < OrdinalType Relation (true) ..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double < OrdinalType Relation (false) ..." << endl;
        double v1(200.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    // Relation >
    {
        cout << "Testing OrdinalType > OrdinalType Relation (false) ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType > OrdinalType Relation (true) ..." << endl;
        IElement v1(OrdinalType, 20);
        IElement v2(OrdinalType, 2);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType > qint64 Relation (false) ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType > qint64 Relation (true) ..." << endl;
        IElement v1(OrdinalType, 20);
        qint64 v2(2);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType > double Relation (false) ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType > double Relation (true) ..." << endl;
        IElement v1(OrdinalType, 20);
        double v2(2.0);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    // Relation <=
    {
        cout << "Testing OrdinalType <= OrdinalType Relation (true, strictly) ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType <= OrdinalType Relation (true, equality) ..." << endl;
        IElement v1(OrdinalType, 100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType <= OrdinalType Relation (false) ..." << endl;
        IElement v1(OrdinalType, 200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType <= qint64 Relation (true, strictly) ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType <= qint64 Relation (true, equality) ..." << endl;
        IElement v1(OrdinalType, 100);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType <= qint64 Relation (false) ..." << endl;
        IElement v1(OrdinalType, 200);
        qint64 v2(100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType <= double Relation (true, strictly) ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType <= double Relation (true, equality) ..." << endl;
        IElement v1(OrdinalType, 100);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType <= double Relation (false) ..." << endl;
        IElement v1(OrdinalType, 200);
        double v2(100.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 <= OrdinalType Relation (true, strictly) ..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 <= OrdinalType Relation (true, equality) ..." << endl;
        qint64 v1(100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 <= OrdinalType Relation (false) ..." << endl;
        qint64 v1(200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double <= OrdinalType Relation (true, strictly) ..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double <= OrdinalType Relation (true, equality) ..." << endl;
        double v1(100.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double <= OrdinalType Relation (false) ..." << endl;
        double v1(200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    // Relation >=
    {
        cout << "Testing OrdinalType >= OrdinalType Relation (true, strictly) ..." << endl;
        IElement v1(OrdinalType, 200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType >= OrdinalType Relation (true, equality) ..." << endl;
        IElement v1(OrdinalType, 100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType >= OrdinalType Relation (false) ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType >= qint64 Relation (true, strictly) ..." << endl;
        IElement v1(OrdinalType, 200);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType >= qint64 Relation (true, equality) ..." << endl;
        IElement v1(OrdinalType, 100);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType >= qint64 Relation (false) ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType >= double Relation (true, strictly) ..." << endl;
        IElement v1(OrdinalType, 200);
        double v2(100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType >= double Relation (true, equality) ..." << endl;
        IElement v1(OrdinalType, 100);
        double v2(100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType >= double Relation (false) ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 >= OrdinalType Relation (true, strictly) ..." << endl;
        qint64 v1(200);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 >= OrdinalType Relation (true, equality) ..." << endl;
        qint64 v1(100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 >= OrdinalType Relation (false) ..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double >= OrdinalType Relation (true, strictly) ..." << endl;
        double v1(200.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double >= OrdinalType Relation (true, equality) ..." << endl;
        double v1(100.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double >= OrdinalType Relation (false) ..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    // Relation ==
    {
        cout << "Testing OrdinalType == OrdinalType Relation (false)..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType == OrdinalType Relation (true)..." << endl;
        IElement v1(OrdinalType, 100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType == qint64 Relation (false)..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType == qint64 Relation (true)..." << endl;
        IElement v1(OrdinalType, 100);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType == double Relation (false)..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType == double Relation (true)..." << endl;
        IElement v1(OrdinalType, 100);
        double v2(100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 == OrdinalType Relation (false)..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 == OrdinalType Relation (true)..." << endl;
        qint64 v1(100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double == OrdinalType Relation (false)..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double == OrdinalType Relation (true)..." << endl;
        double v1(100.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    // Relation !=
    {
        cout << "Testing OrdinalType != OrdinalType Relation (true)  ..." << endl;
        IElement v1(OrdinalType, 2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType != OrdinalType Relation (false) ..." << endl;
        IElement v1(OrdinalType, 100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType != qint64 Relation (true)  ..." << endl;
        IElement v1(OrdinalType, 2);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType != qint64 Relation (false) ..." << endl;
        IElement v1(OrdinalType, 100);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing OrdinalType != double Relation (true)  ..." << endl;
        IElement v1(OrdinalType, 2);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing OrdinalType != double Relation (false) ..." << endl;
        IElement v1(OrdinalType, 100);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 != OrdinalType Relation (true)  ..." << endl;
        qint64 v1(2);
        IElement v2(OrdinalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 != OrdinalType Relation (false) ..." << endl;
        qint64 v1(100);
        IElement v2(OrdinalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double != OrdinalType Relation (true)  ..." << endl;
        double v1(2.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double != OrdinalType Relation (false) ..." << endl;
        double v1(100.0);
        IElement v2(OrdinalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }
}

void tst_baseoperations::test_NominalTypeOperations()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    // Negation
    {
        cout << "Testing NominalType Negation ..." << endl;
        IElement v1(NominalType, 1);
        try {
            IElement v2 = -v1;
            T_UNUSED(v2);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Multiplication
    {
        cout << "Testing NominalType * NominalType Multiplication ..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType * qint64 Multiplication ..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(200);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType * double Multiplication ..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 * NominalType Multiplication ..." << endl;
        qint64 v1(200);
        IElement v2(NominalType, 2);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double * NominalType Multiplication ..." << endl;
        double v1(100.0);
        IElement v2(NominalType, 2);
        try {
            IElement rs = v1*v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Division
    {
        cout << "Testing NominalType/NominalType Division ..." << endl;
        IElement v1(NominalType, 300);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType/qint64 Division ..." << endl;
        IElement v1(NominalType, 300);
        qint64 v2(100);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType/double Division ..." << endl;
        IElement v1(NominalType, 300);
        double v2(100.0);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64/NominalType Division ..." << endl;
        qint64 v1(300);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double/NominalType Division ..." << endl;
        double v1(300.0);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Division by Zero
    {
        cout << "Testing NominalType Division by Zero ..." << endl;
        IElement v1(NominalType, 300);
        IElement v2(NominalType, 0);
        try {
            IElement rs = v1/v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), DivisionByZeroException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Addition
    {
        cout << "Testing NominalType + NominalType Addition ..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1+v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType + qint64 Addition ..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(100);
        try {
            IElement rs = v1+v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType + double Addition ..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        try {
            IElement rs = v1+v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 + NominalType Addition ..." << endl;
        qint64 v1(2);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1+v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double + NominalType Addition ..." << endl;
        double v1(2.0);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1+v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Subtraction
    {
        cout << "Testing NominalType - NominalType Subtraction ..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1-v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType - qint64 Subtraction ..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(100);
        try {
            IElement rs = v1-v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType - double Subtraction ..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        try {
            IElement rs = v1-v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 - NominalType Subtraction ..." << endl;
        qint64 v1(2);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1-v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double - NominalType Subtraction ..." << endl;
        double v1(2.0);
        IElement v2(NominalType, 100);
        try {
            IElement rs = v1-v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Relation <
    {
        cout << "Testing NominalType < NominalType Relation ..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1<v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType < qint64 Relation ..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(100);
        try {
            tribool rs = v1<v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType < double Relation ..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        try {
            tribool rs = v1<v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 < NominalType Relation ..." << endl;
        qint64 v1(2);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1<v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double < NominalType Relation ..." << endl;
        double v1(2.0);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1<v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Relation >
    {
        cout << "Testing NominalType > NominalType Relation ..." << endl;
        IElement v1(NominalType, 20);
        IElement v2(NominalType, 2);
        try {
            tribool rs = v1>v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType > qint64 Relation ..." << endl;
        IElement v1(NominalType, 20);
        qint64 v2(2);
        try {
            tribool rs = v1>v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType > double Relation ..." << endl;
        IElement v1(NominalType, 20);
        double v2(2.0);
        try {
            tribool rs = v1>v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Relation <=
    {
        cout << "Testing NominalType <= NominalType Relation ..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1<=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType <= qint64 Relation ..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(100);
        try {
            tribool rs = v1<=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType <= double Relation ..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        try {
            tribool rs = v1<=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 <= NominalType Relation ..." << endl;
        qint64 v1(2);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1<=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double <= NominalType Relation ..." << endl;
        double v1(2.0);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1<=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Relation >=
    {
        cout << "Testing NominalType >= NominalType Relation ..." << endl;
        IElement v1(NominalType, 200);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1>=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType >= qint64 Relation ..." << endl;
        IElement v1(NominalType, 200);
        qint64 v2(100);
        try {
            tribool rs = v1>=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing NominalType >= double Relation ..." << endl;
        IElement v1(NominalType, 200);
        double v2(100);
        try {
            tribool rs = v1>=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing qint64 >= NominalType Relation ..." << endl;
        qint64 v1(200);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1>=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    {
        cout << "Testing double >= NominalType Relation ..." << endl;
        double v1(200.0);
        IElement v2(NominalType, 100);
        try {
            tribool rs = v1>=v2;
            T_UNUSED(rs);
        } catch(TException& e) {
            cout << "Caught TException" << endl;
            bool rs = endsWith(e.what(), InvalidNominalOpException);
            QCOMPARE(rs, true);
        } catch(...) {
            qDebug() << "Couldn't catch TException";
            QFAIL("Unknown error.");
        }
    }

    // Relation ==
    {
        cout << "Testing NominalType == NominalType Relation (false)..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing NominalType == NominalType Relation (true)..." << endl;
        IElement v1(NominalType, 100);
        IElement v2(NominalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing NominalType == qint64 Relation (false)..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing NominalType == qint64 Relation (true)..." << endl;
        IElement v1(NominalType, 100);
        qint64 v2(100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing NominalType == double Relation (false)..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing NominalType == double Relation (true)..." << endl;
        IElement v1(NominalType, 100);
        double v2(100.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 == NominalType Relation (false)..." << endl;
        qint64 v1(2);
        IElement v2(NominalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 == NominalType Relation (true)..." << endl;
        qint64 v1(100);
        IElement v2(NominalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double == NominalType Relation (false)..." << endl;
        double v1(2.0);
        IElement v2(NominalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double == NominalType Relation (true)..." << endl;
        double v1(100.0);
        IElement v2(NominalType, 100);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    // Relation !=
    {
        cout << "Testing NominalType != NominalType Relation (true)  ..." << endl;
        IElement v1(NominalType, 2);
        IElement v2(NominalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing NominalType != NominalType Relation (false) ..." << endl;
        IElement v1(NominalType, 100);
        IElement v2(NominalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing NominalType != qint64 Relation (true)  ..." << endl;
        IElement v1(NominalType, 2);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing NominalType != qint64 Relation (false) ..." << endl;
        IElement v1(NominalType, 100);
        qint64 v2(100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing NominalType != double Relation (true)  ..." << endl;
        IElement v1(NominalType, 2);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing NominalType != double Relation (false) ..." << endl;
        IElement v1(NominalType, 100);
        double v2(100.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing qint64 != NominalType Relation (true)  ..." << endl;
        qint64 v1(2);
        IElement v2(NominalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing qint64 != NominalType Relation (false) ..." << endl;
        qint64 v1(100);
        IElement v2(NominalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing double != NominalType Relation (true)  ..." << endl;
        double v1(2.0);
        IElement v2(NominalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing double != NominalType Relation (false) ..." << endl;
        double v1(100.0);
        IElement v2(NominalType, 100);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }
}

void tst_baseoperations::test_TypeMixedOperations()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    // *
    {
        cout << "Testing IntegerType * RealType Relation ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.25);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 4.5);
    }

    {
        cout << "Testing RealType * IntegerType  Relation ..." << endl;
        IElement v1(RealType, 2.25);
        IElement v2(IntegerType, 2);
        IElement rs = v1*v2;
        QCOMPARE(rs.value<double>(), 4.5);
    }

    // /
    {
        cout << "Testing IntegerType / RealType Relation ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 0.5);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 4.0);
    }

    {
        cout << "Testing RealType / IntegerType  Relation ..." << endl;
        IElement v1(RealType, 2.5);
        IElement v2(IntegerType, 2);
        IElement rs = v1/v2;
        QCOMPARE(rs.value<double>(), 1.25);
    }

    // +

    {
        cout << "Testing IntegerType + RealType Relation ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.25);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 4.25);
    }

    {
        cout << "Testing RealType + IntegerType  Relation ..." << endl;
        IElement v1(RealType, 2.25);
        IElement v2(IntegerType, 2);
        IElement rs = v1+v2;
        QCOMPARE(rs.value<double>(), 4.25);
    }

    // -

    {
        cout << "Testing IntegerType - RealType Relation ..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.25);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(),-0.25);
    }

    {
        cout << "Testing RealType - IntegerType  Relation ..." << endl;
        IElement v1(RealType, 2.25);
        IElement v2(IntegerType, 2);
        IElement rs = v1-v2;
        QCOMPARE(rs.value<double>(), 0.25);
    }

    // <

    {
        cout << "Testing IntegerType < RealType Relation (true)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.25);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType < RealType Relation (false)..." << endl;
        IElement v1(IntegerType, 3);
        IElement v2(RealType, 2.25);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType < IntegerType Relation (true)..." << endl;
        IElement v1(RealType, 2.5);
        IElement v2(IntegerType, 3);
        tribool rs = v1<v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType < IntegerType Relation (false)..." << endl;
        IElement v1(RealType, 3.5);
        IElement v2(IntegerType, 3);
        tribool rs = v1<v2;
        TCOMPARE(rs, false);
    }

    // >

    {
        cout << "Testing IntegerType > RealType Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.25);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType > RealType Relation (true)..." << endl;
        IElement v1(IntegerType, 3);
        IElement v2(RealType, 2.25);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType > IntegerType Relation (false)..." << endl;
        IElement v1(RealType, 2.5);
        IElement v2(IntegerType, 3);
        tribool rs = v1>v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType > IntegerType Relation (true)..." << endl;
        IElement v1(RealType, 3.5);
        IElement v2(IntegerType, 3);
        tribool rs = v1>v2;
        TCOMPARE(rs, true);
    }

    // <=

    {
        cout << "Testing IntegerType <= RealType Relation (true <- euqality)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.0);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= RealType Relation (true <- strictly)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.1);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType <= RealType Relation (false)..." << endl;
        IElement v1(IntegerType, 3);
        IElement v2(RealType, 2.25);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType <= IntegerType Relation (true <- equality)..." << endl;
        IElement v1(RealType, 3.0);
        IElement v2(IntegerType, 3);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= IntegerType Relation (true <- strictly)..." << endl;
        IElement v1(RealType, 2.8);
        IElement v2(IntegerType, 3);
        tribool rs = v1<=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType <= IntegerType Relation (false)..." << endl;
        IElement v1(RealType, 3.5);
        IElement v2(IntegerType, 3);
        tribool rs = v1<=v2;
        TCOMPARE(rs, false);
    }

    // >=

    {
        cout << "Testing IntegerType >= RealType Relation (true <- euqality)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.0);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= RealType Relation (true <- strictly)..." << endl;
        IElement v1(IntegerType, 3);
        IElement v2(RealType, 2.1);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType >= RealType Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.25);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType >= IntegerType Relation (true <- equality)..." << endl;
        IElement v1(RealType, 3.0);
        IElement v2(IntegerType, 3);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= IntegerType Relation (true <- strictly)..." << endl;
        IElement v1(RealType, 3.8);
        IElement v2(IntegerType, 3);
        tribool rs = v1>=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType >= IntegerType Relation (false)..." << endl;
        IElement v1(RealType, 2.5);
        IElement v2(IntegerType, 3);
        tribool rs = v1>=v2;
        TCOMPARE(rs, false);
    }

    // ==
    {
        cout << "Testing IntegerType == RealType Relation (true)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.0);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IntegerType == RealType Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.00001);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType == IntegerType Relation (true)..." << endl;
        IElement v1(RealType, 2.0);
        IElement v2(IntegerType, 2);
        tribool rs = v1==v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType == IntegerType Relation (false)..." << endl;
        IElement v1(RealType, 2.00001);
        IElement v2(IntegerType, 2);
        tribool rs = v1==v2;
        TCOMPARE(rs, false);
    }

    // !=
    {
        cout << "Testing IntegerType != RealType Relation (false)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.0);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IntegerType != RealType Relation (true)..." << endl;
        IElement v1(IntegerType, 2);
        IElement v2(RealType, 2.00001);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing RealType != IntegerType Relation (false)..." << endl;
        IElement v1(RealType, 2.0);
        IElement v2(IntegerType, 2);
        tribool rs = v1!=v2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing RealType != IntegerType Relation (true)..." << endl;
        IElement v1(RealType, 2.00001);
        IElement v2(RealType, 2);
        tribool rs = v1!=v2;
        TCOMPARE(rs, true);
    }
}

void tst_baseoperations::test_Problem_constructors()
{
    Problem prob;
    ProblemSPtr probPtr(new Problem);
    Problem prob2(prob);
    ProblemSPtr probPtr2 = probPtr->clone();

    ProblemDefinitionStatus defined = prob.isProblemDefined();
    QCOMPARE(defined, Tigon::Undefined);
}

void tst_baseoperations::test_Problem_problemProperties()
{
    ProblemPropertiesFactory* fct = ProblemPropertiesFactory::instance();
    ProblemProperties probPrpts = fct->createProblemProperties(TString("problem name"),
                                                               TString("problem description"));
    Problem prob;
    prob.defineProbProperties(probPrpts);

    ProblemProperties prpts = prob.problemProperties();
    QCOMPARE(prpts.name(),        TString("problem name"));
    QCOMPARE(prpts.description(), TString("problem description"));
    QCOMPARE(prpts.ID(),          probPrpts.ID());
}

void tst_baseoperations::test_Problem_vectorProperties()
{
    TVector<ElementType> typeVec;
    typeVec << RealType << IntegerType << OrdinalType << NominalType;

    Problem prob;
    prob.defineDVecTypes(typeVec);

    fill(typeVec, RealType, 2);
    prob.defineOVecTypes(typeVec);

    fill(typeVec, IntegerType, 3);
    prob.definePVecTypes(typeVec);

    TVector<ElementProperties> prpts = prob.dPrpts();
    QCOMPARE((int)prpts.size(), 4);
    prpts = prob.oPrpts();
    QCOMPARE((int)prpts.size(), 2);
    prpts = prob.pPrpts();
    QCOMPARE((int)prpts.size(), 3);
    prpts = prob.cPrpts();
    QCOMPARE((int)prpts.size(), 0);
    fill(typeVec, NominalType, 1);
    prob.defineCVecTypes(typeVec);


    TString elemName = prob.dVecNames()[3];
    QCOMPARE(elemName, TString("Variable 3"));
    elemName = prob.oPrpts()[1].name();
    QCOMPARE(elemName, TString("Objective 1"));
    elemName = prob.pPrpts()[0].name();
    QCOMPARE(elemName, TString("Parameter 0"));
    elemName = prob.cPrpts()[0].name();
    QCOMPARE(elemName, TString("Constraint 0"));

    ElementType elemtype = prob.dPrpts()[2].type();
    QCOMPARE(elemtype, OrdinalType);
    elemtype = prob.oPrpts()[0].type();
    QCOMPARE(elemtype, RealType);
    elemtype = prob.pPrpts()[0].type();
    QCOMPARE(elemtype, IntegerType);
    elemtype = prob.cPrpts()[0].type();
    QCOMPARE(elemtype, NominalType);
}

void tst_baseoperations::test_Problem_appendFunc()
{
    IFunctionSPtr func(new ExampleFunction);
    Problem prob;
    int nFunc = prob.functionVec().size();
    QCOMPARE(nFunc, 0);

    // Append without maps
    prob.appendFunction(func);
    prob.processProblemDefinition();

    nFunc = prob.functionVec().size();
    QCOMPARE(nFunc, 1);
    TVector<TVector<int> > varMap = prob.f2dMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 1);
    varMap = prob.f2oMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 1);
    varMap = prob.f2pMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE(varMap[0].empty(), true);
    varMap = prob.f2cMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE(varMap[0].empty(), true);

    // Append with maps
    IFunctionSPtr func2(new ExampleFunction);
    func2->TP_defineNInputs(4);
    func2->TP_defineNOutputs(3);
    TVector<int> params;
    params << 1 << 3;
    TVector<int> objs;
    objs << 1;
    TVector<int> constraints;
    constraints << 2;

    prob.appendFunction(func2, params, constraints, objs);
    prob.processProblemDefinition();

    nFunc = prob.functionVec().size();
    QCOMPARE(nFunc, 2);
    varMap = prob.f2dMap();
    QCOMPARE((int)varMap.size(), 2);
    QCOMPARE((int)varMap[1].size(), 2);
    varMap = prob.f2oMap();
    QCOMPARE((int)varMap.size(), 2);
    QCOMPARE((int)varMap[1].size(), 2);
    varMap = prob.f2pMap();
    QCOMPARE((int)varMap.size(), 2);
    QCOMPARE((int)varMap[1].size(), 2);
    varMap = prob.f2cMap();
    QCOMPARE((int)varMap.size(), 2);
    QCOMPARE((int)varMap[1].size(), 1);
}

void tst_baseoperations::test_Problem_unusedOutputs()
{
    IFunctionSPtr func(new ExampleFunction);
    func->TP_defineNInputs(4);
    func->TP_defineNOutputs(4);

    TVector<int> params;
    params << 1 << 3;
    TVector<int> objs;
    objs << 0 << 1;
    TVector<int> constraints;
    constraints << 1 << 3;

    Problem prob;
    prob.appendFunction(func, params, constraints, objs);
    prob.processProblemDefinition();

    int nFunc;
    nFunc = prob.functionVec().size();
    QCOMPARE(nFunc, 1);

    TVector<TVector<int> > varMap;

    // decision variables
    varMap = prob.f2dMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 2);
    QCOMPARE(varMap[0].at(0), 0);
    QCOMPARE(varMap[0].at(1), 2);

    // objectives
    varMap = prob.f2oMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 2);
    QCOMPARE(varMap[0].at(0), 0);
    QCOMPARE(varMap[0].at(1), 1);

    // parameters
    varMap = prob.f2pMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 2);
    QCOMPARE(varMap[0].at(0), 1);
    QCOMPARE(varMap[0].at(1), 3);

    // constraints
    varMap = prob.f2cMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 2);
    QCOMPARE(varMap[0].at(0), 1);
    QCOMPARE(varMap[0].at(1), 3);

    // unused outputs
    varMap = prob.f2uMap();
    QCOMPARE((int)varMap.size(), 1);
    QCOMPARE((int)varMap[0].size(), 1);
    QCOMPARE(varMap[0].at(0), 2);
}

void tst_baseoperations::test_Problem_defineFuncAndMaps()
{
    IFunctionSPtr func = IFunctionSPtr(new ExampleFunction);
    // Two identical functions with one input and one output. Since the variables
    // are the same in both functions, there is only a single variable and single
    // objective to the problem.
    TVector<IFunctionSPtr> funcVec(2, func);
    Problem prob;

    // define only the function
    prob.defineFunctionVec(funcVec);
    QCOMPARE((int)prob.functionVec().size(), 2);
    QCOMPARE(prob.f2dMap().empty(), true);
    QCOMPARE(prob.f2oMap().empty(), true);
    QCOMPARE(prob.f2pMap().empty(), true);
    QCOMPARE(prob.f2cMap().empty(), true);

    // define maps seperately - dimensions can be wrong if not set correctly
    TVector<int> dMap;
    dMap << 0;
    TVector<TVector<int> > varMap;
    varMap << dMap << dMap;
    prob.defineF2dMap(varMap);
    QCOMPARE((int)prob.f2dMap().size(), 2);
}

void tst_baseoperations::test_Problem_definitionStatus()
{
    Problem prob;
    ProblemDefinitionStatus stat = prob.isProblemDefined();
    QCOMPARE(stat, Undefined);

    prob.processProblemDefinition();
    stat = prob.isProblemDefined();
    QCOMPARE(stat, IllDefinedFunctionVec);


    IFunctionSPtr func1 = IFunctionSPtr(new ExampleFunction);
    IFunctionSPtr func2 = IFunctionSPtr(new ExampleFunction);
    func2->TP_defineNInputs(3);
    func2->TP_defineNOutputs(2);
    // change first decision variable and objective to be different from the first
    // function
    TVector<ElementProperties> prpts = func2->inputPrpts();
    ElementPropertiesFactory* fact = ElementPropertiesFactory::instance();
    prpts[0] = fact->createElementProperties(0, "var name", "var description");
    func2->defineInputPrpts(prpts);
    prpts = func2->outputPrpts();
    prpts[0] = fact->createElementProperties(0, "obj name", "obj description");
    func2->defineOutputPrpts(prpts);
    TVector<IFunctionSPtr> funcVec(2);
    funcVec[0] = func1;
    funcVec[1] = func2;
    prob.defineFunctionVec(funcVec);
    stat = prob.isProblemDefined();
    QCOMPARE(stat, UnprocessedChanges);

    stat = prob.processProblemDefinition();
    QCOMPARE(stat, FullyDefined);

    prob.defineBoxConstraints(BoxConstraintsDataSPtr(new BoxConstraintsData));
    stat = prob.isProblemDefined();
    QCOMPARE(stat, UnprocessedChanges);

    stat = prob.processProblemDefinition();
    QCOMPARE(stat, IllDefinedBoxConstraints);
}

void tst_baseoperations::test_ProblemFormulationMultiFunctions()
{
    int nInFunc1 = 3;
    int nOutFunc1 = 2;
    int nInFunc2 = 2;
    int nOutFunc2 = 1;

    IFunctionSPtr func1 = IFunctionSPtr(new ExampleFunction);
    IFunctionSPtr func2 = IFunctionSPtr(new ExampleFunction);

    func1->TP_defineNInputs(nInFunc1);
    func1->TP_defineNOutputs(nOutFunc1);
    func2->TP_defineNInputs(nInFunc2);
    func2->TP_defineNOutputs(nOutFunc2);

    // Appending Function without specifications of the inputs and outputs
    //prob.appendFunction(func1);
    //prob.appendFunction(func2);

    TVector<ElementProperties> inPrpts1 = func1->inputPrpts();
    TVector<ElementProperties> outPrpts1 = func1->outputPrpts();
    TVector<ElementProperties> inPrpts2 = func2->inputPrpts();
    TVector<ElementProperties> outPrpts2 = func2->outputPrpts();

    // Set function 2 output to be different
    ElementPropertiesFactory* elemFact = ElementPropertiesFactory::instance();
    outPrpts2[0] = elemFact->createElementProperties(2, "Output 3", "Out Description");
    func2->defineOutputPrpts(outPrpts2);

    Problem prob;
    prob.appendFunction(func1);
    prob.appendFunction(func2);


    ProblemDefinitionStatus stat = prob.processProblemDefinition();
    for(int i=0; i<2; i++)
    {
        QCOMPARE((int)prob.f2pMap()[i].size(), 0);
        QCOMPARE((int)prob.f2dMap()[i].size(), 3);
        QCOMPARE((int)prob.f2cMap()[i].size(), 0);
        QCOMPARE((int)prob.f2oMap()[i].size(), 3);
        QCOMPARE((int)prob.f2uMap()[i].size(), 0);
        QCOMPARE(stat, FullyDefined);
    }

    // Append functions with defined parameter indexes
    TVector<int> prmtrsIdx(2);
    prmtrsIdx[0] = 1; //DVar 1 to parameter
    prmtrsIdx[1] = 0; //DVar 0 to parameter

    prob = Problem();
    prob.appendFunction(func1, prmtrsIdx);
    prob.appendFunction(func2, prmtrsIdx);

    stat = prob.processProblemDefinition();
    QCOMPARE((int)prob.f2pMap()[0].size(), 2);
    QCOMPARE((int)prob.f2dMap()[0].size(), 1);
    QCOMPARE((int)prob.f2cMap()[0].size(), 0);
    QCOMPARE((int)prob.f2oMap()[0].size(), 3);
    QCOMPARE((int)prob.f2uMap()[0].size(), 0);
    QCOMPARE(prob.f2pMap()[0][0], 0);
    QCOMPARE(prob.f2pMap()[0][1], 1);
    QCOMPARE(prob.f2dMap()[0][0], 2);

    QCOMPARE((int)prob.f2pMap()[1].size(), 2);
    QCOMPARE((int)prob.f2dMap()[1].size(), 1);
    QCOMPARE((int)prob.f2cMap()[1].size(), 0);
    QCOMPARE((int)prob.f2oMap()[1].size(), 3);
    QCOMPARE((int)prob.f2uMap()[1].size(), 0);
    QCOMPARE(prob.f2pMap()[1][0], 0);
    QCOMPARE(prob.f2pMap()[1][1], 1);
    QCOMPARE(prob.f2dMap()[1][0], -1);
    QCOMPARE(stat, FullyDefined);

    // Append functions with defined constraint indexes
    prmtrsIdx = TVector<int>(0);
    TVector<int> cnstrnIdx1(1);
    cnstrnIdx1[0] = 1;
    TVector<int> cnstrnIdx2(0);
    prob = Problem();
    prob.appendFunction(func1, prmtrsIdx, cnstrnIdx1);
    prob.appendFunction(func2, prmtrsIdx, cnstrnIdx2);

    stat = prob.processProblemDefinition();
    // The specified output is both a constraint and an objective
    QCOMPARE((int)prob.f2pMap()[0].size(), 0);
    QCOMPARE((int)prob.f2dMap()[0].size(), 3);
    QCOMPARE((int)prob.f2cMap()[0].size(), 1);
    QCOMPARE((int)prob.f2oMap()[0].size(), 3);
    QCOMPARE((int)prob.f2uMap()[0].size(), 0);
    QCOMPARE(prob.f2oMap()[0][0], 0);
    QCOMPARE(prob.f2oMap()[0][1], 1);
    QCOMPARE(prob.f2oMap()[0][2], -1);
    QCOMPARE(prob.f2cMap()[0][0], 1);

    QCOMPARE((int)prob.f2pMap()[1].size(), 0);
    QCOMPARE((int)prob.f2dMap()[1].size(), 3);
    QCOMPARE((int)prob.f2cMap()[1].size(), 1);
    QCOMPARE((int)prob.f2oMap()[1].size(), 3);
    QCOMPARE((int)prob.f2uMap()[1].size(), 0);
    QCOMPARE(prob.f2oMap()[1][0], -1);
    QCOMPARE(prob.f2oMap()[1][1], -1);
    QCOMPARE(prob.f2oMap()[1][2], 0);
    QCOMPARE(prob.f2cMap()[1][0], -1);
    QCOMPARE(stat, FullyDefined);

    // Append functions with defined unused indexes
    prmtrsIdx = TVector<int>(0);
    TVector<int> objectIdx(0);
    cnstrnIdx1 = TVector<int>(0);
    cnstrnIdx2 = TVector<int>(0);
    TVector<int> unusedIdx1(1);
    unusedIdx1[0] = 1;
    TVector<int> unusedIdx2(1);
    unusedIdx2[0] = 0;

    prob = Problem();
    prob.appendFunction(func1, prmtrsIdx, cnstrnIdx1, objectIdx, unusedIdx1);
    prob.appendFunction(func2, prmtrsIdx, cnstrnIdx2, objectIdx, unusedIdx2);

    stat = prob.processProblemDefinition();
    // The specified output is both a constraint and an objective
    QCOMPARE((int)prob.f2pMap()[0].size(), 0);
    QCOMPARE((int)prob.f2dMap()[0].size(), 3);
    QCOMPARE((int)prob.f2cMap()[0].size(), 0);
    QCOMPARE((int)prob.f2oMap()[0].size(), 1);
    QCOMPARE((int)prob.f2uMap()[0].size(), 2);
    QCOMPARE(prob.f2oMap()[0][0], 0);
    QCOMPARE(prob.f2uMap()[0][0], 1);
    QCOMPARE(prob.f2uMap()[0][1], -1);

    QCOMPARE((int)prob.f2pMap()[1].size(), 0);
    QCOMPARE((int)prob.f2dMap()[1].size(), 3);
    QCOMPARE((int)prob.f2cMap()[1].size(), 0);
    QCOMPARE((int)prob.f2oMap()[1].size(), 1);
    QCOMPARE((int)prob.f2uMap()[1].size(), 2);
    QCOMPARE(prob.f2oMap()[1][0], -1);
    QCOMPARE(prob.f2uMap()[1][0], -1);
    QCOMPARE(prob.f2uMap()[1][1], 0);
    QCOMPARE(stat, FullyDefined);
}

/*
 * This test checks if an uncertain decision variable is sampled within
 * the bounds. It is assumed that a variable sampled out of bounds is
 * truncated to the closest limit.
 * The objective function is f(x) = 1+(x-1)^2 where x \in [0,1]
 * Two normal distributions are defined:
 * 1) N(1.5,0.05)
 * 2) N(-0.5,0.05)
 * The first distribution generates samples that are higher than the upper limit of x
 * The second distribution generates samples that are lower than the lower limit of x
 */
void tst_baseoperations::test_UncertaintyMappingEvaluationOutOfBounds()
{
    /// f(x) = 1+(x-1)^2 where x \in [0,1]
    IFunctionSPtr func(new ExampleFunction);

    /// Test problem settings
    func->TP_defineNInputs(1);
    func->TP_defineNOutputs(1);

    /// Decision variable uncertainty (1)
    /// Normal distribution parameterized by N(1.5,0.05)
    UncertaintyMapping* umap1 = new UncertaintyMapping;
    TVector<double> constTerms1; constTerms1 << 1.5 << 0.05;
    TVector<double> linTerms1;   linTerms1   << 0.0 << 0.0;
    umap1->defineDistributionType(NormalDistType);
    umap1->defineConstTerms(constTerms1);
    umap1->defineLinearTerms(linTerms1);

    /// Decision variable uncertainty (2)
    /// Normal distribution parameterized by N(-0.5,0.05)
    UncertaintyMapping* umap2 = new UncertaintyMapping;
    TVector<double> constTerms2; constTerms2 << -0.5 << 0.05;
    TVector<double> linTerms2;   linTerms2   << 0.0 << 0.0;
    umap2->defineDistributionType(NormalDistType);
    umap2->defineConstTerms(constTerms2);
    umap2->defineLinearTerms(linTerms2);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(func);
    prob->processProblemDefinition();

    BoxConstraintsDataSPtr box = prob->boxConstraints();
    box->replaceLowerBound(0, IElement(RealType, 0.0));
    box->replaceUpperBound(0, IElement(RealType, 1.0));

    IElement dVal(RealType,0.5); // this is actually irrelevant

    /// check that x is set to the upper bound
    /// f(x=1)=1
    prob->redefineDVarUncertainty(0, umap1);
    IMapping imap1(prob);
    imap1.defineDecisionVar(0,dVal);
    imap1.evaluate();
    QCOMPARE(imap1.doubleObjectiveVar(0),1.0);


    /// check that x is set to the lower bound
    /// f(x=0)=2
    prob->redefineDVarUncertainty(0, umap2);
    IMapping imap2(prob);
    imap2.defineDecisionVar(0,dVal);
    imap2.evaluate();
    QCOMPARE(imap2.doubleObjectiveVar(0),2.0);
}

void tst_baseoperations::test_IMapping_constructorsDistructor()
{
    int nDVars = 10;
    int nOVars = 4;
    int nParams = 5;
    int nConsts = 2;

    TVector<int> params;
    for(int i=0; i<nParams; i++) {
        params << nDVars + i;
    }
    // first two objectives are also constraints
    TVector<int> constraints;
    for(int i=0; i<nConsts; i++) {
        constraints << i;
    }

    IFunctionSPtr func(new ExampleFunction);
    func->TP_defineNInputs(nDVars + nParams);
    func->TP_defineNOutputs(nOVars);
    ProblemSPtr prob(new Problem);
    prob->appendFunction(func, params, constraints);
    ProblemDefinitionStatus stat = prob->processProblemDefinition();
    QCOMPARE(stat, FullyDefined);

    // default constructor
    IMapping imap;
    QCOMPARE((int)imap.decisionVec().size(), 0);

    // constructor from problem
    IMapping imap1(prob);
    QCOMPARE((int)imap1.decisionVec().size(), nDVars);
    IMappingSPtr sptrImap(new IMapping(prob));
    QCOMPARE((int)sptrImap->decisionVec().size(), nDVars);
    IMapping* ptrImap = new IMapping(prob);
    QCOMPARE((int)ptrImap->decisionVec().size(), nDVars);

    // copy constructor
    IMapping imap2 = imap1;
    QCOMPARE((int)imap2.decisionVec().size(), nDVars);
    IMapping imap3(imap1);
    QCOMPARE((int)imap3.decisionVec().size(), nDVars);

    // operator() =
    IMapping imap4;
    imap4 = imap1;
    QCOMPARE((int)imap4.decisionVec().size(), nDVars);

}

void tst_baseoperations::test_TypeIMappingOperations()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    int dN = 5;
    int oN = 2;
    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);
    // <
    {
        cout << "Testing IMapping < IMapping Operation (true)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            m1->defineObjectiveVar(i, i+1);
            m2->defineObjectiveVar(i, i+101);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 < *m2;
        TCOMPARE(rs, true);

    }

    {
        cout << "Testing IMapping < IMapping Operation (false)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+101);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 < *m2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IMapping < IMapping Operation (incomparable <- equal)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal(i+1);
            m1->defineObjectiveVar(i, oVal);
            m2->defineObjectiveVar(i, oVal);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 < *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping < IMapping Operation (incomparable <- partial)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        m2->defineObjectiveVar(0, IElement(101));

        tribool rs = *m1 < *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping < IMapping Operation (incomparable <- different dimensions)..." << endl;

        ProblemSPtr difProb = createProblem(dN, oN+1, Tigon::IntegerType);

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(difProb));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        m1->defineEvaluated();
        m2->defineEvaluated();
        tribool rs = (*m2 < *m1);
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping < IMapping Operation (incomparable <- not evaluated)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2 = IMappingSPtr(new IMapping);
        m2=m1;

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << "," <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = (*m2 < *m1);
        TCOMPARE(rs, incomparable);

    }


    // >
    {
        cout << "Testing IMapping > IMapping Operation (true)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+101);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = (*m1 > *m2);
        TCOMPARE(rs, true);

    }

    {
        cout << "Testing IMapping > IMapping Operation (false)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+101);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 > *m2;
        TCOMPARE(rs, false);

    }

    {
        cout << "Testing IMapping > IMapping Operation (incomparable <- equal)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal(i+1);
            m1->defineObjectiveVar(i, oVal);
            m2->defineObjectiveVar(i, oVal);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 > *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping > IMapping Operation (incomparable <- partial)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+2);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        m2->defineObjectiveVar(0, IElement(101));

        tribool rs = *m1 > *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping > IMapping Operation (incomparable <- different dimensions)..." << endl;

        ProblemSPtr difProb = createProblem(dN, oN+1, Tigon::IntegerType);

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(difProb));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        m1->defineEvaluated();
        m2->defineEvaluated();
        tribool rs = *m2>*m1;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping > IMapping Operation (incomparable <- not evaluated)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2 = IMappingSPtr(new IMapping);
        m2=m1;

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << "," <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = *m2>*m1;
        TCOMPARE(rs, incomparable);

    }


    // <=
    {
        cout << "Testing IMapping <= IMapping Operation (true <- strictly)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+101);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 <= *m2;
        TCOMPARE(rs, true);

    }

    {
        cout << "Testing IMapping <= IMapping Operation (true <- partial equality/inequality)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        m2->defineObjectiveVar(0, IElement(101));

        tribool rs = *m1 <= *m2;
        TCOMPARE(rs, true);

    }

    {
        cout << "Testing IMapping <= IMapping Operation (false)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+101);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 <= *m2;
        TCOMPARE(rs, false);

    }

    {
        cout << "Testing IMapping <= IMapping Operation (incomparable <- equality)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 <= *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping <= IMapping Operation (incomparable <- partial)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+101);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        m2->defineObjectiveVar(0, IElement(0));

        tribool rs = *m1 <= *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping <= IMapping Operation (incomparable <- different dimensions)..." << endl;

        ProblemSPtr difProb = createProblem(dN, oN+1, Tigon::IntegerType);

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(difProb));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        m1->defineEvaluated();
        m2->defineEvaluated();
        tribool rs = *m2<=*m1;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping <= IMapping Operation (incomparable <- not evaluated)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2 = IMappingSPtr(new IMapping);
        m2=m1;

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << "," <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = *m2<=*m1;
        TCOMPARE(rs, incomparable);

    }

    // >=
    {
        cout << "Testing IMapping >= IMapping Operation (true <- strictly)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+101);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = (*m1 >= *m2);
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IMapping >= IMapping Operation (false)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+101);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1 >= *m2;
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IMapping >= IMapping Operation (incomparable <- equality)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m1>=*m2;
        TCOMPARE(rs, incomparable);
    }
    {
        cout << "Testing IMapping >= IMapping Operation (incomparable <- partial)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<oN; i++){
            IElement oVal1(i+2);
            IElement oVal2(i+1);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        m2->defineObjectiveVar(0, IElement(101));

        tribool rs = *m1 >= *m2;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping >= IMapping Operation (incomparable <- different dimensions)..." << endl;

        ProblemSPtr difProb = createProblem(dN, oN+1, Tigon::IntegerType);

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(difProb));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        m1->defineEvaluated();
        m2->defineEvaluated();
        tribool rs = *m2>=*m1;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping >= IMapping Operation (incomparable <- not evaluated)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2 = IMappingSPtr(new IMapping);
        m2=m1;

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << "," <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = *m2>=*m1;
        TCOMPARE(rs, incomparable);
    }


    // ==
    {
        cout << "Testing IMapping == IMapping Operation (true)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<dN; i++){
            IElement dVal(i);
            m1->defineDecisionVar(i, dVal);
        }

        for(int i=0; i<oN; i++){
            IElement oVal(i+1);
            m1->defineObjectiveVar(i, oVal);
        }

        m1->defineEvaluated();

        m2=m1;

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        cout << "Index 0 of dVec: " << *m1->decisionVar(0) <<"    " << *m2->decisionVar(0) << endl;
        cout << "Index 0 of oVec: " << *m1->objectiveVar(0) <<"    " << *m2->objectiveVar(0) << endl;
        tribool rs = *m2==*m1;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IMapping == IMapping Operation (incomparable <- different dimensions)..." << endl;

        ProblemSPtr difProb = createProblem(dN+1, oN+1, Tigon::IntegerType);

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(difProb));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        m1->defineEvaluated();
        m2->defineEvaluated();
        tribool rs = (*m2 == *m1);
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping == IMapping Operation "
                "(true <- not evaluated, equal input vec)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2 = IMappingSPtr(new IMapping);
        m2=m1;

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << ", " <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = (*m2 == *m1);
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IMapping == IMapping Operation "
                "(false <- not evaluated, different input vec)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2(new IMapping(prob));
        m2->defineDecisionVar(0, 3);
        m2->defineEvaluated(false);

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << ", " <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        cout << "Index 0 of dVec: " << *m1->decisionVar(0) <<"    " << *m2->decisionVar(0) << endl;

        tribool rs = (*m2 == *m1);
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IMapping == IMapping Operation "
                "(incomparable <- one evaluated, one not)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2(new IMapping(prob));
        m2->defineEvaluated(true);

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << ", " <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = (*m2 == *m1);
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping == IMapping Operation (false <- dVecs are different)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<dN; i++) {
            IElement dVal1(i);
            IElement dVal2(i+100);
            m1->defineDecisionVar(i, dVal1);
            m2->defineDecisionVar(i, dVal2);
        }

        for(int i=0; i<oN; i++) {
            IElement oVal(i+1);
            m1->defineObjectiveVar(i, oVal);
            m2->defineObjectiveVar(i, oVal);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        cout << "Index 0 of dVec: " << *m1->decisionVar(0) <<"    " << *m2->decisionVar(0) << endl;
        cout << "Index 0 of oVec: " << *m1->objectiveVar(0) <<"    " << *m2->objectiveVar(0) << endl;
        tribool rs = (*m2 == *m1);
        TCOMPARE(rs, false);
    }

    {
        cout << "Testing IMapping == IMapping Operation (false <- oVecs are different)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<dN; i++) {
            IElement dVal(i);
            m1->defineDecisionVar(i, dVal);
            m2->defineDecisionVar(i, dVal);
        }

        for(int i=0; i<oN; i++) {
            IElement oVal(i+1);
            IElement oVal2(i+100);
            m1->defineObjectiveVar(i, oVal);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        cout << "Index 0 of dVec: " << *m1->decisionVar(0) <<"    " << *m2->decisionVar(0) << endl;
        cout << "Index 0 of oVec: " << *m1->objectiveVar(0) <<"    " << *m2->objectiveVar(0) << endl;
        tribool rs = (*m2 == *m1);
        TCOMPARE(rs, false);
    }

    // !=
    {
        cout << "Testing IMapping != IMapping Operation (true <- all elements of dVecs and oVecs are different)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<dN; i++){
            IElement dVal1(i);
            IElement dVal2(i+100);
            m1->defineDecisionVar(i, dVal1);
            m2->defineDecisionVar(i, dVal2);
        }

        for(int i=0; i<oN; i++){
            IElement oVal1(i+1);
            IElement oVal2(i+101);
            m1->defineObjectiveVar(i, oVal1);
            m2->defineObjectiveVar(i, oVal2);
        }

        m1->defineEvaluated();
        m2->defineEvaluated();

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        cout << "Index 0 of dVec: " << *m1->decisionVar(0) <<"    " << *m2->decisionVar(0) << endl;
        cout << "Index 0 of oVec: " << *m1->objectiveVar(0) <<"    " << *m2->objectiveVar(0) << endl;
        tribool rs = (*m2 != *m1);
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IMapping != IMapping Operation (true <- only part of elements of dVecs and oVecs are different)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(prob));

        for(int i=0; i<dN; i++){
            IElement dVal(i);
            m1->defineDecisionVar(i, dVal);
            m2->defineDecisionVar(i, dVal);
        }

        for(int i=0; i<oN; i++){
            IElement oVal(i+1);
            m1->defineObjectiveVar(i, oVal);
            m2->defineObjectiveVar(i, oVal);
        }

        // change the value of m2
        m2->defineDecisionVar(0, IElement(100));
        m2->defineObjectiveVar(0, IElement(101));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        for (int i=0; i<dN; i++){
            cout << "dVec[" << i << "]: "<< *m1->decisionVar(i) <<" <-> " << *m2->decisionVar(i) << endl;
        }
        for (int i=0; i<oN; i++){
            cout << "oVec[" << i << "]: "<< *m1->objectiveVar(i) <<" <-> " << *m2->objectiveVar(i) << endl;
        }


        m1->defineEvaluated();
        m2->defineEvaluated();

        tribool rs = *m2!=*m1;
        TCOMPARE(rs, true);
    }

    {
        cout << "Testing IMapping != IMapping Operation (incomparable <- different dimensions)..." << endl;

        ProblemSPtr difProb = createProblem(dN+1, oN+1, Tigon::IntegerType);

        IMappingSPtr m1(new IMapping(prob));
        IMappingSPtr m2(new IMapping(difProb));

        cout << m1->decisionVec().size() <<"    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() <<"    " << m2->objectiveVec().size() << endl;
        m1->defineEvaluated();
        m2->defineEvaluated();
        tribool rs = *m2!=*m1;
        TCOMPARE(rs, incomparable);
    }

    {
        cout << "Testing IMapping != IMapping Operation (false <- not evaluated)..." << endl;

        IMappingSPtr m1(new IMapping(prob));
        m1->defineEvaluated(false);

        IMappingSPtr m2 = IMappingSPtr(new IMapping);
        m2=m1;

        cout << m1->decisionVec().size() << "    " << m2->decisionVec().size() << endl;
        cout << m1->objectiveVec().size() << "    " << m2->objectiveVec().size() << endl;
        cout << "m1 evaluated: " << m1->isEvaluated() << "," <<"m2 evaluated: " << m2->isEvaluated()<<endl;
        tribool rs = *m2!=*m1;
        TCOMPARE(rs, false);
    }

}

void tst_baseoperations::test_TypeIdentification()
{
    using namespace Tigon::Representation;
    cout.precision(15);

    TVector<Tigon::ElementType> typeVec;
    typeVec.push_back(Tigon::NominalType);
    typeVec.push_back(Tigon::RealType);
    typeVec.push_back(Tigon::IntegerType);

    TVector<IElementSPtr> tvec = createIElementSPtrVector(typeVec);

    int i=0;
    cout << "Element number: " << i << " is Nominal." << endl;
    QCOMPARE(Tigon::NominalType, tvec.at(i)->type());
    i++;
    cout << "Element number: " << i << " is Real." << endl;
    QCOMPARE(Tigon::RealType, tvec.at(i)->type());
    i++;
    cout << "Element number: " << i << " is Integer." << endl;
    QCOMPARE(Tigon::IntegerType, tvec.at(i)->type());
    i++;
}

void tst_baseoperations::test_AlgorithmCreation()
{
    using namespace Tigon::Representation;
    cout.precision(15);
    IElementSPtr element = IElementSPtr(new Tigon::Representation::IElement());
    element->defineType(Tigon::RealType);
    double xr = 12.523123017876;
    IElement x(xr);
    element->defineValue(x);
    double y = element->value<double>();
    cout << "Y is: " << y << endl;

    TVector<Tigon::ElementType> typeVec;
    typeVec.push_back(Tigon::NominalType);
    typeVec.push_back(Tigon::RealType);
    typeVec.push_back(Tigon::IntegerType);

    TVector<IElementSPtr> tvec = createIElementSPtrVector(typeVec);

    for(int i=0; i<typeVec.size(); i++) {
        switch((tvec.at(i))->type()) {
        case Tigon::RealType:
            cout << "Element number: " << i << " is Real." << endl;
            break;

        case Tigon::NominalType:
            cout << "Element number: " << i << " is Nominal." << endl;
            break;

        case Tigon::IntegerType:
            cout << "Element number: " << i << " is Integer." << endl;
            break;

        default:
            cout << "Unknown element type." << endl;
            break;

        }
    }
}

void tst_baseoperations::test_ISet_constructorDestructor()
{
    ISet* aSet = new ISet();
    int   rsNTags = aSet->tags().size();
    int   exNTags = 0;
    int     rsSize = aSet->size();
    int     exSize = 0;
    QCOMPARE(rsNTags, exNTags);
    QCOMPARE(rsSize, exSize);
    delete aSet;
}

void tst_baseoperations::test_ISet_constructorDestructorFromSet()
{
    PSetBase* base  = initIPSetTest();
    ISet*     oSet   = base->set(0);
    ISet*     aSet   = new ISet(oSet);
    int       rsSize = aSet->size();
    int       exSize = oSet->size();
    QCOMPARE(rsSize, exSize);

    delete aSet;
    delete base;
}

void tst_baseoperations::test_ISet_constructorDestructorwithTags()
{
    TStringList tags;
    tags << "firstTag" << "secondTag";
    ISet*       aSet   = new ISet(tags);
    TStringList rsTags = aSet->tags();
    QCOMPARE(rsTags.size(), tags.size());
    for(int i=0; i<tags.size(); i++) {
        QCOMPARE(rsTags[i], tags[i]);
    }

    delete aSet;
}
void tst_baseoperations::test_ISet_all()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing all()" << endl;
    TVector<IMappingSPtr> theIMappings = aSet->all();
    int rs = theIMappings.size();
    int ex = 10;
    QCOMPARE(rs, ex);

    delete base;
}

void tst_baseoperations::test_ISet_IMappings()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing range(int from)" << endl;
    TVector<IMappingSPtr> theIMappings = aSet->range(0);
    int rs = theIMappings.size();
    int ex = 10;
    QCOMPARE(rs, ex);

    theIMappings = aSet->range(7);
    rs = theIMappings.size();
    ex = 3;
    QCOMPARE(rs, ex);

    cout << "Testing range(int from, int length)" << endl;
    theIMappings = aSet->range(7, 2);
    rs = theIMappings.size();
    ex = 2;
    QCOMPARE(rs, ex);

    theIMappings = aSet->range(7, 5);
    rs = theIMappings.size();
    ex = 3;
    QCOMPARE(rs, ex);

    theIMappings = aSet->range(0, aSet->size());
    rs = theIMappings.size();
    ex = aSet->size();
    QCOMPARE(rs, ex);

    delete base;
}

void tst_baseoperations::test_ISet_IMap()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing at(int idx) const" << endl;
    IMappingSPtr anIMapping = aSet->at(2);
    int rs = anIMapping->decisionVar(0)->value<int>();
    int ex = 2;
    QCOMPARE(rs, ex);

    delete base;
}

void tst_baseoperations::test_ISet_define_iMap()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing define(int idx, IMappingSPtr iMap)" << endl;
    int rsOld = aSet->at(4)->decisionVar(0)->value<int>();
    int exOld = 4;
    IMappingSPtr IMapTwo = aSet->at(2);
    aSet->define(4, IMapTwo);
    int rsNew = aSet->at(4)->decisionVar(0)->value<int>();
    int exNew = 2;
    QCOMPARE(rsOld, exOld);
    QCOMPARE(rsNew, exNew);

    delete base;
}

void tst_baseoperations::test_ISet_define_set()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing define(TVector<IMappingSPtr> sVec)" << endl;
    ISet aNewSet;
    aNewSet.define(aSet->all());
    int rs = aNewSet.at(3)->decisionVar(0)->value<int>();
    int ex = 3;
    QCOMPARE(rs, ex);

    delete base;
}

void tst_baseoperations::test_ISet_removeIdx()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing remove(int idx)" << endl;
    int rsSizeOld = aSet->size();
    int exSizeOld = 10;
    int rsValueOld = aSet->at(6)->decisionVar(0)->value<int>();
    int exValueOld = 6;
    bool idxSixInBounds = aSet->remove(6);
    cout << "Index 6 is " << (idxSixInBounds? "" : "not ") << "within bounds" << endl;
    int rsSizeNew = aSet->size();
    int exSizeNew = 9;
    int rsValueNew = aSet->at(6)->decisionVar(0)->value<int>();
    int exValueNew = 7;
    bool rsInBounds = aSet->remove(10);
    bool exInBounds = false;
    QCOMPARE(rsSizeOld, exSizeOld);
    QCOMPARE(rsValueOld, exValueOld);
    QCOMPARE(rsSizeNew, exSizeNew);
    QCOMPARE(rsValueNew, exValueNew);
    QCOMPARE(rsInBounds, exInBounds);

    delete base;
}

void tst_baseoperations::test_ISet_removePtr()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing remove(IMappingSPtr iMap)" << endl;
    IMappingSPtr IMapSix = aSet->at(6);
    int rsOld = aSet->size();
    int exOld = 10;
    bool rsFoundFirstTime = aSet->remove(IMapSix);
    int rsNew = aSet->size();
    int exNew = 9;
    bool rsFoundSecondTime = aSet->remove(IMapSix);
    QCOMPARE(rsOld, exOld);
    QCOMPARE(rsNew, exNew);
    QCOMPARE(rsFoundFirstTime, true);
    QCOMPARE(rsFoundSecondTime, false);

    delete base;
}

void tst_baseoperations::test_ISet_replace()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing replace(const TVector<IMappingSPtr> &sVec)" << endl;
    IMappingSPtr IMapFive = aSet->at(5);
    IMappingSPtr IMapSix = aSet->at(6);
    int rsOld = aSet->at(6)->decisionVar(0)->value<int>();
    int exOld = 6;
    bool rsFoundFirstTime = aSet->replace(IMapSix, IMapFive);
    bool rsFoundSecondTime = aSet->replace(IMapSix, IMapFive);
    int rsNew = aSet->at(6)->decisionVar(0)->value<int>();
    int exNew = 5;
    QCOMPARE(rsOld, exOld);
    QCOMPARE(rsNew, exNew);
    QCOMPARE(rsFoundFirstTime, true);
    QCOMPARE(rsFoundSecondTime, false);

    delete base;
}

void tst_baseoperations::test_ISet_shuffle()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    int exSize = 10;

    cout << "Testing shuffle()" << endl;
    QCOMPARE((int)aSet->size(), exSize);
    for(int i = 0; i < exSize; i++) {
        QCOMPARE(aSet->at(i)->decisionVar(0)->value<int>(), i);
    }

    aSet->shuffle();

    QCOMPARE((int)aSet->size(), exSize);

    // test that all IMappings are still there but in a different order
    TVector<bool> exist(exSize, false);
    TVector<int> newOrder;
    int nLarger = 0;
    int nSmaller = 0;
    for(int i = 0; i < exSize; i++) {
        exist[aSet->at(i)->decisionVar(0)->value<int>()] = true;
        newOrder.push_back(aSet->at(i)->decisionVar(0)->value<int>());
        if(i>0) {
            if(aSet->at(i)->decisionVar(0)->value<int>() > aSet->at(i-1)->decisionVar(0)->value<int>()) {
                nLarger++;
            } else {
                nSmaller++;
            }
        }
    }
    for(int i = 0; i < exSize; i++) {
        QCOMPARE(exist[i] == true, true);
    }
    bool rs = (nLarger >= 2) && (nSmaller >= 2);

    QCOMPARE(rs, true);

    delete base;
}

void tst_baseoperations::test_ISet_clear()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    cout << "Testing clear()" << endl;
    int rsOld = aSet->size();
    int exOld = 10;
    aSet->clearMappings();
    int rsNew = aSet->size();
    int exNew = 0;
    QCOMPARE(rsOld, exOld);
    QCOMPARE(rsNew, exNew);

    delete base;
}

void tst_baseoperations::test_ISet_addRemoveTags()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    TStringList tags;
    tags << "" << "tag1" << "tag2" << "tag3" << "tag4";
    for(int i=0; i<tags.size(); i++) {
        aSet->addTag(tags[i]);
    }

    TStringList rsTags = aSet->tags();
    QCOMPARE((int)rsTags.size(), (int)tags.size());

    QCOMPARE(contains(rsTags, "tag2"), true);
    aSet->removeTag("tag2");
    rsTags = aSet->tags();
    QCOMPARE(contains(rsTags, "tag2"), false);
    QCOMPARE((int)rsTags.size(), int(tags.size() - 1));

    // tag doesn't exist
    aSet->removeTag("tag5");
    rsTags = aSet->tags();
    QCOMPARE(contains(rsTags, "tag5"), false);
    QCOMPARE((int)rsTags.size(), int(tags.size() - 1));

    delete base;
}

void tst_baseoperations::test_ISet_defineClearTags()
{
    PSetBase* base  = initIPSetTest();
    ISet*     aSet = base->set(0);

    TStringList tags;
    tags << "tag1" << "tag2" << "tag3" << "tag4";

    aSet->defineTags(tags);
    TStringList rsTags = aSet->tags();
    for(int i=0; i<tags.size(); i++) {
        QCOMPARE(rsTags[i], tags[i]);
    }

    aSet->clearTags();
    rsTags = aSet->tags();
    QCOMPARE((int)rsTags.size(), 0);

    delete base;
}

void tst_baseoperations::test_IPSetSets_allTags()
{
    PSetBase* base  = initIPSetTest();
    TStringList rsTags = base->allTags();
    TStringList exTags;
    exTags << "" << "tag1" << "tag2" << "tag3" << "tag4";
    QCOMPARE((int)rsTags.size(), (int)exTags.size());
    for(int i=0; i<rsTags.size(); i++) {
        QCOMPARE(rsTags[i], exTags[i]);
    }
    delete base;
}

void tst_baseoperations::test_IPSetSets_tagExists()
{
    PSetBase* base  = initIPSetTest();
    QCOMPARE(base->tagExists("tag2"), true);
    QCOMPARE(base->tagExists("does_not_exist"), false);

    delete base;
}

void tst_baseoperations::test_IPSetSets_setsWithTag()
{
    PSetBase* base  = initIPSetTest();
    TVector<ISet*> rsSets;

    rsSets = base->setsWithTag("tag1");
    QCOMPARE((int)rsSets.size(), 1);
    QCOMPARE(rsSets[0]->size(), 1);

    rsSets = base->setsWithTag("tag3");
    QCOMPARE((int)rsSets.size(), 2);
    QCOMPARE(rsSets[0]->size(), 2);
    QCOMPARE(rsSets[1]->size(), 3);

    rsSets = base->setsWithTag("does_not_exist");
    QCOMPARE((int)rsSets.size(), 0);

    delete base;
}

void tst_baseoperations::test_IPSetSets_setsWithTags()
{
    PSetBase* base  = initIPSetTest();
    TStringList tags;
    TVector<ISet*> rsSets;

    tags << "tag3";
    rsSets = base->setsWithTags(tags);
    QCOMPARE((int)rsSets.size(), 2);
    QCOMPARE(rsSets[0]->size(), 2);
    QCOMPARE(rsSets[1]->size(), 3);

    // order shouldn't matter
    tags << "tag2";
    rsSets = base->setsWithTags(tags);
    QCOMPARE((int)rsSets.size(), 1);
    QCOMPARE(rsSets[0]->size(), 2);

    tags[1] = "tag4";
    rsSets = base->setsWithTags(tags);
    QCOMPARE((int)rsSets.size(), 1);
    QCOMPARE(rsSets[0]->size(), 3);

    tags << "tag2";
    rsSets = base->setsWithTags(tags);
    QCOMPARE((int)rsSets.size(), 0);

    delete base;
}

void tst_baseoperations::test_IPSetSets_setWithTag()
{
    PSetBase* base  = initIPSetTest();
    ISet* rsSet;

    rsSet = base->setWithTag("tag3", 1);
    QCOMPARE(rsSet==nullptr, false);
    QCOMPARE(rsSet->size(), 3);

    // default is idx=0
    rsSet = base->setWithTag("tag3");
    QCOMPARE(rsSet==nullptr, false);
    QCOMPARE(rsSet->size(), 2);

    rsSet = base->setWithTag("tag3", 2);
    QCOMPARE(rsSet==nullptr, true);

    rsSet = base->setWithTag("does_not_exist");
    QCOMPARE(rsSet==nullptr, true);

    delete base;
}

void tst_baseoperations::test_IPSetSets_tagSet()
{
    PSetBase* base  = initIPSetTest();
    ISet* rsSet;
    TStringList rsTags;

    TString tag1("tag1");
    rsSet = base->setWithTag(tag1);
    QCOMPARE(rsSet==nullptr, false);
    QCOMPARE(rsSet->size(), 1);
    QCOMPARE((int)rsSet->tags().size(), 1);

    TString newTag("newTag");
    base->tagSet(rsSet, newTag);
    rsTags = rsSet->tags();
    QCOMPARE((int)rsTags.size(), 2);
    QCOMPARE(rsTags[0], tag1);
    QCOMPARE(rsTags[1], newTag);

    delete base;
}

void tst_baseoperations::test_IPSetSets_removeTagFromSet()
{
    PSetBase* base = initIPSetTest();
    ISet* rsSet = base->setWithTag("tag1");
    QCOMPARE((int)rsSet->tags().size(), 1);

    int oldSize = base->allTags().size();

    base->removeTagFromSet(rsSet, "tag1");

    QCOMPARE((int)rsSet->tags().size(), 0);

    int newSize = base->allTags().size();
    QCOMPARE(newSize, oldSize-1);

    delete base;
}

void tst_baseoperations::test_IPSetSets_allSets()
{
    PSetBase* base  = initIPSetTest();
    TVector<ISet*> rsSets = base->allSets();
    QCOMPARE((int)rsSets.size(), 5);

    delete base;
}

void tst_baseoperations::test_IPSetSets_numberOfSets()
{
    PSetBase* base  = initIPSetTest();
    int rsSize = base->numberOfSets();
    QCOMPARE(rsSize, 5);

    delete base;
}

void tst_baseoperations::test_IPSetSets_clearSets()
{
    PSetBase* base  = initIPSetTest();
    base->clearSets();
    QCOMPARE(base->numberOfSets(), 0);
    QCOMPARE((int)base->allTags().size(), 0);

    delete base;
}

void tst_baseoperations::test_IPSetSets_indexOfSet()
{
    PSetBase* base  = initIPSetTest();
    TStringList tags;
    tags << "tag2" << "tag3";
    ISet* aSet = base->setWithTags(tags);
    int rsIdx = base->indexOfSet(aSet);
    QCOMPARE(rsIdx, 2);

    ISet* newSet = new ISet;
    rsIdx = base->indexOfSet(newSet);
    QCOMPARE(rsIdx, -1);

    delete newSet;
    delete base;
}

void tst_baseoperations::test_IPSetSets_appendSet()
{
    // see initIPSetTest()
    PSetBase* base  = initIPSetTest();
    delete base;
}

void tst_baseoperations::test_IPSetSets_removeSet()
{
    PSetBase* base = initIPSetTest();
    ISet* rsSet = base->setWithTag("tag1");
    bool rsExisted = base->removeSet(rsSet);
    QCOMPARE(rsExisted, true);

    rsExisted = base->removeSet(rsSet);
    QCOMPARE(rsExisted, false);

    // make sure the tag is also removed
    rsExisted = base->tagExists("tag1");
    QCOMPARE(rsExisted, false);

    // if the tag is used for other sets, it is not removed
    rsSet = base->setWithTag("tag3", 0);
    base->removeSet(rsSet);
    rsExisted = base->tagExists("tag3");
    QCOMPARE(rsExisted, true);

    delete base;
}

void tst_baseoperations::test_IPSetSets_appendToSet()
{
    PSetBase* base  = initIPSetTest();
    IMappingSPtr imap = base->set(0)->at(0);
    ISet* existingSet = base->set(1);
    ISet* newSet = new ISet("newTag");

    bool rsExisted = base->appendToSet(existingSet, imap);
    QCOMPARE(existingSet->size(), 2); //was originally 1
    QCOMPARE(rsExisted, true);

    // newSet was not a part of IPSet. It is now appended with its tag.
    rsExisted = base->appendToSet(newSet, imap);
    QCOMPARE(rsExisted, false);
    QCOMPARE(base->tagExists("newTag"), true);
    QCOMPARE(base->indexOfSet(newSet), 5);

    delete base;
}
#endif

void tst_baseoperations::test_IPSet_multipack()
{
    TString test("TEST");
    double val(1.0);
    PSetBase* base = new PSetBase();
    bool rs = base->multiPackExists(test);
    QCOMPARE(rs, false);

    base->insertMultiPack(test, TVector<double>({val}));
    rs = base->multiPackExists(test);
    QCOMPARE(rs, true);

    double rsVal = base->multiPack(test)[0];
    QCOMPARE(rsVal, val);

    delete base;
}


void tst_baseoperations::test_IPSet_multipack_customType()
{
    TVector<double> vec({0.1, 0.22});
    TString VectorKey("Vector");
    PSetBase* base = new PSetBase();

    base->insertMultiPack(VectorKey, TVector<double>({vec}));
    bool rs = base->multiPackExists(VectorKey);
    QCOMPARE(rs, true);

    TVector<double> rsVec = base->multiPack(VectorKey);

    QCOMPARE(rsVec.size(), vec.size());
    QCOMPARE(rsVec[0], vec[0]);

    delete base;
}

#ifdef TEST_ALL_BASE_OPERATIONS

void tst_baseoperations::test_nonDominatedArchive()
{
    int sSize = 15;
    TVector<IMappingSPtr> sSet;

    int dN = 1;
    int oN = 2;
    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);

    for(int i=0; i<sSize; i++) {
        sSet.push_back(IMappingSPtr(new IMapping(prob)));
    }

    int sol=0;
    for(int i=1; i<=5; i++) {
        for(int j=1; j<=6-i; j++) {
            IElement dVal(sol);
            IElement oVal1(i);
            IElement oVal2(j);
            sSet.at(sol)->defineDecisionVar(0, dVal);
            sSet.at(sol)->defineObjectiveVar(0, oVal1);
            sSet.at(sol)->defineObjectiveVar(1, oVal2);
            sSet.at(sol++)->defineEvaluated();
        }
    }

    PSetBase* base = new PSetBase();

    ISet* archive = base->setWithTag(Tigon::TNonDominatedArchive);
    bool rs = archive==nullptr;
    QCOMPARE(rs, true);

    Tigon::ArchiveUpdateStatus updated = base->updateNonDominatedArchive(sSet[14]); // (5, 1)
    archive = base->setWithTag(Tigon::TNonDominatedArchive);
    rs = archive==nullptr;
    QCOMPARE(rs, false);
    QCOMPARE(updated, Tigon::ReplacedInArchive);
    QCOMPARE(archive->size(), 1);

    updated = base->updateNonDominatedArchive(sSet[13]); // (4, 2)
    QCOMPARE(updated, Tigon::AddedToArchive);
    QCOMPARE(archive->size(), 2);

    updated = base->updateNonDominatedArchive(sSet[8]); // (2, 4)
    QCOMPARE(updated, Tigon::AddedToArchive);
    QCOMPARE(archive->size(), 3);

    updated = base->updateNonDominatedArchive(sSet[9]); // (3, 1)
    QCOMPARE(updated, Tigon::ReplacedInArchive);
    QCOMPARE(archive->size(), 2); // (2, 4), (3, 1)

    updated = base->updateNonDominatedArchive(sSet[9]); // (3, 1)
    QCOMPARE(updated, Tigon::UnchangedArchive);
    QCOMPARE(archive->size(), 2); // (2, 4), (3, 1)

    updated = base->updateNonDominatedArchive(sSet[12]); // (4, 1)
    QCOMPARE(updated, Tigon::UnchangedArchive);
    QCOMPARE(archive->size(), 2); // (2, 4), (3, 1)

    updated = base->updateNonDominatedArchive(sSet[2]); // (1, 3)
    QCOMPARE(updated, Tigon::ReplacedInArchive);
    QCOMPARE(archive->size(), 2); // (1, 3), (3, 1)
}

void tst_baseoperations::test_nadir()
{
    int sSize = 15;

    int dN = 1;
    int oN = 2;

    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);
    PSetBase*     base = new PSetBase();
    IFormulation* form = new IFormulation(base);
    form->defineProblem(prob);

    ISet* sSet = base->appendSet(TString(""));

    for(int i=0; i<sSize; i++) {
        base->createOptimizationMapping(sSet);
    }

    int sol=0;
    for(int i=1; i<=5; i++) {
        for(int j=1; j<=6-i; j++) {
            IElement dVal(sol);
            IElement oVal1(i);
            IElement oVal2(j);
            sSet->at(sol)->defineDecisionVar(0, dVal);
            sSet->at(sol)->defineObjectiveVar(0, oVal1);
            sSet->at(sol)->defineObjectiveVar(1, oVal2);
            sSet->at(sol++)->defineEvaluated();
        }
    }

    // Archive does not exist initially
    ISet* archive = base->setWithTag(Tigon::TNonDominatedArchive);
    bool rs = archive==nullptr;
    QCOMPARE(rs, true);

    // The Nadir point can be defined if the real (or approximated) optimal
    // front is known.
    base->defineNadirVec(sSet->at(6)->objectiveVec()); //(2, 2)
    TVector<double> rsNadir = IElementVecToRealVec(base->nadirVec());
    TVector<double> exNadir;
    exNadir << 2.0 << 2.0;
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    // By default the archive is not maintained, and the Nadir point is not
    // updated by new IMappings
    bool updated = base->updateNadirVec(sSet->at(14)); // (5, 1)
    QCOMPARE(updated, false);

    rsNadir = IElementVecToRealVec(base->nadirVec());
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    // To allow for nadir points updates, this needs to be allowed:
    base->defineKeepArchive(true);

    // Now it should be updated
    updated = base->updateNadirVec(sSet->at(14)); // (5, 1)
    QCOMPARE(updated, true); //PF = (5, 1); nadir = (5, 1)
    rsNadir = IElementVecToRealVec(base->nadirVec());
    exNadir[0] = 5.0;
    exNadir[1] = 1.0;
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    updated = base->updateNadirVec(sSet->at(13)); // (4, 2)
    QCOMPARE(updated, true); //PF = (5, 1), (4, 2); nadir = (5, 2)
    rsNadir = IElementVecToRealVec(base->nadirVec());
    exNadir[1] = 2.0;
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    updated = base->updateNadirVec(sSet->at(8)); // (2, 4)
    QCOMPARE(updated, true); //PF = (5, 1), (4, 2), (2, 4); nadir = (5, 4)
    rsNadir = IElementVecToRealVec(base->nadirVec());
    exNadir[1] = 4.0;
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    updated = base->updateNadirVec(sSet->at(10)); // (3, 3)
    QCOMPARE(updated, false); //PF = (5, 1), (4, 2), (2, 4), (3, 3); nadir = (5, 4)
    rsNadir = IElementVecToRealVec(base->nadirVec());
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    updated = base->updateNadirVec(sSet->at(9)); // (3, 1)
    QCOMPARE(updated, true); //PF = (3, 1), (2, 4); nadir = (3, 4)
    rsNadir = IElementVecToRealVec(base->nadirVec());
    exNadir[0] = 3.0;
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsNadir[i], exNadir[i]);
    }

    delete form;
    delete base;
}

void tst_baseoperations::test_IPSet_IdealAndAntiIdeal()
{
    PSetBase* base = initIPSetTest();

    TVector<IElementSPtr> rsIdeal = base->idealVec();
    TVector<IElementSPtr> rsAntiIdeal = base->antiIdealVec();
    int rs = rsIdeal.size();
    QCOMPARE(rs, base->objectiveVecSize());
    rs = rsAntiIdeal.size();
    QCOMPARE(rs, base->objectiveVecSize());

    // all integer values bounded between f1, f2 <= 6 , f1+f2 >= 6
    ISet* aSet = base->appendSet(TString(""));
    int sol = 0;
    for(int i=1; i<=6; i++) {
        for(int j = std::max(6-i, 1); j<=6; j++) {
            IMappingSPtr imap = base->createOptimizationMapping(aSet);
            IElement dVal(sol++);
            IElement oVal1(i);
            IElement oVal2(j);
            imap->defineDecisionVar(0, dVal);
            imap->defineObjectiveVar(0, oVal1);
            imap->defineObjectiveVar(1, oVal2);
            base->updateIdealNadirVec(imap);
        }
    }

    rsIdeal = base->idealVec();
    rsAntiIdeal = base->antiIdealVec();
    rs = rsIdeal.size();
    QCOMPARE(rs, base->objectiveVecSize());
    QCOMPARE(rsAntiIdeal[0]->value<int>(), 6);
    QCOMPARE(rsAntiIdeal[1]->value<int>(), 6);
    QCOMPARE(rsIdeal[0]->value<int>(), 1);
    QCOMPARE(rsIdeal[1]->value<int>(), 1);

    // Update the existing boundaries with a new evaluated IMapping
    IMappingSPtr imap = base->createOptimizationMapping(aSet);
    IElement dVal(15);
    IElement oVal1(7);
    IElement oVal2(-2);
    imap->defineDecisionVar(0, dVal);
    imap->defineObjectiveVar(0, oVal1);
    imap->defineObjectiveVar(1, oVal2);

    base->updateIdealNadirVec(imap);

    rsIdeal = base->idealVec();
    QCOMPARE(rsIdeal[1]->value<int>(), -2);
    QCOMPARE(rsIdeal[0]->value<int>(), 1); //shouldn't change

    rsAntiIdeal = base->antiIdealVec();
    QCOMPARE(rsAntiIdeal[0]->value<int>(), 7);
    QCOMPARE(rsAntiIdeal[1]->value<int>(), 6); //shouldn't change
}

void tst_baseoperations::test_IPSet_direction()
{
    int oN = 3;

    // default direction vector is 1 (for one objective)
    PSetBase* base = new PSetBase();
    TVector<double> rsDir = base->dirVec();
    int rs = rsDir.size();
    QCOMPARE(rs, 0);

    ProblemSPtr prob = createProblem(1, oN, IntegerType);
    IFormulation* form = new IFormulation(base);
    form->defineProblem(prob);
    delete form;

    // default direction vector is equal components
    rsDir = base->dirVec();
    rs = rsDir.size();
    QCOMPARE(rs, oN);
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsDir[i], 1.0/oN);
    }

    // vector wasn't updated because did not have the same size
    TVector<double> newDirVec(oN + 1, 1.0);
    base->defineDirVec(newDirVec);
    rsDir = base->dirVec();
    rs = rsDir.size();
    QCOMPARE(rs, oN);

    // vector wasn't updated because has a negative component
    fill(newDirVec, 1.0, oN);
    newDirVec[oN-1] = -1.0;
    base->defineDirVec(newDirVec);
    rsDir = base->dirVec();
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsDir[i], 1.0/oN);
    }

    newDirVec.clear();
    newDirVec << 1.0 << 2.0 << 3.0;
    base->defineDirVec(newDirVec);
    rsDir = base->dirVec();
    for(int i=0; i<oN; i++) {
        QCOMPARE(rsDir[i], newDirVec[i]/6.0);
    }
}

void tst_baseoperations::test_IFunction_ctor()
{
    IFunctionSPtr func = IFunctionSPtr(new ExampleFunction);

    func.reset();
}

void tst_baseoperations::test_IFunction_defineNVariables()
{
    IFunctionSPtr func = IFunctionSPtr(new ExampleFunction);

    int nIn  = 10;
    int nOut = 3;
    func->TP_defineNInputs(nIn);
    func->TP_defineNOutputs(nOut);

    TVector<ElementProperties> inProp  = func->inputPrpts();
    TVector<ElementProperties> outProp = func->outputPrpts();

    int rs_nIn  = func->TP_nInputs();
    int rs_nOut = func->TP_nOutputs();

    QCOMPARE(rs_nIn, nIn);
    QCOMPARE(rs_nOut, nOut);

    QCOMPARE((int)inProp.size(), nIn);
    QCOMPARE((int)outProp.size(), nOut);

    func.reset();
}

// TestIOperator is an empty subclass of IOperator that can be instantiated
class TestIOperator : public Tigon::Operators::IOperator
{
public:
    TestIOperator() {}
    TestIOperator(Tigon::Representation::IPSet* ipset)
        : IOperator(ipset) {}
    virtual ~TestIOperator() {}
    void    evaluateNode(){}
    TString     name(){return TString("name");}
    TString     description(){return TString("description");}
};

void tst_baseoperations::test_IOperators_addOutputTag()
{
    PSetBase* base    = initIPSetTest();
    TestIOperator* op = new TestIOperator(base);

    TString exTag = "test_operator";
    op->addOutputTag(exTag);

    TStringList tags = op->outputTags();
    int rsNum = tags.size();
    int exNum = 1;
    TString rsTag = tags.at(0);
    QCOMPARE(rsNum, exNum);
    QCOMPARE(rsTag, exTag);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_TP_defineOutputTags()
{
    PSetBase* base  = new PSetBase();
    TestIOperator* op = new TestIOperator(base);

    TStringList opTags;
    opTags << "test_operator" << "another_tag";
    op->defineOutputTags(opTags);

    TStringList tags = op->outputTags();
    int rsNum = tags.size();
    int exNum = 2;
    TString rsTag = tags.at(1);
    TString exTag = "another_tag";
    QCOMPARE(rsNum, exNum);
    QCOMPARE(rsTag, exTag);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_removeOutputTag()
{
    PSetBase* base    = initIPSetTest();
    TestIOperator* op = new TestIOperator(base);

    op->addOutputTag("test_operator");

    op->removeOutputTag("not_tagged_with_this_one");
    TStringList tags = op->outputTags();
    bool rs = tags.empty();
    QCOMPARE(rs, false);

    op->removeOutputTag("test_operator");
    tags = op->outputTags();
    rs = tags.empty();
    QCOMPARE(rs, true);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_addInputTag()
{
    PSetBase* base    = initIPSetTest();
    TestIOperator* op = new TestIOperator(base);

    TString exTag = "test_operator";
    op->addInputTag(exTag);

    TStringList tags = op->inputTags();
    int rsNum = tags.size();
    int exNum = 1;
    TString rsTag = tags.at(0);
    QCOMPARE(rsNum, exNum);
    QCOMPARE(rsTag, exTag);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_TP_defineInputTags()
{
    PSetBase* base  = new PSetBase();
    TestIOperator* op = new TestIOperator(base);

    TStringList opTags;
    opTags << "test_operator" << "another_tag";
    op->defineInputTags(opTags);

    TStringList tags = op->inputTags();
    int rsNum = tags.size();
    int exNum = 2;
    TString rsTag = tags.at(1);
    TString exTag = "another_tag";
    QCOMPARE(rsNum, exNum);
    QCOMPARE(rsTag, exTag);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_removeInputTag()
{
    PSetBase* base    = initIPSetTest();
    TestIOperator* op = new TestIOperator(base);

    op->addInputTag("test_operator");

    op->removeInputTag("not_tagged_with_this_one");
    TStringList tags = op->inputTags();
    bool rs = tags.empty();
    QCOMPARE(rs, false);

    op->removeInputTag("test_operator");
    tags = op->inputTags();
    rs = tags.empty();
    QCOMPARE(rs, true);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_appendOutputSet()
{
    PSetBase*      base  = initIPSetTest();
    TestIOperator* op    = new TestIOperator(base);
    TString        opTag = "test_operator";

    op->addOutputTag(opTag);

    // the tag shouldn't exist because appendOutputSet() was not called yet,
    // therefore the tag has no sets.
    QCOMPARE(op->tagExists(opTag), false);

    for(int i=0; i<10; i++) {
        op->appendOutputSet();
    }
    QCOMPARE((int)op->outputSets().size(), 10);
    QCOMPARE(op->tagExists(opTag), true);
    QCOMPARE((int)op->setsWithTag(opTag).size(), 10);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_nextOutputSet()
{
    PSetBase*      base  = initIPSetTest();
    TestIOperator* op    = new TestIOperator(base);
    TString        opTag = "test_operator";

    op->addOutputTag(opTag);

    for(int i=0; i<10; i++) {
        op->appendOutputSet();
    }

    op->resetOutputSetIterator();
    for(int i=0; i<10; i++) {
        ISet* rs = op->nextOutputSet();
        ISet* ex = op->setWithTag(opTag, i);
        QCOMPARE(rs, ex);
    }

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_removeOutputSet()
{
    PSetBase*      base  = initIPSetTest();
    TestIOperator* op    = new TestIOperator(base);

    for(int i=0; i<10; i++) {
        op->appendOutputSet();
    }

    op->removeOutputSet(9);

    int rs = op->outputSets().size();
    QCOMPARE(rs, 9);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_clearOutputSets()
{
    PSetBase*      base  = initIPSetTest();
    TestIOperator* op    = new TestIOperator(base);
    TString        opTag = "test_operator";

    op->addOutputTag(opTag);

    for(int i=0; i<10; i++) {
        op->appendOutputSet();
    }

    op->clearOutputSets();

    QCOMPARE((int)op->outputSets().size(), 0);
    QCOMPARE(op->tagExists(opTag), false);

    delete op;
    delete base;
}

void tst_baseoperations::test_IOperators_acceptableTypes()
{
    PSetBase* base  = new PSetBase();
    TestIOperator* op = new TestIOperator(base);

    cout << "Testing TP_defineSupportedElementTypes(), TP_supportedElementTypes() and isSupportedElementType()" << endl;

    TStringList types;
    types << Tigon::ETNominal << Tigon::ETOrdinal;
    op->defineSupportedElementTypes(types);
    TStringList opTypes = op->supportedElementTypes();
    int rsNum = opTypes.size();
    int exNum = 2;
    TString rsType = opTypes.at(0);
    TString exType = Tigon::ETNominal;
    QCOMPARE(rsNum, exNum);
    QCOMPARE(rsType, exType);
    bool rs = op->isSupportedElementType(Tigon::OrdinalType);
    bool ex = true;
    QCOMPARE(rs, ex);
    rs = op->isSupportedElementType(Tigon::RealType);
    ex = false;
    QCOMPARE(rs, ex);
    IElementSPtr realElement = IElementSPtr(new IElement);
    realElement->defineType(Tigon::RealType);
    rs = op->isSupportedElementType(realElement);
    ex = false;
    QCOMPARE(rs, ex);

    delete op;
    delete base;
}

void tst_baseoperations::test_elementIDGenerator()
{
    ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();

    ElementProperties eid1 = fct->createElementProperties(1, TString("Elem1"), TString("No description available for element."));
    ElementProperties eid2 = fct->createElementProperties(2, TString("Var1"), TString("Tri."));
    ElementProperties eid3 = fct->createElementProperties(2, TString("Var1"), TString("Tri thes description of this element is moot and void and null and irrelevant and meaningless and pointless and so we will not be providing it."));

    cout << "eid1->ID():  " << eid1.ID() << endl;
    cout << "length: " << eid1.ID().length() << endl;
    cout << "eid1->Idx(): " << eid1.idx() << endl;

    cout << "eid2->ID():  " << eid2.ID() << endl;
    cout << "length: " << eid2.ID().length() << endl;
    cout << "eid2->Idx(): " << eid2.idx() << endl;

    cout << "eid3->ID():  " << eid3.ID() << endl;
    cout << "length: " << eid3.ID().length() << endl;
    cout << "eid3->Idx(): " << eid3.idx() << endl;
}

////////////////
/// \brief tst_baseoperations::test_IMapping
#include <QSharedData>
#include <QSharedDataPointer>
class EmployeeData : public QSharedData
{
public:
    EmployeeData() : id(-1) { }
    EmployeeData(const EmployeeData &other)
        : QSharedData(other), id(other.id), name(other.name) { }
    ~EmployeeData() { }

    int id;
    TString name;
};

class Employee
{
public:
    Employee() { d = new EmployeeData; }
    Employee(int id, TString name) {
        d = new EmployeeData;
        setId(id);
        setName(name);
    }
    Employee(const Employee &other)
        : d (other.d)
    {
    }
    void setId(int id) { d->id = id; }
    void setName(TString name) { d->name = name; }

    int id() const { return d->id; }
    TString name() const { return d->name; }

private:
    QSharedDataPointer<EmployeeData> d;
};

void tst_baseoperations::test_SharedDataExperiemnts()
{
    Employee e1(2993, TString("Billy joe"));
    Employee* pe1 = new Employee(3002, TString("Jon cooper"));
    Employee* pe2 = pe1;
    Employee e2 = e1;

    cout << e1.id() << " " << e1.name() << endl;
    cout << e2.id() << " " << e2.name() << endl;
    e2.setId(1212);
    cout << e1.id() << " " << e1.name() << endl;
    cout << e2.id() << " " << e2.name() << endl;

    cout << pe1->id() << " " << pe1->name() << endl;
    cout << pe2->id() << " " << pe2->name() << endl;
    pe2->setId(888);
    cout << pe1->id() << " " << pe1->name() << endl;
    cout << pe2->id() << " " << pe2->name() << endl;
}

void tst_baseoperations::test_IMapping()
{
    int dN = 4;
    int oN = 2;

    ExampleFunction* func = new ExampleFunction;

    func->TP_defineNInputs(dN);
    func->redefineInputType(0, RealType);
    func->redefineInputType(1, NominalType);
    func->redefineInputType(2, IntegerType);
    func->redefineInputType(3, OrdinalType);

    func->TP_defineNOutputs(oN);
    func->redefineOutputType(0, RealType);
    func->redefineOutputType(1, NominalType);

    ProblemSPtr prob(new Problem);
    prob->appendFunction(IFunctionSPtr(func));
    prob->processProblemDefinition();

    IMappingSPtr imap1 = IMappingSPtr(new IMapping(prob));
    bool cdvtype = false;
    bool cobtype = false;
    if(imap1->decisionVar(0)->type() == Tigon::RealType &&
            imap1->decisionVar(1)->type() == Tigon::NominalType &&
            imap1->decisionVar(2)->type() == Tigon::IntegerType&&
            imap1->decisionVar(3)->type() == Tigon::OrdinalType) {
        cdvtype = true;
    }
    if(imap1->objectiveVar(0)->type() == Tigon::RealType &&
            imap1->objectiveVar(1)->type() == Tigon::NominalType) {
        cobtype = true;
    }

    TCOMPARE(cdvtype, true);
    TCOMPARE(cobtype, true);
    cout << "cdvtype: " << (cdvtype ? "true" : "false") << endl;
    cout << "cobtype: " << (cobtype ? "true" : "false") << endl;

    ElementPropertiesFactory* fct = ElementPropertiesFactory::instance();
    ElementProperties p1 = fct->createElementProperties(0, TString("Variable 1"), TString("A boring description..."), Tigon::RealType, Tigon::NonOptimization);
    ElementProperties p2 = p1;

    cout << "Element p1.elementIdx: " << p1.idx() << endl;
    cout << "Element p1.elementID : " << p1.ID() << endl;
    cout << "Element p1.elementIdx: " << p1.name() << endl;
    cout << "Element p1.elementIdx: " << p1.description() << endl;

    cout << "Element p2.elementIdx: " << p2.idx() << endl;
    cout << "Element p2.elementID : " << p2.ID() << endl;
    cout << "Element p2.elementIdx: " << p2.name() << endl;
    cout << "Element p2.elementIdx: " << p2.description() << endl;

    p2.setIdx(1);
    p2.setUnits("Units per kg/s/2");

    cout << "Element p1.elementIdx: " << p1.idx() << endl;
    cout << "Element p1.elementIdx: " << p1.name() << endl;
    cout << "Element p1.elementIdx: " << p1.description() << endl;

    cout << "Element p2.elementIdx: " << p2.idx() << endl;
    cout << "Element p2.elementIdx: " << p2.name() << endl;
    cout << "Element p2.elementIdx: " << p2.description() << endl;

    for(int i=-10; i<10; i++) {
        cout << "indexCycle(" << i << ", " << 5 << ") = " << indexCycle(i, 5) << endl;
        cout << "isInRange(" << i << ", " << 5 << ") = " << (isInRange(i, 5) ? "true" : "false") << endl;
        cout << "isInTRange(" << i << ", " << 5 << ") = " << (isInTRange(i, 5) ? "true" : "false") << endl;
    }

}
#endif

QTEST_GUILESS_MAIN(tst_baseoperations)

#include "tst_baseoperations.moc"
