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

#include <tigon/Tigon.h>
#include <iostream>
//#include <qmath.h>

#include <boost/math/distributions.hpp>

//#define SHOW_LOG

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace CODeM;

class tst_uncertainties : public QObject
{
    Q_OBJECT

public:

private slots:

    void test_RandomGenerator_reset();
    void test_RandomGenerator_seed();
    void test_RandomGenerator_state();
    void test_RandomGenerator_uniform_reset();
    void test_RandomGenerator_normal_states();
    void test_RandomGenerator_chiSquared_states();
    void test_RandomGenerator_multipleDistributions_state();

    void test_Distributions_ctors_dtors();
    void test_Distributions_clone();
    void test_Distributions_resolution();
    void test_Distributions_boundaries();
    void test_Distributions_defineZ();
    void test_Distributions_definePDF();
    void test_Distributions_calculateCDF();
    void test_Distributions_uniform();
    void test_Distributions_linear();
    void test_Distributions_nonParametric();
    void test_Distributions_sampled();
    void test_Distributions_sampledCDF();
    void test_Distributions_sampledPercentile();
    void test_Distributions_merged_z();
    void test_Distributions_merged_pdf();
    void test_Distributions_boundedPeak();
    void test_Distributions_craftedPeak();
    void test_Distributions_normal();

    void test_UncertaintyMapping();

    void test_DistributionAlgebra_Negation();
    void test_DistributionAlgebra_Multiplication();
    void test_DistributionAlgebra_IDistributionMultiplication();
    void test_DistributionAlgebra_Division();
    void test_DistributionAlgebra_IDistributionDivision();
    void test_DistributionAlgebra_Reciprocal();
    void test_DistributionAlgebra_Addition();
    void test_DistributionAlgebra_IDistributionAddition();
    void test_DistributionAlgebra_Subtraction();
    void test_DistributionAlgebra_IDistributionSubtraction();

    void test_Distributions_Nagation();

    void test_UncertaintyKernel_ctor_dtor();
    void test_UncertaintyKernel_symmetry();
    void test_UncertaintyKernel_remoteness();
    void test_UncertaintyKernel_oComponent();
    void test_UncertaintyKernel_dComponent();

    void test_CODeMOperations_relations();
    void test_CODeMOperations_perturbation();
    void test_CODeMDistribution_ctor();
    void test_CODeMDistribution_sample();
    void test_CODeM_pseudoProblem();

    void test_varianceConfidenceIntervals();
    void test_RobustnessIndicators();

    void test_GECCOExample_2obj();
    void test_GECCOExample_3obj();

    void test_parametricDistributionFactory();

    void test_bug_in_sample_distribution();
};

void inline TCOMPARE(tribool actual, tribool expected)
{
    bool flag;

    if(actual.value == expected.value) {
        flag = true;
    } else {
        flag = false;
    }

    //    cout<< "\t TCOMPARE: actual.value = "<< actual.value <<
    // ", expected.value = " << expected.value << ", flag = " << flag <<endl;
    QCOMPARE(flag, true);
}

void dispVector(TVector<double> vec, QString sep=", ", QString endVec="; ")
{
    for(int i=0; i<vec.size()-1; i++) {
        cout << vec[i] << sep.toStdString();
    }
    cout << vec[vec.size()-1] << endVec.toStdString();
}

void tst_uncertainties::test_RandomGenerator_reset()
{
    TVector<int> firstSequence;
    TVector<int> secondSequence;
    int nSamples = 10;
    int nValues  = 10000;

    for(int i=0; i<nSamples; i++) {
        firstSequence.push_back(TRAND.randInt(nValues));
    }

    TRAND.reset();

    for(int i=0; i<nSamples; i++) {
        secondSequence.push_back(TRAND.randInt(nValues));
    }

    for(int i=0; i<nSamples; i++) {
        cout << firstSequence[i] << "  ";
        QCOMPARE(firstSequence[i], secondSequence[i]);
    }
    cout << endl;
}

void tst_uncertainties::test_RandomGenerator_seed()
{
    TVector<int> firstSequence;
    TVector<int> secondSequence;
    TVector<int> thirdSequence;
    int nSamples = 10;
    int nValues  = 10000;
    int seed     = 1234;

    for(int i=0; i<nSamples; i++) {
        firstSequence.push_back(TRAND.randInt(nValues));
    }

    TRAND.defineSeed(seed);
    for(int i=0; i<nSamples; i++) {
        secondSequence.push_back(TRAND.randInt(nValues));
    }

    TRAND.defineSeed(seed);
    for(int i=0; i<nSamples; i++) {
        thirdSequence.push_back(TRAND.randInt(nValues));
    }

    for(int i=0; i<nSamples; i++) {
        cout << firstSequence[i] << "  ";
        QCOMPARE(secondSequence[i], thirdSequence[i]);
        QCOMPARE(firstSequence[i] == secondSequence[i], false);
    }
    cout << endl;
}

void tst_uncertainties::test_RandomGenerator_state()
{
    TVector<int> firstSequence;  // size = 3 * nSamplse
    TVector<int> secondSequence; // size = 1 * nSamplse (beginning of sequence)
    TVector<int> thirdSequence;  // size = 2 * nSamplse (end of sequence)

    int nSamples = 10;
    int nValues  = 10000;

    TRAND.defineSeed(time(NULL));

    int state = TRAND.currentState();
    QCOMPARE(state, 0);

    // entire sequence
    for(int i=0; i<3*nSamples; i++) {
        firstSequence.push_back(TRAND.randInt(nValues));
    }

    state = TRAND.currentState();
    QCOMPARE(state, 3*nSamples);

    // same sequence, but the end (seq3) is generated before the beginning (seq2)
    TRAND.defineState(nSamples);
    while(TRAND.currentState() < uint(3*nSamples)) {
        thirdSequence.push_back(TRAND.randInt(nValues));
    }

    TRAND.reset();
    while(TRAND.currentState() <  uint(nSamples)) {
        secondSequence.push_back(TRAND.randInt(nValues));
    }

    QCOMPARE(secondSequence.size() + thirdSequence.size() == firstSequence.size(), true);

    secondSequence << thirdSequence;
    for(int i=0; i<3*nSamples; i++) {
        QCOMPARE(firstSequence[i], secondSequence[i]);
    }
}

void tst_uncertainties::test_RandomGenerator_uniform_reset()
{
    int nSamples = 5;

    TVector<double> firstSequence;
    TVector<double> secondSequence;

    TRAND.defineSeed(5435);

    for(int i=0; i<nSamples; i++) {
        firstSequence.push_back(TRAND.randUni());
    }

    TRAND.reset();

    for(int i=0; i<nSamples; i++) {
        secondSequence.push_back(TRAND.randUni());
    }

    cout << "First:   " << endl;
    for(int i=0; i<nSamples; i++) {
        cout << firstSequence[i] << "  ";
    }
    cout << endl;

    cout << "Second:  " << endl;
    for(int i=0; i<nSamples; i++) {
        cout << secondSequence[i] << "  ";
        QCOMPARE(firstSequence[i], secondSequence[i]);
    }
    cout << endl;

}

void tst_uncertainties::test_RandomGenerator_normal_states()
{
    int nSamples = 10;

    TVector<double> firstSequence;
    TVector<double> secondSequence;

    TRAND.defineSeed(1234);

    // length 2*nSamples
    for(int i=0; i<2*nSamples; i++) {
        firstSequence.push_back(TRAND.randNorm());
    }

    // normal distribution calls the prng twice
    int state = TRAND.currentState();
    QCOMPARE(state, 4*nSamples);

    // set to the middle of sequence
    TRAND.defineState(2*nSamples);

    for(int i=0; i<nSamples; i++) {
        secondSequence.push_back(TRAND.randNorm());
    }

    for(int i=0; i<nSamples; i++) {
        QCOMPARE(firstSequence[i+nSamples], secondSequence[i]);
    }
}

