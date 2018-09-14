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
#include <algorithm>

#include <tigon/Tigon.h>

DISABLE_WARNINGS
#include <eigen/Core>
#include <eigen/bench/BenchTimer.h>
ENABLE_WARNINGS

#include <boost/math/special_functions/factorials.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "multipolyregressiondata.h"

using namespace boost::uuids;
using namespace boost::math::constants;

//#define BENCHMARK

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Eigen;
//TESTED_COMPONENT=src/libs/tigon/....
class tst_tigonutilities : public QObject
{
    Q_OBJECT

public:

private slots:
    // File IO
    void test_fileIO();

    // Normalisation and distance utilities
    void test_magnitudeAndDirectionP_double();
    void test_magnitudeAndDirectionP_IElementSPtr();
    void test_normalisedDistanceP();
    void test_scale_double();
    void test_scale_IElementSPtr();
    void test_normaliseToUnitBox_double();
    void test_normaliseToUnitBox_IElementSPt();
    void test_scaleBackFromUnitBox_double();
    void test_scaleBackFromUnitBox_IElementSPt();
    void test_directedBoxedIntervalLength();
    void test_distanceP_double();
    void test_distanceP_IElementSPtr();

    // Scalarising functions
    void test_weightedChebyshev();
    void test_weightedSum();
    void test_weightedLp();
    void test_weightedChebyshevAugmented();

    // Generalised decomposition projection
    void test_gDChebyshevProject();
    void test_gDWeightedSumProject();
    void test_gDWeightedLpProject();

    // Domination utilities
    void test_nonDominanceSort_TVector();
    void test_nonDominanceSort_ISet();
    void test_nonDominatedSet_TVector();
    void test_nonDominatedSet_ISet();
    void test_solutionConstraintViolationDetermination();
    void test_preferability();

    // Decision variable correction
    void test_truncateIntoFeasibleDomain();
    void test_reflectIntoFeasibleDomain();
    void test_mapIntoToroidalDomain();

    // Misc
    void test_clearPointerVector();
    void test_IndexedVectorSort();
    void test_shuffle();
    void test_hypercubeGrid_Linspace();
    void test_hypercubeGrid();
    void test_convolution();
    void test_unique_TVector();
    void test_unique_ISet();
    void test_latinHypercubeSampling();
    void test_angleBetweenVectors();

    // Hypervolume
    void test_hypervolume();

    // Kernel Density Estimation
    void test_silvermanBandwidth();
    void test_kernelDensity1D();
    void test_kernelDensity1DCustomBandwithFunction();
    void test_kernelDensity();

    // Polynormial regression
    void test_polyRegression();
    void test_polynormialConstructor();
    void test_multiPolyRegressionPowerMap();
    void test_singleVariateAsMultiPolyRegression();
    void test_multiPolyRegression();
    void test_multiPolyRegressionBreakTorque();

    void test_generateRandomTimePreference();

#ifdef BENCHMARK
    void test_hypervolumeMemoryConsumption();
    void test_l2dist();
    void test_vectorMax();
    void test_vectorMin();
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

Tigon::Representation::ProblemSPtr createProblem(int dN, int oN, Tigon::ElementType type)
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

void ISetShuffle(ISet* set) {
    for(int i=0; i<100; i++) {
        int r = TRAND.randInt(set->size()-1);
        IMappingSPtr imap = set->at(r);
        set->remove(r);
        set->append(imap);
    }
}

inline void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(int i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

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

void tst_tigonutilities::test_fileIO()
{
    {
        // nonexist file
        TString file("no such a file.txt");
        TString content;
        content = Tigon::readAll(file);
        QCOMPARE(content.empty(), true);
    }

    random_generator gen;
    TString id = boost::lexical_cast<std::string>(gen());
    TString fileName("tigon_io_utils_" + id + ".txt");
    TString content = boost::lexical_cast<std::string>(gen());

    // Write to file
    bool res = Tigon::writeToFile(content, fileName);
    QCOMPARE(res, true);

    // Read from file
    TString content2 = Tigon::readAll(fileName);
    QCOMPARE(content2 == content, true);

    res = removeFile(fileName);
    QCOMPARE(res, true);

    content = Tigon::readAll(fileName);
    QCOMPARE(content.empty(), true);
}

void tst_tigonutilities::test_magnitudeAndDirectionP_double()
{
    // Vector to normalise is assumed to be all non-negative
    // THIS IS NOT TESTED BY THE UTILITY FUNCTION
    int vecSize = 10;
    TVector<double> vec(vecSize);
    for(int i=0; i<vecSize; i++) {
        vec[i] = double(i);
    }

    double pNorm = 2.0; //this is the default

    // The normalised vector is passed by reference.
    // TODO: consider returning a struct with the magnitude and the normalised
    //       vector.
    TVector<double> rsNormVec(vec);
    TVector<double> exNormVec(vec);

    double rsMagnitude = magnitudeAndDirectionP(rsNormVec);

    double exMagnitude = 0.0;
    for(int i=0; i<vecSize; i++) {
        exMagnitude += std::pow(vec[i], pNorm);
    }
    exMagnitude = std::pow(exMagnitude, 1.0 / pNorm);

    for(int i=0; i<vecSize; i++) {
        exNormVec[i] /= exMagnitude;
    }

    QCOMPARE(rsMagnitude, exMagnitude);
    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsNormVec[i], exNormVec[i]);
    }

    // test a different norm
    rsNormVec = vec;
    double newMagnitude = magnitudeAndDirectionP(rsNormVec, pNorm + 1.0);
    bool rs(rsMagnitude == newMagnitude);
    QCOMPARE(rs, false);

    pNorm = 3.0;
    rsNormVec = vec;
    exNormVec = vec;

    rsMagnitude = magnitudeAndDirectionP(rsNormVec, pNorm);

    exMagnitude = 0.0;
    for(int i=0; i<vecSize; i++) {
        exMagnitude += std::pow(vec[i], pNorm);
    }
    exMagnitude = std::pow(exMagnitude, 1.0 / pNorm);

    for(int i=0; i<vecSize; i++) {
        exNormVec[i] /= exMagnitude;
    }

    QCOMPARE(rsMagnitude, exMagnitude);
    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsNormVec[i], exNormVec[i]);
    }
}

void tst_tigonutilities::test_magnitudeAndDirectionP_IElementSPtr()
{
    // Compare the IElement version to the (already tested) results of double
    // Initialising the IElements with integers. The algebra shuold convert
    // the values to real values.

    int vecSize = 10;
    TVector<IElementSPtr> vec(vecSize);
    TVector<double> realVec(vecSize);
    for(int i=0; i<vecSize; i++) {
        vec[i] = IElementSPtr(new IElement(IntegerType, i));
        realVec[i] = double(i);
    }

    double pNorm = 2.0; //this is the default

    TVector<IElementSPtr> rsNormVec;
    for(int i=0; i<vecSize; i++) {
        rsNormVec.push_back(vec.at(i)->clone());
    }
    TVector<double> exNormVec(realVec);

    double rsMagnitude = magnitudeAndDirectionP(rsNormVec);
    double exMagnitude = magnitudeAndDirectionP(exNormVec);

    QCOMPARE(rsMagnitude, exMagnitude);
    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsNormVec.at(i)->value<double>(), exNormVec[i]);
    }

    // test a different norm
    for(int i=0; i<vecSize; i++) {
        rsNormVec[i]->defineValue(vec.at(i)->value());
    }
    double newMagnitude = magnitudeAndDirectionP(rsNormVec, pNorm + 1.0);
    bool rs(rsMagnitude == newMagnitude);
    QCOMPARE(rs, false);

    pNorm = 3.0;
    for(int i=0; i<vecSize; i++) {
        rsNormVec[i]->defineValue(vec.at(i)->value());
    }
    exNormVec = realVec;

    rsMagnitude = magnitudeAndDirectionP(rsNormVec, pNorm);
    exMagnitude = magnitudeAndDirectionP(exNormVec, pNorm);

    QCOMPARE(rsMagnitude, exMagnitude);
    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsNormVec.at(i)->value<double>(), exNormVec[i]);
    }
}

void tst_tigonutilities::test_normalisedDistanceP()
{
    TVector<ElementType> typeVec;
    typeVec << RealType << IntegerType << OrdinalType << NominalType;
    BoxConstraintsDataSPtr box(new BoxConstraintsData(typeVec));
    TVector<IElementSPtr> m = createIElementSPtrVector(typeVec);
    TVector<IElementSPtr> c = createIElementSPtrVector(typeVec);

    for(int i = 0; i < box->size(); i++) {
        box->defineLowerBound(i, 0);
        box->defineUpperBound(i, 10);
        m[i]->defineValue(1);
        c[i]->defineValue(1);
    }

    // Test Real, Integer, Ordinal
    c[0]->defineValue(3);
    double dist = normalisedDistanceP(m, c, box);
    double exp  = 2.0/10.0;
    QCOMPARE(dist, exp);

    c[0]->defineValue(1);
    c[1]->defineValue(3);
    dist = normalisedDistanceP(m, c, box);
    QCOMPARE(dist, exp);

    c[1]->defineValue(1);
    c[2]->defineValue(3);
    dist = normalisedDistanceP(m, c, box);
    QCOMPARE(dist, exp);

    // Test Nominal
    c[2]->defineValue(1);
    c[3]->defineValue(3);
    dist = normalisedDistanceP(m, c, box);
    exp = 1.0/10.0;
    QCOMPARE(dist, exp);

}

