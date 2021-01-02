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
#include <QFile>

#include <iostream>
#include <iomanip>

#include <tigon/Tigon.h>

#include <boost/math/constants/constants.hpp>
using namespace boost::math::constants;

#define TST_FANN_SOLUTIONS

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;

class tst_fann : public QObject
{
    Q_OBJECT

public:

private slots:
#ifdef TST_FANN_SOLUTIONS
    void test_language_classification();
#endif

};

void dispVector(TVector<double> vec, string sep = "\t", string endLine = "\n")
{
    for(unsigned int i=0; i<vec.size(); i++) {
        cout << vec[i];
        if(i<vec.size()-1) {
            cout << sep;
        }
    }
    cout << endLine;
}

TVector<double> generate_frequencies(TString filename)
{
    TString file_str = ":/language_classification/"+filename;
    QFile rfile(file_str.data());
    if(!rfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return TVector<double>();
    }

    int num_characters = 0;
    TVector<int> letter_count(26,0);
    char c;
    while(!rfile.atEnd()) {
        QByteArray line = rfile.readLine();
        for (int i = 0; i < line.size(); ++i) {
            c = tolower(line.at(i));
            if(c >= 'a' && c <= 'z') {
                letter_count[c - 'a']++;
                num_characters++;
            }

        }
    }
    rfile.close();

    TVector<double> frequencies;
    for(auto letter: letter_count) {
        frequencies << letter/(double)num_characters;
    }
    return frequencies;
}

#ifdef TST_FANN_SOLUTIONS
void tst_fann::test_language_classification()
{
    /// 1. Calculate character frequency from text files

    // training dataset size
    int nTrainingEnglish    = 3;
    int nTrainingFrench     = 3;
    int nTrainingPortuguese = 3;

    // load english training data
    TVector<TVector<double>> freq;
    TVector<TVector<double>> classif;

    // load english training data
    for(int i=0; i<nTrainingEnglish; i++) {
        TString rfile = "english_training_" + std::to_string(i+1) + ".txt";
        freq << generate_frequencies(rfile);
        classif << TVector<double>{1.0,-1.0,-1.0};
    }

    // load french training data
    for(int i=0; i<nTrainingFrench; i++) {
        TString rfile = "french_training_" + std::to_string(i+1) + ".txt";
        freq << generate_frequencies(rfile);
        classif << TVector<double>{-1.0,1.0,-1.0};
    }

    // load portuguese training data
    for(int i=0; i<nTrainingPortuguese; i++) {
        TString rfile = "portuguese_training_" + std::to_string(i+1) + ".txt";
        freq << generate_frequencies(rfile);
        classif << TVector<double>{-1.0,-1.0,1.0};
    }


    /// 2. Create the neural network

    FannNetwork* aNN = new FannNetwork();
    aNN->defineNumInputs(26);
    aNN->defineNumOutputs(3);
    aNN->defineNumHiddenLayers(1);
    aNN->defineSizeHiddenLayers(13);
    aNN->defineMaxEpochs(200);
    aNN->defineOutputFunction(FannNetwork::ActivationFunction::SIGMOID_SYMETRIC);
    aNN->defineHiddenFunction(FannNetwork::ActivationFunction::SIGMOID_SYMETRIC);

    //aNN->defineInputBounds(TVector<double>(3,-1.0), TVector<double>(3,1.0));
    //aNN->defineOutputBounds(TVector<double>(3,-1.0), TVector<double>(3,1.0));


    /// 3. train the neural network

    aNN->trainSurrogate(freq,classif);


    /// 4. Classify the test data

    int nTestEnglish    = 2;
    int nTestFrench     = 2;
    int nTestPortuguese = 2;

    for(int i=0; i<nTestEnglish; i++) {
        TString rfile = "english_test_" + std::to_string(i+1) + ".txt";
        TVector<double> ofreq = generate_frequencies(rfile);
        TVector<double> oclassif = aNN->evaluate(ofreq);
        QCOMPARE(bool(oclassif[0]>oclassif[1]),true);
        QCOMPARE(bool(oclassif[0]>oclassif[2]),true);
        dispVector(oclassif);
    }

    for(int i=0; i<nTestFrench; i++) {
        TString rfile = "french_test_" + std::to_string(i+1) + ".txt";
        TVector<double> ofreq = generate_frequencies(rfile);
        TVector<double> oclassif = aNN->evaluate(ofreq);
        QCOMPARE(bool(oclassif[1]>oclassif[0]),true);
        QCOMPARE(bool(oclassif[1]>oclassif[2]),true);
        dispVector(oclassif);
    }

    for(int i=0; i<nTestPortuguese; i++) {
        TString rfile = "portuguese_test_" + std::to_string(i+1) + ".txt";
        TVector<double> ofreq = generate_frequencies(rfile);
        TVector<double> oclassif = aNN->evaluate(ofreq);
        QCOMPARE(bool(oclassif[2]>oclassif[0]),true);
        QCOMPARE(bool(oclassif[2]>oclassif[1]),true);
        dispVector(oclassif);
    }
}

#endif

QTEST_GUILESS_MAIN(tst_fann)

#include "tst_fann.moc"