void tst_uncertainties::test_RandomGenerator_chiSquared_states()
{
    int nSamples = 20;

    TVector<double> firstSequence;
    TVector<double> secondSequence;
    TVector<double> thirdSequence;

    TRAND.defineSeed(1234);

    cout << "first sequence with v=2.3:" << endl;
    for(int i=0; i<nSamples; i++) {
        firstSequence.push_back(TRAND.randChiSquared(2.3));
        cout << firstSequence[i] << "  ";
    }
    cout << endl;

    // boost::chi_squared_distribution does not call the prng the same amount
    // of times for every sample. Therefore, the m_state is not advanced
    int state = TRAND.currentState();
    QCOMPARE(state, 0);

    TRAND.reset();

    cout << "second sequence with v=2.3:" << endl;
    for(int i=0; i<nSamples; i++) {
        secondSequence.push_back(TRAND.randChiSquared(2.3));
        cout << secondSequence[i] << "  ";
    }
    cout << endl;

    for(int i=0; i<nSamples; i++) {
        QCOMPARE(firstSequence[i], secondSequence[i]);
    }

    TRAND.reset();

    cout << "third sequence with v=2.0:" << endl;
    for(int i=0; i<nSamples; i++) {
        thirdSequence.push_back(TRAND.randChiSquared(2.0));
        cout << thirdSequence[i] << "  ";
    }
    cout << endl;

    for(int i=0; i<nSamples; i++) {
        QCOMPARE((firstSequence[i] == thirdSequence[i]), false);
    }
}

void tst_uncertainties::test_RandomGenerator_multipleDistributions_state()
{
    TVector<double> normSequence1;
    TVector<int>   intSequence1;
    TVector<int>   intSequence2;

    int nSamples = 10;
    int nValues  = 10000;

    TRAND.defineSeed(123456);

    for(int i=0; i<nSamples; i++) {
        normSequence1.push_back(TRAND.randNorm());
    }

    for(int i=0; i<nSamples; i++) {
        intSequence1.push_back(TRAND.randInt(nValues));
    }

    int state = TRAND.currentState();
    QCOMPARE(state, 3*nSamples);

    TRAND.defineState(2*nSamples);

    for(int i=0; i<nSamples; i++) {
        intSequence2.push_back(TRAND.randInt(nValues));
    }

    for(int i=0; i<nSamples; i++) {
        QCOMPARE(intSequence1[i], intSequence2[i]);
    }


}

void tst_uncertainties::test_Distributions_ctors_dtors()
{
    //Test that the distribution can be constructed and distructed

    IDistribution*           r = new IDistribution;
    UniformDistribution*     u = new UniformDistribution;
    LinearDistribution*      l = new LinearDistribution;
    PeakDistribution*        p = new PeakDistribution;
    MergedDistribution*      m = new MergedDistribution;
    NonParaDistribution*    np = new NonParaDistribution;
    SampledDistribution*     s = new SampledDistribution;
    NormalDistribution*      n = new NormalDistribution;
    ChiSquaredDistribution* cs = new ChiSquaredDistribution;


    // Parametric constructors
    TVector<double> parameters;

    // Chi Squared [dof]
    parameters << 0.1;
    ChiSquaredDistribution* cs2 = new ChiSquaredDistribution(parameters);

    // Uniform [lowerBound, upperBound]
    parameters << 0.3;
    UniformDistribution*  u2 = new UniformDistribution(parameters);

    // Peak [tendency, locality]
    PeakDistribution*   p2 = new PeakDistribution(parameters);

    // Normal [mean, std]
    NormalDistribution*  n2 = new NormalDistribution(parameters);

    // Linear [lowerBound, upperBound, direction]
    parameters << 1.0;
    LinearDistribution*   l2 = new LinearDistribution(parameters);

    delete r;
    delete u;
    delete l;
    delete p;
    delete m;
    delete np;
    delete s;
    delete n;
    delete cs;
    delete cs2;
    delete u2;
    delete p2;
    delete n2;
    delete l2;
}

void tst_uncertainties::test_Distributions_clone()
{
    IDistribution* r = new IDistribution;
    IDistribution* c = r->clone();

    delete r;
    delete c;
}

void tst_uncertainties::test_Distributions_resolution()
{
    NonParaDistribution* np  = new NonParaDistribution;
    double rs = np->resolution();
    double ex = 0.5; // default value;
    QCOMPARE(rs, ex);

    np->defineResolution(0.3);
    rs = np->resolution();
    ex = 0.25;        // the range must be a multiple of the resolution
    QCOMPARE(rs, ex);

    UniformDistribution* u = new UniformDistribution;
    rs = u->resolution();
    ex = 1.0;        // default value for uniform distribution
    QCOMPARE(rs, ex);

    delete np;
}

void tst_uncertainties::test_Distributions_boundaries()
{
    LinearDistribution* l = new LinearDistribution;
    double lb = 4.0;
    double ub = 3.0;
    // ub is set to be a bit larger than lb when lb >= ub
    l->defineBoundaries(lb, ub);
    double rsL = l->lowerBound();
    double rsU = l->upperBound();
    QCOMPARE(rsL, 4.0);
    QCOMPARE(rsU, lb*1.001);

    ub = 10;
    // now it should work
    l->defineBoundaries(lb, ub);
    rsU = l->upperBound();
    QCOMPARE(rsU, 10.0);

    delete l;
}

void tst_uncertainties::test_Distributions_defineZ()
{
    NonParaDistribution* np  = new NonParaDistribution;

    TVector<double> z;
    z << 0.0 << 1.2 << 0.2 << 0.5 << 0.5 << 1.0 << 1.3;

    np->defineZ(z);

    TVector<double> rsZ = np->zSamples();
    // Should be sorted and without duplications
    TVector<double> exZ;
    exZ << 0.0 << 0.2 << 0.5 << 1.0 << 1.2 << 1.3;

    QCOMPARE(rsZ.size() == exZ.size(), true);

    for(int i=0; i<rsZ.size(); i++) {
        QCOMPARE(rsZ[i], exZ[i]);
    }

    delete np;
}

void tst_uncertainties::test_Distributions_definePDF()
{
    // when asking for the samples of an uninitialised distribution, a default
    // distribution, uniformly distributed between [0 1], is returned
    NonParaDistribution* np  = new NonParaDistribution;
    TVector<double> rsZ = np->zSamples();
    int defaultNSamples = rsZ.size();
    QCOMPARE(defaultNSamples > 0, true);

    TVector<double> pdf;
    np->definePDF(pdf);
    // When a non-parametric distribution is defined with an empty vector
    // (representing a pdf), nothing happens
    rsZ = np->zSamples();
    QCOMPARE(rsZ.size() == defaultNSamples, true);

    pdf << 1.0 << 2.0 << 0.0 << 1.0 << 5.0 << 2.0 << 0.0;
    np->definePDF(pdf);
    rsZ = np->zSamples();
    // Equally spaced with the same number of samples
    TVector<double> exZ;
    exZ << 0.0 << 1.0/6 << 2.0/6 << 3.0/6 << 4.0/6 << 5.0/6 << 1.0;
    QCOMPARE(rsZ.size(), exZ.size());
    for(int i=0; i<rsZ.size(); i++) {
        QCOMPARE(rsZ[i], exZ[i]);
    }

    delete np;
}

void tst_uncertainties::test_Distributions_calculateCDF()
{
    // test that the cdf is monotonically increasing, starts with 0 and ends with 1;

    TVector<IDistribution*> dists;
    NonParaDistribution* np  = new NonParaDistribution;

    TVector<double> pdf;
    pdf << 1.0 << 2.0 << 0.0 << 1.0 << 5.0 << 2.0 << 0.0;
    np->definePDF(pdf);

    dists.push_back(np);
    dists.push_back(new UniformDistribution(2.0, 4.0));
    dists.push_back(new LinearDistribution(6.0, 10.0));

    for(int i=0; i<dists.size(); i++) {
        TVector<double> rsCDF = dists[i]->cdf();
        QCOMPARE(rsCDF.front(), 0.0);
        QCOMPARE(rsCDF.back(), 1.0);
        for(int j=1; j<rsCDF.size(); j++) {
            QCOMPARE(rsCDF[j-1] <= rsCDF[j], true);
        }
    }

    clearPointerVector(dists);
}