void tst_tigonutilities::test_scale_double()
{
    // The scaled vector is passed by reference.
    // TODO: consider pass by value and return the scaled vector.

    int vecSize = 10;
    double factor = 4.5;
    TVector<double> vec(vecSize);
    for(int i=0; i<vecSize; i++) {
        vec[i] = double(i);
    }

    TVector<double> rsScaledVec(vec);
    scale(rsScaledVec, factor);

    TVector<double> exScaledVec(vec);
    for(int i=0; i<vecSize; i++) {
        exScaledVec[i] *= factor;
    }

    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsScaledVec[i], exScaledVec[i]);
    }
}

void tst_tigonutilities::test_scale_IElementSPtr()
{
    int vecSize = 10;
    double factor = 4.5;
    TVector<IElementSPtr> vec(vecSize);
    TVector<double> realVec(vecSize);
    for(int i=0; i<vecSize; i++) {
        vec[i] = IElementSPtr(new IElement(IntegerType, i));
        realVec[i] = double(i);
    }

    TVector<IElementSPtr> rsScaledVec;
    for(int i=0; i<vecSize; i++) {
        rsScaledVec.push_back(vec.at(i)->clone());
    }
    cout << "before first call to scale()" << endl;
    scale(rsScaledVec, factor);

    TVector<double> exScaledVec(realVec);
    cout << "before second call to scale()" << endl;
    scale(exScaledVec, factor);

    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsScaledVec[i]->value<double>(), exScaledVec[i]);
    }
}

void tst_tigonutilities::test_normaliseToUnitBox_double()
{
    // The normalised vector is passed by reference.
    // TODO: consider pass by value and return the scaled vector.

    int vecSize = 10;
    TVector<double> rsNormVec(vecSize);
    TVector<double> antiIdeal(vecSize);
    TVector<double> ideal(vecSize);
    for(int i=0; i<vecSize; i++) {
        rsNormVec[i] = double(i+1);
        antiIdeal[i] = (i+1) * 2.0;
        ideal[i]     = (i+1) / 2.0;
    }

    normaliseToUnitBoxInMemory(rsNormVec, ideal, antiIdeal);
    TVector<double> exNormVec(vecSize, 1.0 / 3.0);

    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsNormVec[i], exNormVec[i]);
    }
}

void tst_tigonutilities::test_normaliseToUnitBox_IElementSPt()
{
    int vecSize = 10;
    TVector<IElementSPtr> rsNormVec(vecSize);
    TVector<IElementSPtr> exNormVec(vecSize);
    TVector<IElement>     antiIdeal(vecSize, IElement(RealType));
    TVector<IElement>     ideal(vecSize, IElement(RealType));
    for(int i=0; i<vecSize; i++) {
        rsNormVec[i] = IElementSPtr(new IElement(IntegerType, i+1));
        exNormVec[i] = IElementSPtr(new IElement(RealType, 1.0/3.0 ));
        antiIdeal[i].defineValue((i+1) * 2.0);
        ideal[i].defineValue((i+1) / 2.0);
    }

    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->defineLowerBounds(ideal);
    box->defineUpperBounds(antiIdeal);

    normaliseToUnitBoxInMemory(rsNormVec, box);

    for(int i=0; i<vecSize; i++) {
        tribool rs(*rsNormVec.at(i) == *exNormVec.at(i));
        TCOMPARE(rs, true);
    }
}

void tst_tigonutilities::test_scaleBackFromUnitBox_double()
{
    // The normalised vector is passed by reference.
    // TODO: consider pass by value and return the scaled vector.

    int vecSize = 10;
    TVector<double> vec(vecSize);
    TVector<double> antiIdeal(vecSize);
    TVector<double> ideal(vecSize);
    for(int i=0; i<vecSize; i++) {
        vec[i]       = double(i+1);
        ideal[i]     = (i+1) / 2.0;
        antiIdeal[i] = (i+1) * 2.0;
    }

    TVector<double> rsNormVec(vec);

    // normalise to unit box and scale back should produce the original vector
    normaliseToUnitBoxInMemory(rsNormVec, ideal, antiIdeal);
    scaleBackFromUnitBox(rsNormVec, ideal, antiIdeal);

    for(int i=0; i<vecSize; i++) {
        QCOMPARE(rsNormVec[i], vec[i]);
    }
}

void tst_tigonutilities::test_scaleBackFromUnitBox_IElementSPt()
{
    int vecSize = 10;
    TVector<IElementSPtr> rsNormVec(vecSize);
    TVector<IElementSPtr> vec(vecSize);
    TVector<IElement>     antiIdeal(vecSize, IElement(RealType));
    TVector<IElement>     ideal(vecSize, IElement(RealType));
    for(int i=0; i<vecSize; i++) {
        vec[i]       = IElementSPtr(new IElement(IntegerType, i+1));
        rsNormVec[i] = IElementSPtr(vec.at(i)->clone());
        antiIdeal[i].defineValue((i+1) * 2.0);
        ideal[i].defineValue((i+1) / 2.0);
    }

    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->defineLowerBounds(ideal);
    box->defineUpperBounds(antiIdeal);

    // normalise to unit box and scale back should produce the original vector
    normaliseToUnitBoxInMemory(rsNormVec, box);
    scaleBackFromUnitBox(rsNormVec, box);

    for(int i=0; i<vecSize; i++) {
        tribool rs(*rsNormVec.at(i) == *vec.at(i));
        TCOMPARE(rs, true);
    }
}

void tst_tigonutilities::test_directedBoxedIntervalLength()
{
    // Create a direction vector
    int vecSize = 10;
    TVector<double> dir(vecSize, 0.0);
    dir[0] = 1.0;

    // test length of elementary vectors
    double pNorm = 2.0; //this is the default
    double rs = directedBoxedIntervalLength(dir);
    QCOMPARE(rs, 1.0);

    // same result in other norms
    pNorm = 5.0;
    rs = directedBoxedIntervalLength(dir, pNorm);
    QCOMPARE(rs, 1.0);

    // two equal components
    dir[1] = dir[0];
    // normalise the direction vector
    toUnitVec(dir, 1.0);
    pNorm = 2.0; //this is the default
    rs = directedBoxedIntervalLength(dir);
    double ex = std::pow(2.0, 1.0 / pNorm);
    QCOMPARE(rs, ex);

    pNorm = 5.0;
    rs = directedBoxedIntervalLength(dir, pNorm);
    ex = std::pow(2.0, 1.0 / pNorm);
    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_distanceP_double()
{
    int vecSize = 10;
    TVector<double> vecA(vecSize);
    TVector<double> vecB(vecSize);
    for(int i=0; i<vecSize; i++) {
        vecA[i] = double(i);
        vecB[i] = 2.0 * double(i);
    }

    double pNorm = 2.0; //this is the default
    double rs = l2dist(vecA, vecB);
    TVector<double> diff(vecA);
    double ex = magnitudeAndDirectionP(diff, pNorm);
    QCOMPARE(rs, ex);

    pNorm = 5.0;
    rs = lpdist(vecA, vecB, pNorm);
    diff = vecA;
    ex = magnitudeAndDirectionP(diff, pNorm);
    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_distanceP_IElementSPtr()
{
    int vecSize = 10;
    TVector<IElementSPtr> vecA(vecSize);
    TVector<IElementSPtr> vecB(vecSize);
    TVector<double> realVecA(vecSize);
    TVector<double> realVecB(vecSize);
    for(int i=0; i<vecSize; i++) {
        vecA[i] = IElementSPtr(new IElement(IntegerType, i));
        vecB[i] = IElementSPtr(new IElement(IntegerType, 2*i));
        realVecA[i] = double(i);
        realVecB[i] = 2.0 * double(i);
    }

    double pNorm = 2.0; //this is the default
    double rs = l2dist(vecA, vecB);
    double ex = lpdist(realVecA, realVecB, pNorm);
    QCOMPARE(rs, ex);

    pNorm = 5.0;
    rs = lpdist(vecA, vecB, pNorm);
    ex = lpdist(realVecA, realVecB, pNorm);
    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_weightedChebyshev()
{
    // Create a vector and a weighting vector
    int   vecSize = 10;

    TVector<double> vec(vecSize);
    TVector<double> w(vecSize);
    double ex = 0.0;
    for(int i=0; i<vecSize; i++) {
        w[i]   = double(i+1);
        vec[i] = std::pow(i+1, 2.0);
        ex = std::max(ex, vec[i] * w[i] / vecSize / (vecSize+1.0) * 2.0);
    }
    toUnitVec(w, 1.0);

    double rs = weightedChebyshev(w, vec);

    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_weightedSum()
{
    // Create a vector and a weighting vector
    int vecSize = 10;
    TVector<double> vec(vecSize);
    TVector<double> w(vecSize);
    RowVectorXd    ww(vecSize);
    VectorXd       ff(vecSize);
    for(int i=0; i<vecSize; i++) {
        w[i]   = double(i+1);
        vec[i] = std::pow(i+1, 2.0);
        ww(i)  = w[i] / vecSize / (vecSize+1.0) * 2.0;
        ff(i)  = vec[i];
    }
    toUnitVec(w, 1.0);

    double rs = weightedSum(w, vec);

    double ex = ww * ff;

    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_weightedLp()
{
    // Create a vector and a weighting vector
    int   vecSize = 10;
    double p       = 5.0;

    TVector<double> vec(vecSize);
    TVector<double> w(vecSize);
    VectorXd       ww(vecSize);
    VectorXd       ff(vecSize);
    for(int i=0; i<vecSize; i++) {
        w[i]   = double(i+1);
        vec[i] = std::pow(i+1, 2.0);
        ww(i)  = w[i] / vecSize / (vecSize+1.0) * 2.0;
        ff(i)  = vec[i];
    }
    toUnitVec(w, 1.0);

    double rs = weightedLp(w, vec, p);

    // sum( (w_i*f_i)^p ) ^ (1/p)
    double ex = std::pow((ww.array() * ff.array()).array().pow(p).sum(), 1.0/p);

    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_weightedChebyshevAugmented()
{
    // Create a vector and a weighting vector
    int   vecSize = 10;

    TVector<double> vec(vecSize);
    TVector<double> w(vecSize);
    double ex = 0.0;
    for(int i=0; i<vecSize; i++) {
        w[i]   = double(i+1);
        vec[i] = std::pow(i+1, 2.0);
        ex = std::max(ex, vec[i] * w[i] / vecSize / (vecSize+1.0) * 2.0);
    }
    toUnitVec(w, 1.0);

    double term=0.0;
    for(int i=0; i<vecSize; i++) {
        term += w[i] * vec[i];
    }
    ex = ex + Tigon::AugmentedConstant*term;

    double rs = weightedChebyshevAugmented(w, vec);

    QCOMPARE(rs, ex);
}

void tst_tigonutilities::test_gDChebyshevProject()
{
    // I can't think of a good test, so I just call the function to see that
    // it runs.
    // TODO: come up with a meaningful test.
    int vecSize = 10;
    TVector<double> vec(vecSize);
    TVector<double> dir(vecSize);
    for(int i=0; i<vecSize; i++) {
        dir[i] = double(i+1);
        vec[i] = std::pow(i+1, 2.0);
    }

    ProjectedVec pVec = gDChebyshevProject(dir, vec);
}

void tst_tigonutilities::test_gDWeightedSumProject()
{
    TVector<double> vec;
    TVector<double> dir;
    TVector<double> ex;

    vec << 0.0               << 0.408032303593854 << 1.036115039661530;
    dir << 0.400000000000000 << 0.200000000000000 << 0.400000000000000;
    ex  << 0.617393215616413 << 0.308696607808207 << 0.617393215616413;
    double exM =  1.543483039041032;

    ProjectedVec pVec = gDWeightedSumProject(dir, vec);

    QCOMPARE(std::abs(pVec.magnitude - exM) < 1e-4, true);
    for(int i=1; i<3; i++) {
        QCOMPARE(std::abs(pVec.vec[i] - ex[i]) < 1e-4, true);
    }
}

void tst_tigonutilities::test_gDWeightedLpProject()
{
    TVector<double> vec;
    TVector<double> dir;
    TVector<double> ex;

    double pNorm = 0.1;
    vec << 0.501566958116607 << 0.688418648231925 << 0.494193444087938;
    dir << 0.599999999999999 << 0.200000000000000 << 0.200000000000000;
    ex  << 1.174755172425969 << 0.391585057475324 << 0.391585057475324;
    double exM =  1.957925287376618;

    ProjectedVec pVec = gDWeightedLpProject(dir, vec, pNorm);

    QCOMPARE(std::abs(pVec.magnitude - exM) < 1e-4, true);
    for(int i=1; i<3; i++) {
        QCOMPARE(std::abs(pVec.vec[i] - ex[i]) < 1e-4, true);
    }
    vec.clear();
    ex.clear();

    pNorm = 2.0;
    vec << 0.0               << 1.018641504298387 << 0.531109944387154;
    ex  << 1.989754975347112 << 0.663251658449040 << 0.663251658449040;
    exM =  3.316258292245192;

    pVec = gDWeightedLpProject(dir, vec, pNorm);

    QCOMPARE(std::abs(pVec.magnitude - exM) < 1e-4, true);
    for(int i=1; i<3; i++) {
        QCOMPARE(std::abs(pVec.vec[i] - ex[i]) < 1e-4, true);
    }
}

void tst_tigonutilities::test_nonDominanceSort_TVector()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    cout << "Testing nonDominanceSort(TVector<IMappingSPtr>)" << endl;

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
            IElement dVal(IntegerType, sol);
            IElement oVal1(IntegerType, i);
            IElement oVal2(IntegerType, j);
            sSet.at(sol)->defineDecisionVar(0, dVal);
            sSet.at(sol)->defineObjectiveVar(0, oVal1);
            sSet.at(sol)->defineObjectiveVar(1, oVal2);
            sSet.at(sol++)->defineEvaluated();
        }
    }

    bool weakDominance = true;
    TVector<TVector<IMappingSPtr>> ranks = nonDominanceSort(sSet,weakDominance);
    int rs = ranks.size();
    int ex = 5;
    cout << "Weak dominance:" << endl;
    for(int i=0; i<rs; i++) {
        cout << "Rank " << i << ":\t";
        TVector<IMappingSPtr> front = ranks.at(i);
        for(int j=0; j<front.size(); j++) {
            TVector<IElementSPtr> oVec = front.at(j)->objectiveVec();
            cout << "(" << oVec.at(0)->value<int>() <<
                    "," << oVec.at(1)->value<int>() << ")\t";
        }
        cout << endl;
    }
    QCOMPARE(rs,ex);

    weakDominance = false;
    ranks = nonDominanceSort(sSet,weakDominance);
    rs = ranks.size();
    ex = 3;
    cout << "Strong dominance:" << endl;
    for(int i=0; i<rs; i++) {
        cout << "Rank " << i << ":\t";
        TVector<IMappingSPtr> front = ranks.at(i);
        for(int j=0; j<front.size(); j++) {
            TVector<IElementSPtr> oVec = front.at(j)->objectiveVec();
            cout << "(" << oVec.at(0)->value<int>() <<
                    "," << oVec.at(1)->value<int>() << ")\t";
        }
        cout << endl;
    }
    QCOMPARE(rs,ex);
}

void tst_tigonutilities::test_nonDominanceSort_ISet()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    cout << "Testing nonDominatedSet(ISet*) after shuffling the set" << endl;

    int sSize = 15;
    ISet* sSet = new ISet();

    int dN = 1;
    int oN = 2;

    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);

    for(int i=0; i<sSize; i++) {
        sSet->append(IMappingSPtr(new IMapping(prob)));
    }

    // all integer values bounded between f1,f2 >= 1 , f1+f2 <= 6
    int sol=0;
    for(int i=1; i<=5; i++) {
        for(int j=1; j<=6-i; j++) {
            IElement dVal(IntegerType, sol);
            IElement oVal1(IntegerType, i);
            IElement oVal2(IntegerType, j);
            sSet->at(sol)->defineDecisionVar(0, dVal);
            sSet->at(sol)->defineObjectiveVar(0, oVal1);
            sSet->at(sol)->defineObjectiveVar(1, oVal2);
            sSet->at(sol++)->defineEvaluated();
        }
    }

    ISetShuffle(sSet);

    bool weakDominance = true;
    TVector<ISet*> ranks = nonDominanceSort(sSet,weakDominance);
    int rs = ranks.size();
    int ex = 5;
    cout << "Weak dominance:" << endl;
    for(int i=0; i<rs; i++) {
        cout << "Rank " << i << ":\t";
        ISet* front = ranks.at(i);
        for(int j=0; j<front->size(); j++) {
            TVector<IElementSPtr> oVec = front->at(j)->objectiveVec();
            cout << "(" << oVec.at(0)->value<int>() <<
                    "," << oVec.at(1)->value<int>() << ")\t";
        }
        cout << endl;
    }
    QCOMPARE(rs,ex);

    weakDominance = false;
    ranks = nonDominanceSort(sSet,weakDominance);
    rs = ranks.size();
    ex = 3;
    cout << "Strong dominance:" << endl;
    for(int i=0; i<rs; i++) {
        cout << "Rank " << i << ":\t";
        ISet* front = ranks.at(i);
        for(int j=0; j<front->size(); j++) {
            TVector<IElementSPtr> oVec = front->at(j)->objectiveVec();
            cout << "(" << oVec.at(0)->value<int>() <<
                    "," << oVec.at(1)->value<int>() << ")\t";
        }
        cout << endl;
    }
    QCOMPARE(rs,ex);
}

void tst_tigonutilities::test_nonDominatedSet_TVector()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    cout << "Testing nonDominanceSort(TVector<IMappingSPtr>)" << endl;

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
            IElement dVal(IntegerType, sol);
            IElement oVal1(IntegerType, i);
            IElement oVal2(IntegerType, j);
            sSet.at(sol)->defineDecisionVar(0, dVal);
            sSet.at(sol)->defineObjectiveVar(0, oVal1);
            sSet.at(sol)->defineObjectiveVar(1, oVal2);
            sSet.at(sol++)->defineEvaluated();
        }
    }

    bool weakDominance = true;
    TVector<IMappingSPtr> ndSet = nonDominatedSet(sSet,weakDominance);
    int rs = ndSet.size();
    int ex = 1;
    cout << "Weak dominance:" << endl;
    for(int i=0; i<rs; i++) {
        TVector<IElementSPtr> oVec = ndSet.at(i)->objectiveVec();
        cout << "(" << oVec.at(0)->value<int>() <<
                "," << oVec.at(1)->value<int>() << ")\t";
    }
    cout << endl;
    QCOMPARE(rs,ex);

    weakDominance = false;
    ndSet = nonDominatedSet(sSet,weakDominance);
    rs = ndSet.size();
    ex = 9;
    cout << "Strong dominance:" << endl;
    for(int i=0; i<rs; i++) {
        TVector<IElementSPtr> oVec = ndSet.at(i)->objectiveVec();
        cout << "(" << oVec.at(0)->value<int>() <<
                "," << oVec.at(1)->value<int>() << ")\t";
    }
    cout << endl;
    QCOMPARE(rs,ex);
}