void tst_uncertainties::test_Distributions_uniform()
{
    // Uniform distribution U(10, 20)
    UniformDistribution* u = new UniformDistribution(10, 20);

    // Calling cdf() generates z, then generates the pdf, then calculates the cdf
    TVector<double> rsCDF = u->cdf();
    TVector<double> rsPDF = u->pdf();
    TVector<double> rsZ   = u->zSamples();

    // Uniform distribution is defined by only two samples with equal pdf
    QCOMPARE(rsZ.size() == 2, true);
    QCOMPARE(rsZ[0], 10.0);
    QCOMPARE(rsZ[1], 20.0);
    QCOMPARE(rsCDF[0], 0.0);
    QCOMPARE(rsCDF[1], 1.0);
    QCOMPARE(rsPDF[0], 0.1);
    QCOMPARE(rsPDF[1], 0.1);

    delete u;
}

void tst_uncertainties::test_Distributions_linear()
{
    // linear pdf from zero to max value
    LinearDistribution* l = new LinearDistribution(3.0, 3.5);

    // Calling cdf() generates z, then generates the pdf, then calculates the cdf
    TVector<double> rsCDF = l->cdf();
    TVector<double> rsPDF = l->pdf();

    QCOMPARE(rsCDF[0], 0.0);
    QCOMPARE(rsCDF.back(), 1.0);
    QCOMPARE(rsPDF[0], 0.0);

    l->defineAscend(false);

    delete l;
}

void tst_uncertainties::test_Distributions_nonParametric()
{
    // defining the samples
    NonParaDistribution*  np = new NonParaDistribution;
    TVector<double> npZ = {0.0 , 1.0 , 2.0 , 4.0 , 5.0 , 8.0};
    np->defineZ(npZ);

    // getting the pdf without defining it returns a uniform distribution
    TVector<double> rsZ   = np->zSamples();
    TVector<double> rsPDF = np->pdf();

    QCOMPARE(rsZ.size(), rsPDF.size());
    for(int i=0; i<npZ.size(); i++) {
        QCOMPARE(rsZ[i], npZ[i]);
        QCOMPARE(rsPDF[i], 1.0/8.0);
    }

    TVector<double> npPDF = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6};
    np->definePDF(npPDF);
    TVector<double> rsCDF = np->cdf();
    QCOMPARE(rsCDF.size() == 6, true);
    QCOMPARE(rsCDF.back(), 1.0);

    // constructor with pdf - assums equal intervals within [0 1];
    NonParaDistribution* np2 = new NonParaDistribution(npPDF);
    rsZ = np2->zSamples();
    int rsSize = rsZ.size();
    QCOMPARE(rsSize == npPDF.size(), true);
    for(int i=0; i<rsSize; i++) {
        QCOMPARE(rsZ[i], (double)i/(rsSize-1));
    }

    delete np;
    delete np2;
}


void tst_uncertainties::test_Distributions_sampled()
{
    TRAND.reset();
    double exMean = 5.0;
    double exStd  = 3.0;
    NormalDistribution* n = new NormalDistribution(exMean, exStd);
    int nSamples = 10000;
    TVector<double> samps(nSamples);
    for(int i=0; i<nSamples; i++) {
        samps[i] = n->sample();
    }

    SampledDistribution* s = new SampledDistribution(samps);

    double rsMean = s->mean();
    double rsStd  = s->std();

    cout << "Expected mean:  " << exMean << " \t";
    cout << "Expected std:  " << exStd << " \t";
    cout << "Actual mean: " << rsMean << " \t";
    cout << "Actual std:  " << rsStd << endl;

    QCOMPARE(std::abs(rsMean - exMean) < 0.1, true);
    QCOMPARE(std::abs(rsStd  -  exStd) < 0.1,  true);

    delete n;
    delete s;
}

void tst_uncertainties::test_Distributions_sampledCDF()
{
    TVector<double> samps, probabilities, exZ, exPDF, exCDF, zSamps, rsZ, rsPDF,
            rsCDF, rsInterpPDF, rsInterpCDF;

    samps         << 1.0 << 9.0 << 5.0 << 3.0 << 5.0;
    probabilities << 1.0 << 1.0 << 2.0 << 2.0 << 4.0; //not normalised

    exZ   << 1.0 << 3.0 << 5.0 << 9.0;
    exPDF << 0.1 << 0.2 << 0.6 << 0.1;
    exCDF << 0.1 << 0.3 << 0.9 << 1.0;

    int nVals = exZ.size();

    for(double z=0.0; z<=10.0; z+=0.01) {
        zSamps.push_back(z);
    }
    int nSamps = zSamps.size();

    SampledDistribution* s = new SampledDistribution(samps, probabilities);


    rsZ   = s->zSamples();
    rsPDF = s->pdf();
    rsCDF = s->cdf();
    QCOMPARE(rsZ.size() ==  nVals, true);
    QCOMPARE(rsPDF.size() ==  nVals, true);
    QCOMPARE(rsCDF.size() ==  nVals, true);

    for(int i=0; i<nVals; i++) {
        QCOMPARE(rsZ[i], exZ[i]);
        QCOMPARE(rsPDF[i], exPDF[i]);
        QCOMPARE(rsCDF[i], exCDF[i]);
    }

    // Discrete distribution pdf with spikes at the samples and 0.0 otherwise
    rsInterpPDF = s->pdf(zSamps);
    QCOMPARE(rsInterpPDF.size() == nSamps, true);
    for(int i=0; i<nSamps; i++) {
        int idx = vectorIndexOf(rsZ, zSamps[i]);
        if(idx == -1) {
            // not a sampled value
            QCOMPARE(rsInterpPDF[i], 0.0);
        } else {
            // sampled value
            QCOMPARE(rsInterpPDF[i], rsPDF[idx]);
        }
    }

    rsInterpCDF = s->cdf(zSamps);
    QCOMPARE(rsInterpCDF.size() == nSamps, true);
    for(int i=0; i<nSamps; i++) {
        if(zSamps[i] < rsZ[0]) {
            QCOMPARE(rsInterpCDF[i], 0.0);
        } else if(zSamps[i] < rsZ[1]) {
            QCOMPARE(rsInterpCDF[i], rsCDF[0]);
        } else if(zSamps[i] < rsZ[2]) {
            QCOMPARE(rsInterpCDF[i], rsCDF[1]);
        } else if(zSamps[i] < rsZ[3]) {
            QCOMPARE(rsInterpCDF[i], rsCDF[2]);
        } else {
            QCOMPARE(rsInterpCDF[i], rsCDF[3]); // = 1.0
        }
    }

    delete s;
}

void tst_uncertainties::test_Distributions_sampledPercentile()
{
    TVector<double> samps, probabilities, percentilesVec, exPercVals;

    samps         << 1.0 << 9.0 << 5.0 << 3.0 << 5.0;
    probabilities << 1.0 << 1.0 << 2.0 << 2.0 << 4.0; //not normalised

    // for reference
    //    exZ   << 1.0 << 3.0 << 5.0 << 9.0;
    //    exPDF << 0.1 << 0.2 << 0.6 << 0.1;
    //    exCDF << 0.1 << 0.3 << 0.9 << 1.0;

    percentilesVec << -1.0 << 0.05 << 0.30001 << 0.5 << 0.95 << 0.9999 << 1.00001 << 2.0;
    exPercVals     <<  1.0 << 1.0  << 3.0     << 3.0 << 5.0  << 5.0    << 9.0     << 9.0;

    int nPerc = percentilesVec.size();

    SampledDistribution* s = new SampledDistribution(samps, probabilities);

    for(int i=0; i<nPerc; i++) {
        double val = s->percentile(percentilesVec[i]);
        QCOMPARE(val, exPercVals[i]);
    }

    delete s;
}