void tst_tigonutilities::test_nonDominatedSet_ISet()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    cout << "Testing nonDominatedSet(ISet*) after shuffling the set" << endl;

    int sSize = 15;
    ISet* sSet = new ISet();

    int dN = 1;
    int oN = 2;

    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);

    for(int i=0; i<sSize; i++) {
        sSet->append(IMappingSPtr(new IMapping(prob)));
    }

    // all integer values bounded between f1,f2 >= 1 , f1+f2 <= 6
    int sol=0;
    for(int i=1; i<=5; i++) {
        for(int j=1; j<=6-i; j++) {
            IElement dVal(IntegerType, sol);
            IElement oVal1(IntegerType, i);
            IElement oVal2(IntegerType, j);
            sSet->at(sol)->defineDecisionVar(0, dVal);
            sSet->at(sol)->defineObjectiveVar(0, oVal1);
            sSet->at(sol)->defineObjectiveVar(1, oVal2);
            sSet->at(sol++)->defineEvaluated();
        }
    }

    ISetShuffle(sSet);

    bool weakDominance = true;
    ISet* ndSet = nonDominatedSet(sSet, weakDominance);
    int rs = ndSet->size();
    int ex = 1;
    cout << "Weak dominance:" << endl;
    for(int i=0; i<rs; i++) {
        TVector<IElementSPtr> oVec = ndSet->at(i)->objectiveVec();
        cout << "(" << oVec.at(0)->value<int>() <<
                "," << oVec.at(1)->value<int>() << ")\t";
    }
    cout << endl;
    QCOMPARE(rs,ex);

    weakDominance = false;
    ndSet = nonDominatedSet(sSet,weakDominance);
    rs = ndSet->size();
    ex = 9;
    cout << "Strong dominance:" << endl;
    for(int i=0; i<rs; i++) {
        TVector<IElementSPtr> oVec = ndSet->at(i)->objectiveVec();
        cout << "(" << oVec.at(0)->value<int>() <<
                "," << oVec.at(1)->value<int>() << ")\t";
    }
    cout << endl;
    QCOMPARE(rs,ex);
}

void tst_tigonutilities::test_solutionConstraintViolationDetermination()
{
    cout << "Testing the constraint violation determination" << endl;

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

    /// Infeasible Pareto-optimal decision-vector for first constraint only
    double x1=0.4;
    double x2=0.0;
    imap->defineDecisionVar(0,IElement(x1));
    imap->defineDecisionVar(1,IElement(x2));
    imap->evaluate();
    QCOMPARE(imap->isFeasible(), false);
    TVector<double> cVec = imap->doubleConstraintVec();
    QCOMPARE(cVec[0]>threshVec[0], true);
    QCOMPARE(cVec[1]>threshVec[1], false);

    double value = solutionConstraintViolation(imap, threshVec);
    double g = 1.0+x2;
    double c1 = (-1.0)*(g*std::exp(-x1/g) - (0.858*std::exp(-0.541*x1)));
    double expected = c1-threshVec[0];
    QCOMPARE(value,expected);

    /// Infeasible Pareto-optimal decision-vector for both constraints
    x1=0.8;
    x2=0.0;
    imap->defineDecisionVar(0,IElement(x1));
    imap->defineDecisionVar(1,IElement(x2));
    imap->evaluate();
    QCOMPARE(imap->isFeasible(), false);
    cVec = imap->doubleConstraintVec();
    QCOMPARE(cVec[0]>threshVec[0], true);
    QCOMPARE(cVec[1]>threshVec[1], true);

    value = solutionConstraintViolation(imap, threshVec);
    g = 1.0+x2;
    c1 = (-1.0)*(g*std::exp(-x1/g) - (0.858*std::exp(-0.541*x1)));
    double c2 = (-1.0)*(g*std::exp(-x1/g) - (0.728*std::exp(-0.295*x1)));
    expected = (c1-threshVec[0])+(c2-threshVec[1]);
    QCOMPARE(value,expected);
}

void tst_tigonutilities::test_preferability()
{
    /// solution a is preferred to b:
    /// solutions a and b do not meet the goal for the first objective
    /// both solutions meet the goal for the second and third objective
    /// a dominates b with respect to the first objective
    TVector<double> a = {1.5,0.5,0.5};
    TVector<double> b = {2.0,0.5,0.5};
    TVector<double> g = {1.0,1.0,1.0};

    tribool res = preferability(a,b,g);
    TCOMPARE(res, true);


    /// solution a is preferred to b:
    /// both solutions meet the goals
    /// a dominates b with respect to the third objective
    a = {0.5,0.5,0.4};
    b = {0.5,0.5,0.5};
    g = {1.0,1.0,1.0};

    res = preferability(a,b,g);
    TCOMPARE(res, true);


    /// solution a is equivalent to b:
    /// the solutions are exactly the same
    a = {2.0,0.5,0.5};
    b = {2.0,0.5,0.5};
    g = {1.0,1.0,1.0};

    res = preferability(a,b,g);
    TCOMPARE(res, incomparable);


    /// solution a is equivalent to b:
    /// both solutions meet the goals
    /// a and b are non-dominated
    a = {0.5,0.5,0.4};
    b = {0.5,0.3,0.5};
    g = {1.0,1.0,1.0};

    res = preferability(a,b,g);
    TCOMPARE(res, incomparable);


    /// solution a is equivalent to b:
    /// solution a does not meet the first objective goal, but solution b does
    /// solution b does not meet the second objective goal, but solution a does
    /// a and b are non-dominated
    a = {1.1,0.9,0.9};
    b = {0.9,1.1,0.9};
    g = {1.0,1.0,1.0};

    res = preferability(a,b,g);
    TCOMPARE(res, incomparable);


    /// solution a is preferred to b:
    /// solution b does not meet the first objective goal, but solution a does
    /// the goals for the second and third objectives have not been set
    /// solution b is better than a in the second and third objective
    a = {0.9,2.0,2.0};
    b = {1.1,1.0,1.0};
    g = {1.0, Tigon::Lowest, Tigon::Lowest};

    res = preferability(a,b,g);
    TCOMPARE(res, true);


    /// solution a is preferred to b:
    /// solution b does not meet the first and third objective goal,
    ///  but solution a does
    /// the goal for the second objective has not been set
    /// solution b is better than a in the second objective
    a = {0.9,2.0,1.0};
    b = {1.1,1.0,2.0};
    g = {1.0, Tigon::Lowest, 1.5};

    res = preferability(a,b,g);
    TCOMPARE(res, true);
}

void tst_tigonutilities::test_truncateIntoFeasibleDomain()
{
    int dN = 3;
    int oN = 1;

    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);

    IMappingSPtr imap(new IMapping(prob));
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->appendBounds(0.0, 1.0);
    imap->defineDecisionVar(0, IElement(3));

    box->appendBounds(1.0, 3.0);
    imap->defineDecisionVar(1, IElement(2));

    box->appendBounds(2.0, 6.0);
    imap->defineDecisionVar(2, IElement(1));

    truncateIntoFeasibleDomain(imap, box);

    TVector<double> truncatedInputs = imap->doubleDecisionVec();

    QCOMPARE(truncatedInputs[0], 1.0);
    QCOMPARE(truncatedInputs[1], 2.0);
    QCOMPARE(truncatedInputs[2], 2.0);
}

void tst_tigonutilities::test_reflectIntoFeasibleDomain()
{
    int dN = 6;
    int oN = 1;

    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);
    IMappingSPtr imap(new IMapping(prob));

    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(0, IElement(2));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(1, IElement(3));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(2, IElement(4));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(3, IElement(6));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(4, IElement(7));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(5, IElement(-7));

    reflectIntoFeasibleDomain(imap, box);

    TVector<double> truncatedInputs = imap->doubleDecisionVec();

    QCOMPARE(truncatedInputs[0], 2.0);
    QCOMPARE(truncatedInputs[1], 3.0);
    QCOMPARE(truncatedInputs[2], 2.0);
    QCOMPARE(truncatedInputs[3], 0.0);
    QCOMPARE(truncatedInputs[4], 1.0);
    QCOMPARE(truncatedInputs[5], 1.0);
}

void tst_tigonutilities::test_mapIntoToroidalDomain()
{
    int dN = 6;
    int oN = 1;

    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);
    IMappingSPtr imap(new IMapping(prob));

    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(0, IElement(2));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(1, IElement(3));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(2, IElement(4));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(3, IElement(6));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(4, IElement(7));

    box->appendBounds(0.0, 3.0);
    imap->defineDecisionVar(5, IElement(-7));

    mapIntoToroidalDomain(imap, box);

    TVector<double> truncatedInputs = imap->doubleDecisionVec();

    QCOMPARE(truncatedInputs[0], 2.0);
    QCOMPARE(truncatedInputs[1], 3.0);
    QCOMPARE(truncatedInputs[2], 1.0);

    // Unexpected behaviour when mapped to the boundaries:
    // Can be either of the lower or upper bound
    // (as they have the same value in torodial space)
    // QCOMPARE(truncatedInputs[3], 3.0);
    QCOMPARE(truncatedInputs[3], 0.0);

    QCOMPARE(truncatedInputs[4], 1.0);
    QCOMPARE(truncatedInputs[5], 2.0);
}

void tst_tigonutilities::test_clearPointerVector()
{
    class TestClass
    {
    public:
        TestClass(int i) : m_data(i) {cout << "Object " << m_data << " created. ";}
        ~TestClass() {cout << "Object " << m_data << " destroyed. ";}
    private:
        int m_data;
    };

    TVector<TestClass*> vec(10);
    for(int i=0; i<vec.size(); i++) {
        vec[i] = new TestClass(i);
    }
    cout << endl;
    clearPointerVector(vec);
    cout << endl;

    QCOMPARE(vec.size() == 0, true);
}

void tst_tigonutilities::test_IndexedVectorSort()
{
    TVector<double> vec;
    vec   << 1.0 << 3.0 << 2.0 << 4.0 << 6.0 << 5.0;
    TVector<int> rsInd = Tigon::indSort(vec);
    TVector<int> exInd;
    exInd << 0 << 2 << 1 << 3 << 5 << 4;
    for(int i=0; i<vec.size(); i++) {
        QCOMPARE(rsInd[i], exInd[i]);
        if(i != 0) {
            QCOMPARE(vec[rsInd[i]] > vec[rsInd[i-1]] , true);
        }
    }
}

void tst_tigonutilities::test_shuffle()
{
    // TVector<double>
    TVector<double> vec;
    vec   << 1.0 << 2.0 << 3.0 << 4.0 << 5.0 << 6.0;
    TVector<double> shuffledVec = Tigon::shuffle(vec);

    QCOMPARE(vec.size(), shuffledVec.size());
    cout << "Original vector: ";
    dispVector(vec);
    cout << "Shuffled vector: ";
    dispVector(shuffledVec);

    // TVector<TVector<double> >
    TVector<TVector<double> > vecOfVectors;
    TVector<double> realVec(4);
    for(double i = 1.0; i<=6.0; i += 1.0) {
        fill(realVec, i);
        vecOfVectors << realVec;
    }
    TVector<TVector<double> > shuffledVecOfVectors = Tigon::shuffle(vecOfVectors);

    QCOMPARE((vecOfVectors).size(), shuffledVecOfVectors.size());
    cout << "Original vector: ";
    for(int i=0; i<vecOfVectors.size(); i++) {
        dispVector(vecOfVectors[i],", ", ";   ");
    }
    cout << endl << "Shuffled vector: ";
    for(int i=0; i<shuffledVecOfVectors.size(); i++) {
        dispVector(shuffledVecOfVectors[i],", ", ";   ");
    }
    cout << endl;
}

void tst_tigonutilities::test_hypercubeGrid_Linspace()
{
    int m=1;
    TVector<int> ns({10});
    TVector<double> a(m), b(m);
    TVector<int> c(m);
    for(int i=0; i<m; i++) {
        a[i] = 0.0;
        b[i] = 1.0;
        c[i] = 1;
    }

    TVector<TVector<double>> samples = hypercubeGrid(m, ns, a, b, c);
    cout << "HypercubeGrid for linspace: "<< samples;

    QCOMPARE(samples.size() == 10, true);
    QCOMPARE(samples[0].size() == 1, true);
    QCOMPARE(samples[0][0], 0.0);
    QCOMPARE(std::abs(samples[1][0] - 0.1111111111) < Tigon::Epsilon, true);
    QCOMPARE(std::abs(samples[2][0] - 0.2222222222) < Tigon::Epsilon, true);
    QCOMPARE(samples[9][0], 1.0);
}

void tst_tigonutilities::test_hypercubeGrid()
{
    int m=2;
    TVector<int> ns({5, 3});
    TVector<double> lbs(m), ubs(m);
    TVector<int> c(m);
    for(int i=0; i<m; i++) {
        lbs[i] = 0.0;
        ubs[i] = 1.0;
        c[i] = 1;
    }

    TVector<TVector<double>> samples = hypercubeGrid(m, ns, lbs, ubs, c);
    cout << "HypercubeGrid: "<< samples;
    QCOMPARE(samples.size() == 15, true);
    QCOMPARE(samples[0].size() == 2, true);
    QCOMPARE(samples[0][0], 0.0);
    QCOMPARE(samples[1][0], 0.25);
    QCOMPARE(samples[3][0], 0.75);
    QCOMPARE(samples[4][0], 1.0);
    QCOMPARE(samples[5][1], 0.5);
    QCOMPARE(samples[10][1], 1.0);
}

void tst_tigonutilities::test_convolution()
{
    // test for double
    TVector<double> x,y,z;
    x << 1.0 << 2.0 << 3.0 << 4.0;
    y << 2.0 << 2.0 << 3.0;

    z = Tigon::conv(x,y);

    cout << fixed << "Vector x: ";
    for(int i=0; i<x.size(); i++) {
        cout << x[i] << '\t';
    }
    cout << endl;

    cout << "Vector y: ";
    for(int i=0; i<y.size(); i++) {
        cout << y[i] << '\t';
    }
    cout << endl;

    cout << "Vector z: ";
    for(int i=0; i<z.size(); i++) {
        cout << z[i] << '\t';
    }
    cout << endl;

    //test for int
    TVector<int> X,Y,Z;
    X << 2 << 2 << 3;
    Y << 1 << 2 << 3 << 4;

    Z = Tigon::conv(X,Y);

    cout << fixed << "Vector X: ";
    for(int i=0; i<X.size(); i++) {
        cout << X[i] << '\t';
    }
    cout << endl;

    cout << "Vector Y: ";
    for(int i=0; i<Y.size(); i++) {
        cout << Y[i] << '\t';
    }
    cout << endl;

    cout << "Vector Z: ";
    for(int i=0; i<Z.size(); i++) {
        cout << Z[i] << '\t';
    }
    cout << endl;
}

void tst_tigonutilities::test_unique_TVector()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    cout << "Testing uniqueSet(TVector<IMappingSPtr>)" << endl;

    int sSize = 9;
    TVector<IMappingSPtr> sSet;

    int dN = 3;
    int oN = 2;


    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);
    for(int i=0; i<sSize; i++) {
        sSet.push_back(IMappingSPtr(new IMapping(prob)));
    }

    int sol = 0;
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            IElement dVal0(IntegerType, i+j);
            IElement dVal1(IntegerType, i-j);
            IElement dVal2(IntegerType, i*j);
            IElement oVal0(IntegerType, i);
            IElement oVal1(IntegerType, j);
            sSet.at(sol)->defineDecisionVar(0, dVal0);
            sSet.at(sol)->defineDecisionVar(1, dVal1);
            sSet.at(sol)->defineDecisionVar(2, dVal2);
            sSet.at(sol)->defineObjectiveVar(0, oVal0);
            sSet.at(sol)->defineObjectiveVar(1, oVal1);
            sSet.at(sol++)->defineEvaluated();
        }
    }

    // adding duplicates
    sSet.push_back(IMappingSPtr(new IMapping(prob)));

    //    TODO - This method doesn't work - crashes in IMapping::setInputVec()
    //    in the line:
    //    oidx.remove(toremove[j]);

    //    TVector<IElement> dVec(dN, IElement(IntegerType, 0));
    //    cout << "sSet.size = " << sSet.size() << ". sol = " << sol <<
    //            ". dVec.size = " << dVec.size() << endl;
    //    sSet.at(sol)->setInputVec(dVec);

    for(int i=0; i<dN; i++) {
        sSet.at(sol)->defineDecisionVar(i, IElement(0));
    }
    TVector<IElement> oVec(oN, IElement(IntegerType, 0));
    sSet.at(sol)->defineObjectiveVec(oVec);
    sSet.at(sol++)->defineEvaluated();

    sSet.push_back(sSet.at(2));

    cout << "Execute uniqueSet()" << endl;
    TVector<IMappingSPtr> u = uniqueSet(sSet);

    QCOMPARE(u.size(), sSet.size()-2);
}