void tst_uncertainties::test_Distributions_merged_z()
{
    UniformDistribution*  u = new UniformDistribution(2.0, 10.0);
    UniformDistribution* uu = new UniformDistribution(-5.0, 5.0);
    NonParaDistribution* np = new NonParaDistribution;
    MergedDistribution*   m = new MergedDistribution;

    TVector<double> npZ;
    npZ << 0.0 << 1.0 << 2.0 << 4.0 << 5.0 << 8.0;
    np->defineZ(npZ);
    // The pdf of the merged distribution is a linear combination with
    // coefficients (ratios between the distributions) of 1:2:3, i.e.
    // pdf(m) = 1*pdf(u) + 2*pdf(uu) + 3*pdf(np)
    m->appendDistribution(u, 1.0);
    m->appendDistribution(uu, 2.0);
    m->appendDistribution(np, 3.0);

    TVector<double> rsZ =  m->zSamples();

    QCOMPARE(rsZ.size() == 8, true);

    for(int i=0; i<rsZ.size()-1; i++) {
        QCOMPARE(rsZ[i]<rsZ[i+1], true);
    }

    // Append a distribution after generateZ() was called:
    NonParaDistribution* np2 = new NonParaDistribution;
    TVector<double> np2Z;
    np2Z << -8.0 << -5.0 << 0.0 << 4.5 << 12.0 << 13.0;
    np2->defineZ(np2Z);
    m->appendDistribution(np2, 4.0);

    rsZ =  m->zSamples();

    QCOMPARE(rsZ.size() == 12, true);

    for(int i=0; i<rsZ.size()-1; i++) {
        QCOMPARE(rsZ[i]<rsZ[i+1], true);
    }

    delete m;
}

void tst_uncertainties::test_Distributions_merged_pdf()
{
    UniformDistribution* u  = new UniformDistribution(2.0, 10.0);
    UniformDistribution* uu = new UniformDistribution(-5.0, 5.0);
    NonParaDistribution* np = new NonParaDistribution;
    MergedDistribution*   m = new MergedDistribution;

    //define the non-parametric distribution
    TVector<double> npZ;
    npZ << 0.0 << 1.0 << 2.0 << 4.0 << 5.0 << 8.0;
    np->defineZ(npZ);
    TVector<double> npPDF;
    npPDF << 1.0 << 4.0 << 9.0 << 4.0 << 1.0 << 0.0;
    np->definePDF(npPDF);

    //ratios of 1:2:3
    m->appendDistribution(u, 1.0);
    m->appendDistribution(uu, 2.0);
    m->appendDistribution(np, 3.0);

    TVector<double> rsPDF =  m->pdf();
    QCOMPARE(rsPDF.size() == 8, true);

    TVector<double> rsCDF =  m->cdf();
    QCOMPARE(rsCDF.back(), 1.0);

    // Append a distribution after generatePDF() was called:
    TVector<double> np2Z;
    np2Z << -8.0 << -5.0 << 0.0 << 4.5 << 12.0 << 13.0;
    TVector<double> np2PDF;
    np2PDF << 1.0 << 1.0 << 2.0 << 1.0 << 0.0 << 0.0;

    NonParaDistribution* np2 = new NonParaDistribution(np2Z, np2PDF);

    m->appendDistribution(np2, 4.0);
    rsPDF = m->pdf();
    rsCDF = m->cdf();
    QCOMPARE(rsPDF.size() == 12, true);
    QCOMPARE(rsCDF.back(), 1.0);

    delete m;
}