void tst_tigonutilities::test_unique_ISet()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    cout << "Testing uniqueSet(ISet*)" << endl;

    int sSize = 9;
    ISet* sSet = new ISet;

    int dN = 3;
    int oN = 2;
    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);

    for(int i=0; i<sSize; i++) {
        sSet->append(IMappingSPtr(new IMapping(prob)));
    }

    int sol = 0;
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) {
            IElement dVal0(IntegerType, i+j);
            IElement dVal1(IntegerType, i-j);
            IElement dVal2(IntegerType, i*j);
            IElement oVal0(IntegerType, i);
            IElement oVal1(IntegerType, j);
            sSet->at(sol)->defineDecisionVar(0, dVal0);
            sSet->at(sol)->defineDecisionVar(1, dVal1);
            sSet->at(sol)->defineDecisionVar(2, dVal2);
            sSet->at(sol)->defineObjectiveVar(0, oVal0);
            sSet->at(sol)->defineObjectiveVar(1, oVal1);
            sSet->at(sol++)->defineEvaluated();
        }
    }

    // adding duplicates
    sSet->append(IMappingSPtr(new IMapping(prob)));

    //    TODO - This method doesn't work - crashes in IMapping::setInputVec()
    //    in the line:
    //    oidx.remove(toremove[j]);

    //    TVector<IElement> dVec(dN, IElement(IntegerType, 0));
    //    cout << "sSet->size = " << sSet->size() << ". sol = " << sol <<
    //            ". dVec.size = " << dVec.size() << endl;
    //    sSet->at(sol)->setInputVec(dVec);

    for(int i=0; i<dN; i++) {
        sSet->at(sol)->defineDecisionVar(i, IElement(0));
    }
    TVector<IElement> oVec(oN, IElement(IntegerType, 0));
    sSet->at(sol)->defineObjectiveVec(oVec);
    sSet->at(sol++)->defineEvaluated();

    sSet->append(sSet->at(2));

    cout << "Execute uniqueSet()" << endl;
    ISet* u = uniqueSet(sSet);
    int origSize = sSet->size();
    int uniqueSize = u->size();

    QCOMPARE(uniqueSize, origSize-2);

    delete sSet;
    delete u;
}

void tst_tigonutilities::test_latinHypercubeSampling()
{
    int nDims  = 10;
    int nSamps = 4;
    TVector<TVector<int> > LHS = latinHypercubeSampling(nDims, nSamps);

    cout << "Testing latinHypercubeSampling() for " << nSamps <<
            " samples and " << nDims << " dimensions:" << endl;
    for(int i=0; i<nSamps; i++) {
        for(int j=0; j<nDims; j++) {
            cout << LHS[i][j] << " ";
        }
        cout << endl;
    }

    nDims  = 4;
    nSamps = 10;
    LHS.clear();
    LHS = latinHypercubeSampling(nDims, nSamps);
    cout << "Testing latinHypercubeSampling() for " << nSamps <<
            " samples and " << nDims << " dimensions:" << endl;
    for(int i=0; i<nSamps; i++) {
        for(int j=0; j<nDims; j++) {
            cout << LHS[i][j] << " ";
        }
        cout << endl;
    }

    nDims  = 2;
    nSamps = 100;
    LHS.clear();
    LHS = latinHypercubeSampling(nDims, nSamps);

    cout << "Testing latinHypercubeSampling() for " << nSamps <<
            " samples and " << nDims << " dimensions:" << endl;
    cout << "Run this in Matlab:" << endl << "a = [";
    for(int i=0; i<nSamps; i++) {
        for(int j=0; j<nDims; j++) {
            cout << LHS[i][j] << " ";
        }
        cout << "; ";
    }
    cout << "];" << endl << "scatter(a(:,1), a(:,2), 'filled');" << endl;
}

void tst_tigonutilities::test_angleBetweenVectors()
{
    TVector<double> x1, x2;
    double ret;

    /// The output (angles) by the following tests is given
    /// in degrees, as opposed to radians, for convenience.
    /// However, all the calculations are in radians.

    // vectors with a 0 degrees angle (or 0 radians)
    x1 << 1.0 << 0.0;
    x2 << 1.0 << 0.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << 0.0 << endl;
    //cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,0.0);
    x1.clear(); x2.clear();

    // vectors with a 30 degrees angle (or pi/6 radians)
    x1 << 1.0 << 0.0;
    x2 << qSqrt(3)/2.0 << 0.5;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/6.0*180/pi<double>() << endl;
    //cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/6.0);
    x1.clear(); x2.clear();

    // vectors with a 45 degrees angle (or pi/4 radians)
    x1 << 1.0 << 0.0;
    x2 << qSqrt(2)/2.0 << qSqrt(2)/2.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/4.0*180/pi<double>() << endl;
    //cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/4.0);
    x1.clear(); x2.clear();

    // vectors with a 60 degrees angle (or pi/3 radians)
    x1 << 1.0 << 0.0;
    x2 << 0.5 << qSqrt(3)/2.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/3.0*180/pi<double>() << endl;
    //cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/3.0);
    x1.clear(); x2.clear();

    // vectors with a 90 degrees angle (or pi/2 radians)
    x1 << 1.0 << 0.0;
    x2 << 0.0 << 1.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/2.0*180/pi<double>() << endl;
    //cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/2.0);
    x1.clear(); x2.clear();

    /// **************************************
    /// Swap the vectors of the previous tests
    /// **************************************

    cout << endl;
    cout << "Swap the vectors:" << endl;

    // vectors with a 30 degrees angle (or pi/6 radians)
    x1 << qSqrt(3)/2.0 << 0.5;
    x2 << 1.0 << 0.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/6.0*180/pi<double>() << endl;
    cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/6.0);
    x1.clear(); x2.clear();

    // vectors with a 45 degrees angle (or pi/4 radians)
    x1 << qSqrt(2)/2.0 << qSqrt(2)/2.0;
    x2 << 1.0 << 0.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/4.0*180/pi<double>() << endl;
    cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/4.0);
    x1.clear(); x2.clear();

    // vectors with a 60 degrees angle (or pi/3 radians)
    x1 << 0.5 << qSqrt(3)/2.0;
    x2 << 1.0 << 0.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/3.0*180/pi<double>() << endl;
    cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/3.0);
    x1.clear(); x2.clear();

    // vectors with a 90 degrees angle (or pi/2 radians)
    x1 << 0.0 << 1.0;
    x2 << 1.0 << 0.0;
    ret = angleDistance(x1,x2);
    //cout << "The expected angle is " << pi<double>()/2.0*180/pi<double>() << endl;
    cout << "The obtained angle is " << ret*180/pi<double>() << endl;
    QCOMPARE(ret,pi<double>()/2.0);
    x1.clear(); x2.clear();
}

void tst_tigonutilities::test_hypervolume()
{
    using namespace Tigon;
    using namespace Tigon::Representation;

    ISet* sSet = new ISet();

    int dN = 1;
    int oN = 2;
    ProblemSPtr prob = createProblem(dN, oN, Tigon::IntegerType);

    IMappingSPtr imap;

    // (1,4) , (2,3) , (3,2) , (4,1)
    for(int i=1; i<=4; i++) {
        IElement dVal(IntegerType, i-1);
        IElement oVal1(IntegerType, i);
        IElement oVal2(IntegerType, 5-i);

        imap = IMappingSPtr(new IMapping(prob));
        imap->defineDecisionVar(0, dVal);
        imap->defineObjectiveVar(0, oVal1);
        imap->defineObjectiveVar(1, oVal2);
        imap->defineEvaluated();
        sSet->append(imap);
    }
    // (2,2)
    imap = IMappingSPtr(new IMapping(prob));
    imap->defineDecisionVar(0, IElement(4));
    imap->defineObjectiveVar(0, IElement(2));
    imap->defineObjectiveVar(1, IElement(2));
    imap->defineEvaluated();
    sSet->append(imap);
    // (0,6)
    imap = IMappingSPtr(new IMapping(prob));
    imap->defineDecisionVar(0, IElement(4));
    imap->defineObjectiveVar(0, IElement(0));
    imap->defineObjectiveVar(1, IElement(6));
    imap->defineEvaluated();
    sSet->append(imap);

    TVector<TVector<double> > vSet;
    for(int i=0; i<sSet->size(); i++) {
        vSet.push_back(sSet->at(i)->doubleObjectiveVec());
    }

    cout << "Entire set:" << "\t\t";
    for(int j=0; j<sSet->size(); j++) {
        TVector<IElementSPtr> oVec = sSet->at(j)->objectiveVec();
        cout << "(" << oVec.at(0)->value<int>() <<
                "," << oVec.at(1)->value<int>() << ")\t";
    }
    cout << endl;

    bool weakDominance = true;
    ISet* nd = nonDominatedSet(sSet, weakDominance);
    int rs = nd->size();
    int ex = 4;
    cout << "Non-dominated points:" << "\t";
    for(int j=0; j<nd->size(); j++) {
        TVector<IElementSPtr> oVec = nd->at(j)->objectiveVec();
        cout << "(" << oVec.at(0)->value<int>() <<
                "," << oVec.at(1)->value<int>() << ")\t";
    }
    cout << endl;
    QCOMPARE(rs, ex);

    // reference point
    TVector<double> refPoint;
    refPoint << 5.0 << 5.0;

    cout << "Reference point: \t(" << (int)refPoint[0] << "," <<
            (int)refPoint[1] << ")" << endl;

    BenchTimer timer;
    timer.start();
    double rsHV = hypervolume(sSet, refPoint);
    timer.stop();
    double exHV = 11.0;
    cout << "Hypervolume: \t\t" << rsHV
         << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s"
         << "; Real Time: "<< timer.value(REAL_TIMER) << "s" << endl;
    QCOMPARE(rsHV, exHV);

    rsHV = hypervolume(vSet, refPoint);
    QCOMPARE(rsHV, exHV);

    // no reference point, use the largest in each dimension (4,6)
    rsHV = hypervolume(sSet);
    exHV = 10.0;
    QCOMPARE(rsHV, exHV);

    rsHV = hypervolume(vSet);
    QCOMPARE(rsHV, exHV);
}

void tst_tigonutilities::test_silvermanBandwidth()
{
    TVector<double> xx({-9.0206, -10.2656, -10.5484, -10.0963, -11.3807,
                        -10.7284,  -8.1140, -12.9414,  -9.0200, -11.1918,
                        9.5620,  11.3665,   9.1273,  10.0893,  10.2477,  10.4070,
                        10.6481,   9.8355,  10.8115,  10.4083});

    double h = silvermanBandwidth(xx);

    QCOMPARE(h, 6.173012392338832);
}

void tst_tigonutilities::test_kernelDensity1D()
{
    // Compare the new 1D implementation with the previous one
    NormalDistribution randn(0.0, 1.0);
    int n = 50;
    int nq = 2*n;

    TVector<double> xx(n);
    TVector<double> xq(nq);

    TVector<TVector<double> > xxx(n);
    TVector<TVector<double> > xqq(nq);

    for (int i=0; i<n; i++) {
        if (i<n/2) {
            xx[i] = randn.sample()-10;
            xxx[i] << xx[i];
        } else {
            xx[i] = randn.sample()+10;
            xxx[i] <<  xx[i];
        }
    }
    for(int i=0; i<nq; i++) {
        xq[i] = -25.0 + 50.0/nq * i;
        xqq[i] << xq[i];
    }
    TVector<double> kde = gaussianKDE(xx, xq);

    double bandwidth = silvermanBandwidth(xx);
    // Pass the same bandwidth to previous kde, should get the same results
    TVector<double> kde_old = gaussianKDE(xxx, xqq, bandwidth);
    for (int i=0; i<kde.size(); i++) {
        QCOMPARE(kde[i], kde_old[i]);
    }

    //cout <<"xq = " << xq;
    //cout << "kde = " << kde;
    //cout << "kde_old = " << kde_old;
    //cout << "plot(xq, kde, \'*\'); hold on; plot(xq, kde_old, \'x\');" << endl;
    //cout << "legend({\'new kde\' \'kde_old\'})" << endl;
}

void tst_tigonutilities::test_kernelDensity1DCustomBandwithFunction()
{
    NormalDistribution randn(0.0, 1.0);
    int n = 50;
    int nq = 2*n;

    TVector<double> xx(n);
    TVector<double> xq(nq);

    for (int i=0; i<n; i++) {
        if (i<n/2) {
            xx[i] = randn.sample()-10;
        } else {
            xx[i] = randn.sample()+10;
        }
    }
    for(int i=0; i<nq; i++) {
        xq[i] = -25.0 + 50.0/nq * i;
    }
    TVector<double> kde = gaussianKDE(xx, xq, silvermanBandwidth);
    TVector<double> kdeRef = gaussianKDE(xx, xq);

    for (int i=0; i<kde.size(); i++) {
        QCOMPARE(kde[i], kdeRef[i]);
    }
}

void tst_tigonutilities::test_kernelDensity()
{
    int   nPoints     = 200;
    double bandwidth   = 0.16;
    int   nQIntervals = 100; //in each dimension
    double muOne       = 3.0;
    double sigmaOne    = 1.0;
    double muTwo       = 0.0;
    double sigmaTwo    = 3.0;

    NormalDistribution* xOne = new NormalDistribution(muOne, sigmaOne);
    NormalDistribution* xTwo = new NormalDistribution(muTwo, sigmaTwo);

    // TODO - unify random seeds in Tigon
    // just so the two distributions don't use the same seed...
    for(int i=0; i < (2*nPoints); i++) {
        xOne->sample();
    }

    TVector<TVector<double> > xx;

    for(int i=0; i<nPoints; i++) {
        TVector<double> point;
        point << xOne->sample() << xTwo->sample();
        xx.push_back(point);
    }

    double lbOne  = muOne - 4.0*sigmaOne;
    double ubOne  = muOne + 4.0*sigmaOne;
    double incOne = (ubOne - lbOne) / nQIntervals;
    double lbTwo  = muTwo - 4.0*sigmaTwo;
    double ubTwo  = muTwo + 4.0*sigmaTwo;
    double incTwo = (ubTwo - lbTwo) / nQIntervals;

    TVector<TVector<double> > xq;

    double xqOne = lbOne;
    while(xqOne <= ubOne) {
        double xqTwo = lbTwo;
        while(xqTwo <= ubTwo) {
            TVector<double> xqi;
            xqi << xqOne << xqTwo;
            xq.push_back(xqi);
            xqTwo += incTwo;
        }
        xqOne += incOne;
    }

    TVector<double> kde = gaussianKDE(xx, xq, bandwidth);

    int nq = xq.size();
    QCOMPARE(kde.size() == nq, true);
    for(int i=0; i<nq; i++) {
        QCOMPARE((kde[i] >=0.0), true);
    }

    // write the results to an .m file:
    QFile file("tst_kernelDensity.m");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    out << "clc;clear;clf;\n\n";
    out << "xx = [";
    for(int i=0; i<nPoints; i++) {
        writeVector(out, xx[i], " ", "; ");
    }
    out << "];" << endl;

    out << "xq = [";
    for(int i=0; i<nq; i++) {
        writeVector(out, xq[i], " ", "; ");
    }
    out << "];" << endl;

    out << "kde = [";
    for(int i=0; i<nq; i++) {
        out << kde[i] << "; ";
    }
    out << "];\n" << endl;

    out << "%Plot the results:\n"
           "figure(1); clf;\n"
           "[xg,yg] = meshgrid(" << lbOne << ":.1:" << ubOne <<
           ", " << lbTwo << ":.1:" << ubTwo << ");" << endl;
    out << "vg = griddata(xq(:,1), xq(:,2), kde, xg, yg);\n"
           "mesh(xg, yg, vg, 'facecolor', 'none'); hold on;\n"
           "scatter3(xx(:,1), xx(:,2), zeros(1, size(xx,1)), 'filled');" << endl;

    delete xOne;
    delete xTwo;
}

void tst_tigonutilities::test_polyRegression()
{
    // X = [0 0.3 0.8 1.1 1.6 2.3];
    // Y = [0.6 0.67 1.01 1.35 1.47 1.25];
    // https://uk.mathworks.com/help/matlab/data_analysis/programmatic-fitting.html
    TVector<double> X({0, 0.3, 0.8, 1.1, 1.6, 2.3});
    TVector<double> Y({0.6, 0.67, 1.01, 1.35, 1.47, 1.25});
    PolynomialRegression reg(X, Y, 2);
    cout << reg.toString() << std::endl;
    cout << "error: " << reg.error() << endl;
    TVector<double> coeff = reg.coeff();

    // comapre wiht reuslts from matlab
    QCOMPARE(coeff[0], 0.498115354015531);
    QCOMPARE(coeff[1], 1.023142947902906);
    QCOMPARE(coeff[2], -0.294163851511839);

    double a0 = -0.9874830;
    double a1 = 9.38839;
    double a2 = 1.304830;
    double a3 = -0.984394739;

    for(std::size_t i=0; i<X.size(); i++) {
        Y[i] = a0 + a1*X[i] + a2*X[i]*X[i] + a3*X[i]*X[i]*X[i];
    }

    PolynomialRegression reg2(X, Y, 3);
    cout << reg2.toString() << std::endl;
    cout << "error: " << reg2.error() << endl;
    coeff = reg2.coeff();
    cout << coeff << endl;

    QCOMPARE(coeff[0], a0);
    QCOMPARE(coeff[1], a1);
    QCOMPARE(coeff[2], a2);
    QCOMPARE(coeff[3], a3);

    QCOMPARE(reg2.interpolate(X[2]), Y[2]);
}

void tst_tigonutilities::test_polynormialConstructor()
{
    TVector<TVector<double>> powerMap = MultiPolynomialRegression::generatePowerMap(1, 2);
    TVector<double> coeff({1.23, 0.67, -2.9347});
    MultiPolynomialRegression reg(coeff, powerMap);

    for (std::size_t i=0; i<coeff.size(); i++) {
        QCOMPARE(reg.coeff()[i], coeff[i]);
    }

    TVector<TVector<double>> X({{1.2332}, {0.83663}, {3.26583}, {-8.968947}});

    for (std::size_t i=0; i<X.size(); i++) {
        double val = coeff[0] + coeff[1]*X[i][0] + coeff[2]*X[i][0]*X[i][0];
        QCOMPARE(reg.interpolate(X[i]), val);
    }
}