void tst_uncertainties::test_Distributions_boundedPeak()
{
    // This test creates a bounded distribution, with zero probability in the
    // lower and upper bounds, defined by tendency and locallity.
    // The zSamples and pdf are printed as MATLAB syntax
    PeakDistribution* p = new PeakDistribution(0.0, 0.2);

    p->generatePDF();
    TVector<double> rsPDF = p->pdf();
    TVector<double> rsZ   = p->zSamples();

    cout << "z =  [";
    for(int i=0; i<rsZ.size(); i++) {
        cout << rsZ[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pdf = [";
    for(int i=0; i<rsPDF.size(); i++) {
        cout << rsPDF[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    TVector<double> rsCDF = p->cdf();
    QCOMPARE(rsCDF.back(), 1.0);

    delete p;
}

void dispDistProperties(IDistribution* dist, double perc)
{
    std::cout << "Distribution with parameters"
              << ": " << dist->parameters() << std::endl;
    std::cout << "median             =" << dist->median() << std::endl;
    std::cout << "mean               =" << dist->mean() << std::endl;
    std::cout << perc*100 << "th percentile =" << dist->percentile(perc) << endl;
}

void tst_uncertainties::test_Distributions_craftedPeak()
{
    // A test to study the statistics of different parameter settings for the
    // PeakDistribution

    // parameters are: tendency, locality, scale, bias
    // scale and bias use default of 1.0 and 0.0, respectively
    TVector<double> params;
    params << 0.13 << 0.018;
    PeakDistribution* p = new PeakDistribution(params);

    double perc = 0.975;

    dispDistProperties(p, perc);

    // add bias and scale
    params.clear();
    params << 0.72 << 0.0 << 0.15 << 0.95;
    perc = 0.2;
    p->defineParameters(params);
    dispDistProperties(p, perc);

    SampledDistribution* s = new SampledDistribution;
    for(int i=0; i<10000; i++) {
        s->addSample(p->sample());
    }
    qDebug() << "sampled median           =" << s->median();
    qDebug() << "sampled mean             =" << s->mean();
    qDebug() << "sampled" << perc*100 << "th percentile =" << s->percentile(perc) << endl;


    delete p;
}

void tst_uncertainties::test_Distributions_normal()
{
    NormalDistribution* nd = new NormalDistribution;
    cout << "Samples from a normal distribution:" << endl;
    for(int i=0; i<100; i++) {
        cout << nd->sample() << '\t';
    }
    cout << endl;
    double rs = nd->mean();
    QCOMPARE(rs, 0.0);
    rs = nd->std();
    QCOMPARE(rs, 1.0);
    rs = 1.0 - nd->cdf(-1.0) * 2;  // the probability within +- 1simga
    double ex = 0.682689492137;
    QCOMPARE(rs, ex);

    delete nd;
}

NonParaDistribution* newDist()
{
    TVector<double> z;
    z << -2.0 << -1.1 << -0.5 << 0 << 0.4 << 1.1 << 1.5 << 2.5;
    TVector<double> pdf;
    pdf << 1 << 2 << 3 << 2 << 1.5 << 0.5 << 0 << 0;

    NonParaDistribution* ret = new NonParaDistribution(z, pdf);
    return ret;
}

void tst_uncertainties::test_UncertaintyMapping()
{
    // Convert a Nominal variable with choices 0 and 1 to Normal distributions
    // 0: N(0.9,0.05),  1: N(1.25,0.06)

    DistributionType distType = NormalDistType;
    TVector<double> constTerms;
    constTerms << 0.9 << 0.05;
    TVector<double> linTerms;
    linTerms << 0.35 << 0.01;

    IElementSPtr el0(new IElement(NominalType, 0));
    IElementSPtr el1(new IElement(NominalType, 1));

    IDistribution* dist = el0->dist();
    QCOMPARE(dist==nullptr, true);

    UncertaintyMapping uMap;
    uMap.defineDistributionType(distType);
    uMap.defineConstTerms(constTerms);
    uMap.defineLinearTerms(linTerms);

    uMap.evaluateUncertainty(el0);
    uMap.evaluateUncertainty(el1);

    dist = el0->dist();
    QCOMPARE(dist==nullptr, false);
    QCOMPARE(dist->type(), distType);
    QCOMPARE(dist->mean(), 0.9);
    QCOMPARE(dist->std(), 0.05);

    dist = el1->dist();
    QCOMPARE(dist==nullptr, false);
    QCOMPARE(dist->type(), distType);
    QCOMPARE(dist->mean(), 1.25);
    QCOMPARE(dist->std(), 0.06);
}

void tst_uncertainties::test_DistributionAlgebra_Negation()
{
    IDistribution* v1 = newDist();
    IDistribution* v2 = v1->clone();
    v2->negate();

    TVector<double> z1 = v1->zSamples();
    TVector<double> z2 = v2->zSamples();
    QCOMPARE(z1[0],-z2[7]);

    delete v1;
    delete v2;
}

void tst_uncertainties::test_DistributionAlgebra_Multiplication()
{
    IDistribution* v1 = newDist();
    IDistribution* v2 = v1->clone();

    qint64 multInt = 3;
    double  multReal = 3.0;

    v2->multiply(multInt);
    double ex = v1->zSamples()[2] * 3.0;
    double rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v2;
    v2 = v1->clone();
    v2->multiply(multReal);
    rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v1;
    delete v2;
}

void tst_uncertainties::test_DistributionAlgebra_IDistributionMultiplication()
{
    TVector<double> z1;
    z1 << -6.0 << 4.0 << 4.5 << 6.0 << 6.5 << 8;
    TVector<double> pdf1;
    pdf1 << 4 << 1 << 3 << 2 << 4 << 4;

    IDistribution* v1 = new NonParaDistribution(z1, pdf1);

    TVector<double> z2;
    z2 << -1.0 << 1.0 << 5.0 << 6.0 << 7.0 << 11.0;
    TVector<double> pdf2;
    pdf2 << 0.5 << 1 << 2 << 2 << 3 << 1;

    IDistribution* v2 = new NonParaDistribution(z2, pdf2);

    IDistribution* v3 = v1->clone();
    v3->multiply(v2);

    cout << "% IDistribution multiplication" << endl;
    cout << "x =  [";
    for(int i=0; i<v1->zSamples().size(); i++) {
        cout << v1->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pX = [";
    for(int i=0; i<v1->pdf().size(); i++) {
        cout << v1->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;


    cout << "y =  [";
    for(int i=0; i<v2->zSamples().size(); i++) {
        cout << v2->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pY = [";
    for(int i=0; i<v2->pdf().size(); i++) {
        cout << v2->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;


    cout << "v =  [";
    for(int i=0; i<v3->zSamples().size(); i++) {
        cout << v3->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pV = [";
    for(int i=0; i<v3->pdf().size(); i++) {
        cout << v3->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    delete v1;
    delete v2;
    delete v3;
}

void tst_uncertainties::test_DistributionAlgebra_Division()
{
    IDistribution* v1 = newDist();
    IDistribution* v2 = v1->clone();

    qint64  divInt = 3;
    double  divReal = 3.0;

    v2->divide(divInt);
    double ex = v1->zSamples()[2] / 3.0;
    double rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v2;
    v2 = v1->clone();
    v2->divide(divReal);
    rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v1;
    delete v2;
}

void tst_uncertainties::test_DistributionAlgebra_IDistributionDivision()
{
    TVector<double> z1;
    z1 << -6.0 << 4.0 << 4.5 << 6.0 << 6.5 << 8;
    TVector<double> pdf1;
    pdf1 << 4 << 1 << 3 << 2 << 4 << 4;

    IDistribution* v1 = new NonParaDistribution(z1, pdf1);

    TVector<double> z2;
    z2 << -1.0 << 1.0 << 5.0 << 6.0 << 7.0 << 11.0;
    TVector<double> pdf2;
    pdf2 << 0.5 << 1 << 2 << 2 << 3 << 1;

    IDistribution* v2 = new NonParaDistribution(z2, pdf2);

    IDistribution* v3 = v1->clone();
    v3->divide(v2);

    cout << "% IDistribution division" << endl;
    cout << "x =  [";
    for(int i=0; i<v1->zSamples().size(); i++) {
        cout << v1->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pX = [";
    for(int i=0; i<v1->pdf().size(); i++) {
        cout << v1->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;


    cout << "y =  [";
    for(int i=0; i<v2->zSamples().size(); i++) {
        cout << v2->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pY = [";
    for(int i=0; i<v2->pdf().size(); i++) {
        cout << v2->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;


    cout << "q =  [";
    for(int i=0; i<v3->zSamples().size(); i++) {
        cout << v3->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pQ = [";
    for(int i=0; i<v3->pdf().size(); i++) {
        cout << v3->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    delete v1;
    delete v2;
    delete v3;
}

void tst_uncertainties::test_DistributionAlgebra_Reciprocal()
{
    TVector<double> z;
    z << 4.0 << 4.5 << 6.0 << 6.5 << 8;
    TVector<double> pdf;
    pdf << 1 << 3 << 2 << 4 << 4;

    IDistribution* v1 = new NonParaDistribution(z, pdf);

    IDistribution* v2 = v1->clone();
    v2->reciprocal();
    v2->multiply(2.0);

    cout << "% IDistribution reciprocal" << endl;
    cout << "x =  [";
    for(int i=0; i<v1->zSamples().size(); i++) {
        cout << v1->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pX = [";
    for(int i=0; i<v1->pdf().size(); i++) {
        cout << v1->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "r =  [";
    for(int i=0; i<v2->zSamples().size(); i++) {
        cout << v2->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pR = [";
    for(int i=0; i<v2->pdf().size(); i++) {
        cout << v2->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    delete v1;
    delete v2;
}

void tst_uncertainties::test_DistributionAlgebra_Addition()
{
    IDistribution* v1 = newDist();
    IDistribution* v2 = v1->clone();

    qint64 addInt = 3;
    double addReal = 3.0;

    v2->add(addInt);
    double ex = v1->zSamples()[2] + 3.0;
    double rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v2;
    v2 = v1->clone();
    v2->add(addReal);
    rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v1;
    delete v2;
}

void tst_uncertainties::test_DistributionAlgebra_IDistributionAddition()
{
    TVector<double> z1;
    z1 << -6.0 << 4.0 << 4.5 << 6.0 << 6.5 << 8;
    //    TVector<double> pdf1(z1.size() == 1.0); //even distribution
    TVector<double> pdf1;
    pdf1 << 4 << 1 << 3 << 2 << 4 << 4;

    IDistribution* v1 = new NonParaDistribution(z1, pdf1);

    TVector<double> z2;
    z2 << -1.0 << 1.0 << 5.0 << 6.0 << 7.0 << 11.0;
    //    TVector<double> pdf2(z2.size() == 1.0); //even distribution
    TVector<double> pdf2;
    pdf2 << 0.5 << 1 << 2 << 2 << 3 << 1;

    IDistribution* v2 = new NonParaDistribution(z2, pdf2);

    IDistribution* v3 = v1->clone();
    v3->add(v2);

    cout << "% IDistribution addition" << endl;

    cout << "z =  [";
    for(int i=0; i<v3->zSamples().size(); i++) {
        cout << v3->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pZ = [";
    for(int i=0; i<v3->pdf().size(); i++) {
        cout << v3->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    delete v1;
    delete v2;
    delete v3;

}

void tst_uncertainties::test_DistributionAlgebra_Subtraction()
{
    IDistribution* v1 = newDist();
    IDistribution* v2 = v1->clone();

    qint64 subInt = 3;
    double subReal = 3.0;

    v2->subtract(subInt);
    double ex = v1->zSamples()[2] - 3.0;
    double rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v2;
    v2 = v1->clone();
    v2->subtract(subReal);
    rs = v2->zSamples()[2];
    QCOMPARE(rs, ex);

    delete v1;
    delete v2;
}

void tst_uncertainties::test_DistributionAlgebra_IDistributionSubtraction()
{
    TVector<double> z1;
    z1 << -6.0 << 4.0 << 4.5 << 6.0 << 6.5 << 8;
    TVector<double> pdf1;
    pdf1 << 4 << 1 << 3 << 2 << 4 << 4;

    IDistribution* v1 = new NonParaDistribution(z1, pdf1);

    TVector<double> z2;
    z2 << -1.0 << 1.0 << 5.0 << 6.0 << 7.0 << 11.0;
    TVector<double> pdf2;
    pdf2 << 0.5 << 1 << 2 << 2 << 3 << 1;

    IDistribution* v2 = new NonParaDistribution(z2, pdf2);

    IDistribution* v3 = v1->clone();
    v3->subtract(v2);

    cout << " %IDistribution subtraction" << endl;
    cout << "z1 =  [";
    for(int i=0; i<v1->zSamples().size(); i++) {
        cout << v1->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pdf1 = [";
    for(int i=0; i<v1->pdf().size(); i++) {
        cout << v1->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;


    cout << "z2 =  [";
    for(int i=0; i<v2->zSamples().size(); i++) {
        cout << v2->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pdf2 = [";
    for(int i=0; i<v2->pdf().size(); i++) {
        cout << v2->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "zM =  [";
    for(int i=0; i<v3->zSamples().size(); i++) {
        cout << v3->zSamples()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    cout << "pZm = [";
    for(int i=0; i<v3->pdf().size(); i++) {
        cout << v3->pdf()[i] << " ";
        if(!((i+1)%500)) {
            cout << "...\n";
        }
    }
    cout << "];" << endl;

    delete v1;
    delete v2;
    delete v3;
}

void tst_uncertainties::test_Distributions_Nagation()
{
    TVector<IDistribution*> dists;
    TVector<double> params;
    //[0] uniform
    params << -0.3 << 1.7;
    dists << new UniformDistribution(params);

    //[1] normal
    dists << new NormalDistribution(params);

    //[2] peak
    params.clear();
    params << 0.2 << 0.5 << 2.0 << -5.0;
    dists << new PeakDistribution(params);

    //[3] linear
    params.clear();
    params << 1.0 << 5.0 << -1.0;
    dists << new LinearDistribution(params);

    //[4] sampled
    params << 3.0 << 5.0 << 1.5 << 1.0;
    dists << new SampledDistribution(params); // samples

    //[5] non-parametric
    params.clear();
    params << 0.1 << 0.2 << 0.3 << 0.25 << 0.15;
    dists << new NonParaDistribution(params); // pdf

    //[6] merged
    MergedDistribution* m = new MergedDistribution;
    m->appendDistribution(dists[0]->clone());
    m->appendDistribution(dists[1]->clone());
    m->appendDistribution(dists[2]->clone());
    dists << m;

    // chi squared should not be negated

    for(int i = 0; i < dists.size(); i++) {
        qDebug() << "Distribution" << i;
        TVector<double> originalPdf      = dists[i]->pdf();
        TVector<double> originalZSamples = dists[i]->zSamples();

        dists[i]->negate();

        TVector<double> newPdf      = dists[i]->pdf();
        TVector<double> newZSamples = dists[i]->zSamples();

        int s = originalZSamples.size();
        QCOMPARE(s > 1, true);
        QCOMPARE(s == newZSamples.size(), true);
        QCOMPARE(s == originalPdf.size(), true);
        QCOMPARE(s == newPdf.size(), true);

        for(int j = 0; j < s; j++) {
            QCOMPARE(std::abs(originalZSamples[j] - (-newZSamples[s-1-j])) < Epsilon, true);
            cout << std::abs(originalPdf[j] - newPdf[s-1-j]) << endl;
            QCOMPARE(std::abs(originalPdf[j] - newPdf[s-1-j]) < Epsilon, true);
        }
    }

    clearPointerVector(dists);
}

void tst_uncertainties::test_UncertaintyKernel_ctor_dtor()
{
    // test the constructors of UncertaintyKernel

    TVector<double> inputs(10, 1.0);
    TVector<double> outputs(5, 2.0);
    TVector<IElement> lb(10, IElement(RealType, 0.0));
    TVector<IElement> ub(10, IElement(RealType, 5.0));
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->defineLowerBounds(lb);
    box->defineUpperBounds(ub);
    TVector<double> ideal(5, -10.0);
    TVector<double> antiIdeal(5, 10.0);

    // Create and destroy an UncertaintyKernel
    UncertaintyKernel* cm = new UncertaintyKernel(inputs, outputs, box);
    delete cm;

    cm = new UncertaintyKernel(inputs, outputs, box, ideal, antiIdeal);
    delete cm;
}

void tst_uncertainties::test_UncertaintyKernel_symmetry()
{
    TVector<double> inputs(10, 1.0);
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);

    // Centre - symmetry() = 1
    TVector<double> outputs(3, 5.0);

    UncertaintyKernel* cm = new UncertaintyKernel(inputs, outputs, box);
    double rsSym = cm->symmetry();
    QCOMPARE(rsSym, 1.0);
    delete cm;

    // In range
    outputs[0] = 0.0;
    cm = new UncertaintyKernel(inputs, outputs, box);
    rsSym = cm->symmetry();
    QCOMPARE(rsSym<1.0 && rsSym>0.0 , true);
    delete cm;

    // Boundary - symmerty() = 0
    outputs[1] = 0.0;
    cm = new UncertaintyKernel(inputs, outputs, box);
    rsSym = cm->symmetry();
    QCOMPARE(rsSym, 0.0);
    delete cm;
}

void tst_uncertainties::test_UncertaintyKernel_remoteness()
{
    TVector<double> inputs(10, 1.0);
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);

    TVector<double> outputs(3);
    for(int i=0; i<outputs.size(); i++) {
        outputs[i] = 2.0 * i;
    }

    TVector<double> ideal(3, -10.0);
    TVector<double> antiIdeal(3, 10.0);

    // Directed boundaries use 2-norm normalised objective values
    double lb = 0.4;
    double ub = 1.1;

    // Create an UncertaintyKernel
    UncertaintyKernel* cm = new UncertaintyKernel(inputs, outputs, box,
                                                  lb, ub, ideal, antiIdeal);

    //remoteness is defined as the value on the interval between lb and ub
    double rsP = cm->remoteness();
    double exDist = sqrt(0.25+0.36+0.49);
    double exP = (exDist-0.4) / (ub-lb);

    QCOMPARE(rsP, exP);
    rsP = cm->remoteness();
    QCOMPARE(rsP, exP);

    delete cm;
}

void tst_uncertainties::test_UncertaintyKernel_oComponent()
{
    // oComponent(i) returns the normalised objective f_i

    TVector<double> inputs(10, 1.0);
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);

    TVector<double> outputs(3);
    for(int i=0; i<outputs.size(); i++) {
        outputs[i] = 2.0 * i;
    }

    TVector<double> ideal(3, -10.0);
    TVector<double> antiIdeal(3, 10.0);

    // Create an UncertaintyKernel
    UncertaintyKernel* cm = new UncertaintyKernel(inputs, outputs, box,
                                                  ideal, antiIdeal);

    double rsD = cm->oComponent(0);
    QCOMPARE(rsD, 0.5);
    rsD = cm->oComponent(1);
    QCOMPARE(rsD, 0.6);

    delete cm;
}

void tst_uncertainties::test_UncertaintyKernel_dComponent()
{
    // dComponent(i) returns the normalised decision variable d_i
    TVector<double> inputs(5);
    TVector<double> outputs;
    TVector<IElement> lb(5, IElement(RealType, 0.0));
    TVector<IElement> ub(5, IElement(RealType, 10.0));
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->defineLowerBounds(lb);
    box->defineUpperBounds(ub);

    for(int i=0; i<inputs.size(); i++) {
        inputs[i] = (double)i;
    }

    // Create a UncertaintyKernel
    UncertaintyKernel* cm = new UncertaintyKernel(inputs, outputs, box);

    double rsD = cm->dComponent(0);
    QCOMPARE(rsD, 0.0);
    rsD = cm->dComponent(1);
    QCOMPARE(rsD, 0.1);

    delete cm;
}

void tst_uncertainties::test_CODeMOperations_relations()
{
    // test CODeM utility functions

    double val = 0.3;

    double rs = linearDecrease(val);
    double ex = 1.0 - val;
    QCOMPARE(rs, ex);

    double alpha = 3.0;

    rs = skewedIncrease(val, alpha);
    ex = std::pow(val, alpha);
    QCOMPARE(rs, ex);

    rs = skewedDecrease(val, alpha);
    ex = 1.0 - std::pow(val, alpha);
    QCOMPARE(rs, ex);

    double loc = 0.4;
    double width = 0.3;

    rs = lowOnValue(val, loc, width);
    ex = 4.0/std::pow(width, 2.0) * std::pow(val-loc, 2.0);
    QCOMPARE(rs, ex);

    rs = highOnValue(val, loc, width);
    ex = 1.0 - 4.0/std::pow(width, 2.0) * std::pow(val-loc, 2.0);
    QCOMPARE(rs, ex);
}

void tst_uncertainties::test_CODeMOperations_perturbation()
{
    // perturbs the vector f by radius 0.1 on the direction simplex
    // and prints out the result

    TVector<double> f;
    f << 1.0 << 2.0 << 3.0;
    cout << endl << "f = [" << f[0] << ";" << f[1] << ";" << f[2] <<
            "]; fPert = [[]";

    TVector<double> pertF = directionPerturbation(f, 0.1);
    cout << ", [" << pertF[0] << ';'
         << pertF[1] << ';' << pertF[2] << ']' << endl;
}

void tst_uncertainties::test_CODeMDistribution_ctor()
{
    // test that the class can be constructed

    TVector<double> f;
    f << 1.2 << 0.2 << 0.8;

    LinearDistribution* l = new LinearDistribution(1.5, 10.0);

    TVector<double> ideal(3, -10.0);
    TVector<double> antiIdeal(3, 20.0);
    double pNorm = 3.0;
    double dirPertMaxR = 0.05;
    double lb = 0.5;
    double ub = 0.7;

    CODeMDistribution cd(l, f, lb, ub, ideal, antiIdeal, dirPertMaxR, pNorm);
}

void tst_uncertainties::test_CODeMDistribution_sample()
{
    // Initialise a distribution and sample it. Print out the result

    TVector<double> f;
    f << -1.2 << 0.2 << 1.3;


    TVector<double> ideal(3, -2.0);
    TVector<double> antiIdeal(3, 2.5);
    double pNorm = 3.0;
    double dirPertMaxR = 0.05;
    double lb = 0.5;
    double ub = 0.7;

    TVector<double>  fNorm(f);
    normaliseToUnitBoxInMemory(fNorm, ideal, antiIdeal);
    double mag = magnitudeAndDirectionP(fNorm);
    double prox = directedBoxedIntervalLength(fNorm);

    LinearDistribution* l = new LinearDistribution(mag/prox, 0.9);
    //    LinearDistribution* l = new LinearDistribution(0.1, 0.9);
    l->defineAscend(false);

    CODeMDistribution cd(l, f, lb, ub, ideal, antiIdeal, dirPertMaxR, pNorm);

    cout << endl << "f = [" << f[0] << ";" << f[1] << ";" << f[2] <<
            "];" << endl << "fPert = [[]";

    TVector<double> pertF = cd.sampleDistribution();
    cout << ", [" << pertF[0] << ';'
         << pertF[1] << ';' << pertF[2] << ']' << endl;
}

void tst_uncertainties::test_CODeM_pseudoProblem()
{
    // A test that demonstrates how to use the various tools in the toolbox to
    // create stochastic problems.
    // This example assumes there is a deterministic problem, and the objective
    // values are artificially defined.

    // First solution
    TVector<double> inputsA(5);
    TVector<double> outputsA(3);
    for(int i=0; i<inputsA.size(); i++) {
        inputsA[i] = (double)i;
    }
    for(int i=0; i<outputsA.size(); i++) {
        outputsA[i] = 2.0 * i;
    }

    // Second solution
    TVector<double> inputsB(5);
    TVector<double> outputsB(3);
    for(int i=0; i<inputsB.size(); i++) {
        inputsB[i] = 10.0 - 4.0 * (i + 1.0);
    }
    for(int i=0; i<outputsB.size(); i++) {
        outputsB[i] = 5.0 - i;
    }

    // Define decision and objective space
    TVector<IElement> lb(5, IElement(RealType, 0.0));
    TVector<IElement> ub(5, IElement(RealType, 10.0));
    BoxConstraintsDataSPtr box = BoxConstraintsDataSPtr(new BoxConstraintsData);
    box->defineLowerBounds(lb);
    box->defineUpperBounds(ub);

    TVector<double> ideal(3, -10.0);
    TVector<double> antiIdeal(3, 10.0);

    TVector<TVector<double>> outputs;
    outputs << outputsA << outputsB;

    // Create UncertaintyKernels:
    UncertaintyKernel* kernA = new UncertaintyKernel(inputsA, outputsA, box,
                                                     ideal, antiIdeal);
    UncertaintyKernel* kernB = new UncertaintyKernel(inputsB, outputsB, box,
                                                     ideal, antiIdeal);

    TVector<UncertaintyKernel*> kerns;
    kerns << kernA << kernB;

    /// Define parametric distributions for each IMapping
    /// 0.9 * Uniform(prox, skewed(prox, 5.0))
    /// 0.1 * Peak(prox+0.1, symmetry)
    /// dirPertRad = 0.1*LoV(oComp(2), 0.45, 0.3))

    double uniLB, uniLoc, uniUB, peakTend, peakLoc, dirPertRad;

    for(int i=0; i<2; i++) {
        uniLB  = kerns[i]->remoteness();
        uniLoc = skewedIncrease(kerns[i]->remoteness(), 5.0);
        uniUB  = uniLB + uniLoc * (1.0-uniLB);

        peakTend = kerns[i]->remoteness()+0.1;
        peakLoc  = kerns[i]->symmetry();

        dirPertRad = 0.1*lowOnValue(kerns[i]->oComponent(2), 0.45, 0.3);

        MergedDistribution* d = new MergedDistribution;
        d->appendDistribution(new UniformDistribution(uniLB, uniUB), 0.9);
        d->appendDistribution(new PeakDistribution(peakTend, peakLoc), 0.1);

        double lb = 0;
        double ub = 1;
        CODeMDistribution cd(d, outputs[i], lb, ub, ideal, antiIdeal, dirPertRad, 2.0);

        TVector<double> pertF = cd.sampleDistribution();
    }

    clearPointerVector(kerns);
}

void tst_uncertainties::test_varianceConfidenceIntervals()
{
    TVector<double> samps;
    samps << 1.1 << 1.3 << 0.95 << 1.21 << 1.01 << 0.8;
    int N = samps.size();
    SampledDistribution sd(samps);
    boost::math::chi_squared cs(N-1);

    double mean = sd.mean();
    double std  = sd.std();
    double var  = sd.variance();

    double confidence = 0.95;
    double alpha = 1.0 - confidence;

    double stdUpperBound = std * std::sqrt((N-1) /
                                      boost::math::quantile(cs, alpha/2.0));
    double stdLowerBound = std * std::sqrt((N-1) /
                                      boost::math::quantile(cs, 1.0 - alpha/2.0));

    double varUpperBound = var * ((N-1) /
                                 boost::math::quantile(cs, alpha/2.0));
    double varLowerBound = var * ((N-1) /
                                 boost::math::quantile(cs, 1.0 - alpha/2.0));

    cout << "mean = " << mean << ". std = " << std << ". variance = " << var << endl;
    cout << "Confidence interval for the std is (" << stdLowerBound <<
            " , " << stdUpperBound << ")." << endl;
    cout << "Confidence interval for the variance is (" << varLowerBound <<
            " , " << varUpperBound << ")." << endl;
    cout << "Confidence interval for the variance is (" <<
            std::pow(stdLowerBound, 2) << " , " << std::pow(stdUpperBound, 2) <<
            ")." << endl;
}

void tst_uncertainties::test_RobustnessIndicators()
{
    double cVal = 0.95;
    IDistribution* dist = new UniformDistribution(0.0, 100.0);
    ConfidenceIndicator* conf = new ConfidenceIndicator(cVal);
    ThresholdIndicator* thresh = new ThresholdIndicator(90.0);

    double rs = conf->evaluate(dist);
    QCOMPARE(rs, cVal*100);

    rs = thresh->evaluate(dist);
    QCOMPARE(rs, 0.9);

    delete dist;
    dist = new NormalDistribution(0.0, 1.0);

    cout << cVal*100 << "% confidence level is: " << conf->evaluate(dist) << endl;

    delete dist;
    delete conf;
    delete thresh;
}


void tst_uncertainties::test_GECCOExample_2obj()
{
    TRAND.defineSeed(2);

    int nObj = 2;
    int nVar = 6;
    int nPareto = 5;
    int nRand   = 10;
    int nSamp = 50;

    TVector<TVector<double> > paretoDeterministic;
    TVector<TVector<double> > randDeterministic;
    TVector<TVector<TVector<double> > > paretoObj;
    TVector<TVector<TVector<double> > > randObj;

    TVector<TVector<double> > paretoDirVars = simplexLatticeRecursive(nPareto-1, 2);

    // Pareto optimal vectors
    TVector<double> optDistVars(nVar - 1, 0.5);
    for(int i=0; i<paretoDirVars.size(); i++) {
        TVector<double> iVec(1, paretoDirVars[i][0]);
        iVec << optDistVars;
        TVector<double> determObjVec = deterministicOVec(7, iVec, nObj);
        paretoDeterministic.push_back(determObjVec);
        TVector<TVector<double> > oVecSamps = CODeM::GECCOExample(iVec, nObj, nSamp);
        paretoObj.push_back(oVecSamps);
    }

    // Random vectors
    for(int i=0; i<nRand; i++) {
        TVector<double> iVec;
        for(int j=0; j<nVar; j++) {
            iVec.push_back(TRAND.randUni());
        }
        TVector<double> determObjVec = deterministicOVec(7, iVec, nObj);
        randDeterministic.push_back(determObjVec);
        TVector<TVector<double> > oVecSamps = CODeM::GECCOExample(iVec, nObj, nSamp);
        randObj.push_back(oVecSamps);
    }

#ifdef SHOW_LOG
    // Display the results
    cout << "\n% Optimal vectors:" << endl;
    for(int v=0; v<paretoObj.size(); v++) {
        cout << "paretoObj{" << v+1 << "} = [";
        for(int i=0; i<paretoObj[v].size(); i++) {
            dispVector(paretoObj[v][i]);
        }
        cout << "];" << endl;
    }

    cout << "\n% Random vectors:" << endl;
    for(int v=0; v<randObj.size(); v++) {
        cout << "randObj{" << v+1 << "} = [";
        for(int i=0; i<randObj[v].size(); i++) {
            dispVector(randObj[v][i]);
        }
        cout << "];" << endl;
    }

    cout << "\n% Optimal deterministic vectors:" << endl;
    cout << "determOptimal = [";
    for(int i=0; i<paretoDeterministic.size(); i++) {
        dispVector(paretoDeterministic[i]);
    }
    cout << "];" << endl;

    cout << "\n% Random deterministic vectors:" << endl;
    cout << "determRand = [";
    for(int i=0; i<randDeterministic.size(); i++) {
        dispVector(randDeterministic[i]);
    }
    cout << "];\n" << endl;
#endif
}

void tst_uncertainties::test_GECCOExample_3obj()
{
    TRAND.defineSeed(2);

    int nObj = 3;
    int nVar = 7;
    int nPareto = 3;
    int nRand   = 10;
    int nSamp = 50;

    TVector<TVector<double> > randDeterministic;
    TVector<TVector<double> > paretoDeterministic = simplexLatticeRecursive(nPareto-1, 3, 0.5);
    TVector<TVector<TVector<double> > > paretoObj;
    TVector<TVector<TVector<double> > > randObj;

    // Pareto optimal vectors
    for(int i=0; i<paretoDeterministic.size(); i++) {
        TVector<TVector<double> > oVecSamps =
                CODeM::GECCOExamplePerturb(nVar, paretoDeterministic[i], nSamp);
        paretoObj.push_back(oVecSamps);
    }

    TRAND.defineSeed(2);

    // Random vectors
    for(int i=0; i<nRand; i++) {
        TVector<double> iVec;
        for(int j=0; j<nVar; j++) {
            iVec.push_back(TRAND.randUni());
        }
        TVector<double> determObjVec = deterministicOVec(7, iVec, nObj);
        randDeterministic.push_back(determObjVec);
        TVector<TVector<double> > oVecSamps = CODeM::GECCOExample(iVec, nObj, nSamp);
        randObj.push_back(oVecSamps);
    }
#ifdef SHOW_LOG
    // Display the results
    cout << "\n% Optimal vectors:" << endl;
    for(int v=0; v<paretoObj.size(); v++) {
        cout << "paretoObj{" << v+1 << "} = [";
        for(int i=0; i<paretoObj[v].size(); i++) {
            dispVector(paretoObj[v][i]);
        }
        cout << "];" << endl;
    }

    cout << "\n% Random vectors:" << endl;
    for(int v=0; v<randObj.size(); v++) {
        cout << "randObj{" << v+1 << "} = [";
        for(int i=0; i<randObj[v].size(); i++) {
            dispVector(randObj[v][i]);
        }
        cout << "];" << endl;
    }


    cout << "\n% Optimal deterministic vectors:" << endl;
    cout << "determOptimal = [";
    for(int i=0; i<paretoDeterministic.size(); i++) {
        dispVector(paretoDeterministic[i]);
    }
    cout << "];" << endl;

    cout << "\n% Random deterministic vectors:" << endl;
    cout << "determRand = [";
    for(int i=0; i<randDeterministic.size(); i++) {
        dispVector(randDeterministic[i]);
    }
    cout << "];\n" << endl;
#endif
}

void tst_uncertainties::test_parametricDistributionFactory()
{
    // Already implemented
    {
        TVector<double> parameters(2);
        DistributionType type = GenericDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters;
        parameters << 0.5 << 20;
        DistributionType  type = UniformDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist != nullptr, true);
        for(int i=0; i<dist->parameters().size(); i++) {
            QCOMPARE(dist->parameters()[i], parameters[i]);
        }
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters;
        parameters << 0.5 << 0.35;
        DistributionType  type = NormalDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist != nullptr, true);
        for(int i=0; i<dist->parameters().size(); i++) {
            QCOMPARE(dist->parameters()[i], parameters[i]);
        }
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters;
        parameters << 0.2 << 0.85 << 0.5;
        DistributionType  type = LinearDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist != nullptr, true);
        for(int i=0; i<dist->parameters().size(); i++) {
            QCOMPARE(dist->parameters()[i], parameters[i]);
        }
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters;
        parameters << 0.2 << 0.3 << 0.5 << 2.0;
        DistributionType  type = PeakDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist != nullptr, true);
        for(int i=0; i<dist->parameters().size(); i++) {
            QCOMPARE(dist->parameters()[i], parameters[i]);
        }
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    // Not implemeted
    {
        TVector<double> parameters(2);
        DistributionType  type = BetaDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = GammaDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = BinomialDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = ChiSquaredDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = ExponentialDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = PoissonDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = StudentTDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = CauchyDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = BernoulliDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

    {
        TVector<double> parameters(2);
        DistributionType  type = DiscreteUniformDistType;
        IDistribution* dist = DistributionFactory::instance()
                ->createParametricDist(type, parameters);
        QCOMPARE(dist == nullptr, true);
        std::cout << DistributionFactory::instance()->parameaterNames(type) << std::endl;
        delete dist;
    }

}

void tst_uncertainties::test_bug_in_sample_distribution()
{
    SampledDistribution* dist = new SampledDistribution();
    dist->addSamples(TVector<double>({1, 20, 45}));
    TRAND.defineSeed(1);
    int N = 10000;
    int counter = 0;
    for(int i=0; i<N; i++) {
        double res = dist->sample();
        if(res < 0 || res > 45) {
            ++counter;
            cout <<"FAIL at : "<< i << " res: " << res << endl;
        }
    }
    cout << "In total, failed " << counter << ", " << 100.0*counter/N << "%" << endl;
    QCOMPARE(counter, 0);
}

QTEST_GUILESS_MAIN(tst_uncertainties)

#include "tst_uncertainties.moc"