void tst_tigonutilities::test_multiPolyRegressionPowerMap()
{
    int N = 20;
    int nDim = 2;

    TVector<double> Y;
    TVector<TVector<double>> X;

    X.reserve(N);
    Y.reserve(N);
    for(int i=0; i<N; i++) {
        TVector<double> temp;
        temp.reserve(nDim);
        for(int j=0; j<nDim; j++) {
            temp.push_back(0.1*(i+j));
        }
        X.push_back(temp);
        Y.push_back(0);
    }

    /// Check power map
    int degree = 3;
    MultiPolynomialRegression reg(X, Y, degree);
    for (int i=0; i<reg.powerMap().size(); i++) {
        TVector<double> v = reg.powerMap()[i];
        int sum = std::accumulate(v.begin(), v.end(), 0.0);
        QCOMPARE(sum <= degree, true);
    }
}

void tst_tigonutilities::test_singleVariateAsMultiPolyRegression()
{
    // X = [0 0.3 0.8 1.1 1.6 2.3];
    // Y = [0.6 0.67 1.01 1.35 1.47 1.25];
    // https://uk.mathworks.com/help/matlab/data_analysis/programmatic-fitting.html
    TVector<TVector<double>> X({{0}, {0.3}, {0.8}, {1.1}, {1.6}, {2.3}});
    TVector<double> Y({0.6, 0.67, 1.01, 1.35, 1.47, 1.25});
    MultiPolynomialRegression reg(X, Y, 2);
    cout << "error: " << reg.error() << endl;
    TVector<double> coeff = reg.coeff();

    // comapre wiht reuslts from matlab
    QCOMPARE(coeff[0], 0.498115354015531);
    QCOMPARE(coeff[1], 1.023142947902906);
    QCOMPARE(coeff[2], -0.294163851511839);

    double a0 = -0.9874830;
    double a1 = 9.38839;
    double a2 = 1.304830;
    double a3 = -0.984394739;

    for(std::size_t i=0; i<X.size(); i++) {
        Y[i] = a0 + a1*X[i][0] + a2*X[i][0]*X[i][0] + a3*X[i][0]*X[i][0]*X[i][0];
    }

    MultiPolynomialRegression reg2(X, Y, 3);
    cout << "error: " << reg2.error() << endl;
    coeff = reg2.coeff();
    cout << coeff << endl;

    QCOMPARE(coeff[0], a0);
    QCOMPARE(coeff[1], a1);
    QCOMPARE(coeff[2], a2);
    QCOMPARE(coeff[3], a3);

    QCOMPARE(reg2.interpolate(X[2]), Y[2]);

    cout << "Poly string: " << reg2.toString() << endl;
}

void tst_tigonutilities::test_multiPolyRegression()
{
    int N = 20;
    int nDim = 2;

    TVector<double> Y;
    TVector<TVector<double>> X;

    // Poly terms
    double a00 = 1.48474;
    double a01 = -0.83746;
    double a10 = 3.03847;
    double a02 = 0.0;
    double a11 = 5.384639;
    double a20 = 1.374339;

    X.reserve(N);
    Y.reserve(N);
    for(int i=0; i<N; i++) {
        TVector<double> Xi;
        Xi.reserve(nDim);
        for(int j=0; j<nDim; j++) {
            Xi.push_back(TRAND.randUni(10.0));
        }
        X.push_back(Xi);

        double yval = a00 + a10*Xi[0] + a01*Xi[1]
                + a02*std::pow(Xi[1],2)
                + a11*Xi[0]*Xi[1] + a20*std::pow(Xi[0],2);

        // Generate poly results
        Y.push_back(yval);
    }

    MultiPolynomialRegression reg(X,Y,2);
    //cout << X;
    //cout << Y;
    //cout << reg.powerMap() << endl;
    //cout << reg.coeff() << endl;

    double eps = 1e-12;
    QCOMPARE(reg.error() < eps, true);
    QCOMPARE(std::abs(reg.coeff()[0] - a00) < eps, true);
    QCOMPARE(std::abs(reg.coeff()[1] - a01) < eps, true);
    QCOMPARE(std::abs(reg.coeff()[2] - a10) < eps, true);
    QCOMPARE(std::abs(reg.coeff()[3] - a02) < eps, true);
    QCOMPARE(std::abs(reg.coeff()[4] - a11) < eps, true);
    QCOMPARE(std::abs(reg.coeff()[5] - a20) < eps, true);

    for (std::size_t i=0; i<X.size(); i++) {
        QCOMPARE(reg.interpolate(X[i]), Y[i]);
    }

    cout << "Poly string: " << reg.toString() << endl;
}

void tst_tigonutilities::test_multiPolyRegressionBreakTorque()
{
    int X1n = 25;
    int X2n = 16;

    TVector<double> Y(X1n*X2n);
    TVector<TVector<double>> X(X1n*X2n);

    TVector<double> X1 = linspace(0.0,600.0,X1n);
    TVector<double> X2 = linspace(750.0,4500.0,X2n);

    int counter=0;
    for(int i=0; i<X1n; i++) {
        for(int j=0; j<X2n; j++) {
            X[counter] << X1[i] << X2[j];
            Y[counter] = breakTorque[i][j];
            counter++;
        }
    }

    // check the model fit with root-mean-square
    for(int t=0; t<10; t++) {

        MultiPolynomialRegression reg(X,Y,t+2);

        cout << "Degree(" << t+2 << ")" << endl;
        double sum = 0.0;
        counter = 0;
        for(int i=0; i<X1n; i++) {
            for(int j=0; j<X2n; j++) {
                double value = reg.interpolate(X[counter]);
                sum += std::pow(value - breakTorque[i][j],2);
                counter++;

                cout << value << " ";
            }
            cout << endl;
        }
        double Erms = std::sqrt(sum/(X1n*X2n));
        cout << "RMS: " << Erms << endl << endl;
    }
}

void tst_tigonutilities::test_generateRandomTimePreference()
{
    int n = 2;
    double alpha = 0.7;
    int runNumber = generateRandomNumberTimePreference(n,alpha);
    cout << "run number: " << runNumber << endl;
}


#ifdef BENCHMARK
void tst_tigonutilities::test_hypervolumeMemoryConsumption()
{
    using namespace Tigon;
    //    using namespace Tigon::Representation;

    int nPoints = 100;
    int nDims   = 4;
    int nIters  = 100000;

    TVector<double> ref(nDims, 1.0);

    for (int i=0; i< nIters; i++) {
        // initialise the points
        TVector<TVector<double> > points;
        for(int j=0; j<nPoints; j++) {
            TVector<double> point;
            for(int k=0; k<nDims; k++) {
                double r = TRAND.randUni();
                point.push_back(r);
            }
            points.push_back(point);
        }
        hypervolume(points, ref);
    }
}

void tst_tigonutilities::test_l2dist()
{
    int n=1000000;
    TVector<qint64> vec1(n);
    TVector<qint64> vec2(n);
    for(int i=0; i<n; i++) {
        vec1[i] = TRAND.randInt(n);
        vec2[i] = TRAND.randInt(n);
    }

    Eigen::BenchTimer timer;
    double dist;
    timer.start();
    dist = l2dist(vec1, vec2);
    timer.stop();
    std::cout << "Size :" << n << ", using l2dist: dist         " << dist
              << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s" << endl;

    timer.start();
    if(vec1.size() != vec2.size()) {
        return;
    }
    dist = std::inner_product(vec1.begin(), vec1.end(), vec2.begin(), 0.0,
                              std::plus<qint64>(),
                              [](qint64 a, qint64 b) {return (a-b)*(a-b);});
    dist = std::sqrt(dist);
    timer.stop();
    std::cout << "Size :" << n << ", using std::inner_product   " << dist
              << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s" << endl;
}

void tst_tigonutilities::test_vectorMax()
{
    int n=1000000;
    TVector<int> vec(n);
    for(int i=0; i<n; i++) {
        vec[i] = TRAND.randInt(n);
    }

    Eigen::BenchTimer timer;
    timer.start();
    int maxInt = vectorMax(vec);
    timer.stop();
    std::cout << "Size :" << n << ", using vectorMax: maxInt         " << maxInt
              << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s"
              << "; Real Time: "<< timer.value(REAL_TIMER) << "s" << endl;


    timer.start();
    maxInt = *std::max_element(vec.begin(), vec.end());
    timer.stop();

    std::cout << "Size :" << n << ", using std::max_element; maxInt  " << maxInt
              << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s"
              << "; Real Time: "<< timer.value(REAL_TIMER) << "s" << endl;

}

void tst_tigonutilities::test_vectorMin()
{
    int n=1000000;
    TVector<int> vec(n);
    for(int i=0; i<n; i++) {
        vec[i] = TRAND.randInt(n);
    }
    Eigen::BenchTimer timer;
    timer.start();
    int minInt = vectorMin(vec);
    timer.stop();
    std::cout << "Size :" << n << ", using vectorMin; minInt         " << minInt
              << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s"
              << "; Real Time: "<< timer.value(REAL_TIMER) << "s" << endl;

    timer.start();
    minInt = *std::min_element(vec.begin(), vec.end());
    timer.stop();

    std::cout << "Size :" << n << ", using std::min_element; minInt  " << minInt
              << "; CPU Time : "<< timer.value(CPU_TIMER)  << "s"
              << "; Real Time: "<< timer.value(REAL_TIMER) << "s" << endl;
}
#endif

QTEST_APPLESS_MAIN(tst_tigonutilities)

#include "tst_tigonutilities.moc"
