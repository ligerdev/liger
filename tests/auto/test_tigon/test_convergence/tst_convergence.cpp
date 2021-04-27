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

using namespace std;
using namespace Tigon;
using namespace Tigon::Representation;
using namespace Tigon::Operators;
using namespace Tigon::Algorithms;

class tst_convergence : public QObject
{
    Q_OBJECT

public:

private slots:        
    void test_ScalarisingSpaceUnitTest();
    void test_ScalarisingSpaceWithNeighboursUnitTest();
    void test_ScalarisingSpaceMonteCarloUnitTest();
};

void tst_convergence::test_ScalarisingSpaceUnitTest()
{    
    TVector<TVector<double>> myObjVec = {{0.2011,1.8}, {1.082,0.5025},
                                        {0.9043,0.8695}, {0.8546,1.443},
                                        {1.253,0.6318}, {1.326,0.364},
                                        {0.4243,1.545}, {0.5146,1.618}};

    TVector<double> ideal = {0.2011, 0.5025};
    TVector<double> antiIdeal = {1.082, 1.8};
    TVector<TVector<double> > refSet = {{0, 1}, {0.33, 0.67}, {0.67, 0.33}};

    /// Calculate convergence metric value
    int oVecSize = 2;
    int nDoE = 4;
    int count = nDoE;
    int index = 0;
    int iter = (myObjVec.size()-nDoE)/2+1;
    TVector<double> sumCost;
    sumCost.reserve(iter);
    TVector<double> actualSumCost = {0.1978, 0.1978, 0.1762};

    do {
        TVector<TVector<double> > part_myObjVec(count, TVector<double>(oVecSize));
        // Obtain obj only for this piece
        for(int i=0; i<count; i++) {
            for(int j=0; j<oVecSize; j++) {
                part_myObjVec[i][j] = myObjVec[i][j];
            }
        }

        sumCost.push_back(scalarisingSpace(part_myObjVec, refSet, ideal, antiIdeal));
        sumCost[index] = round(sumCost[index]*10000) / 10000;       
        QCOMPARE(sumCost[index] == actualSumCost[index], true);


        count += 2;
        if (count > myObjVec.size()) {
            break;
        }
        index += 1;
    } while(true);
    QCOMPARE(index == (myObjVec.size()-nDoE)/2, true);
    QCOMPARE(count == (myObjVec.size()+2), true);
}

void tst_convergence::test_ScalarisingSpaceWithNeighboursUnitTest()
{
    TVector<TVector<double>> myObjVec = {{1.282,0.5025}, {1.214,0.3783},
                                        {1.060,0.1325}, {1.154,0.1829},
                                        {1.420,0.5925}, {1.454,0.5829}};

    TVector<TVector<double>> myDecVec = {{0.2769,0.1927}, {0.1923,0.2714},
                                        {0.2170,0.3832}, {0.2212,0.1159},
                                        {0.2270,0.3532}, {0.2312,0.1759}};

    ///Normalise the goal vector
    TVector<double> ideal = {1.214, 0.3783};
    TVector<double> antiIdeal = {1.282, 0.5025};
    TVector<TVector<double> > refSet = {{0,1}, {0.33, 0.67}, {0.67, 0.33}};

    /// Use reference directions in calculating real cost & robust cost
    int oVecSize = 2;
    int dVecSize = 2;
    int nDoE = 2;
    int count = nDoE;
    int index = 0;
    TVector<TVector<double > > myBound{{0.0, 1.0},{0.0, 1.0}};
    double neighbourhoodRadius = 0.2;
    double maxDist = neighbourhoodRadius * qSqrt(dVecSize);
    double confidenceLevel = 0.9;
    int iter = (myObjVec.size()-nDoE)/2+1;
    TVector<double> sumCost;
    sumCost.reserve(iter);
    TVector<double> actualSumCost = {0.9967, 0.8248, 1.0713};

    do {
        TVector<TVector<double> > part_myObjVec(count, TVector<double>(oVecSize));
        // Obtain obj only for this piece
        for(int i=0; i<count; i++) {
            for(int j=0; j<oVecSize; j++) {
                part_myObjVec[i][j] = myObjVec[i][j];
            }
        }

        TVector<TVector<double> > part_myDecVec(count, TVector<double>(dVecSize));
        // Obtain obj only for this piece
        for(int i=0; i<count; i++) {
            for(int j=0; j<oVecSize; j++) {
                part_myDecVec[i][j] = myDecVec[i][j];
            }
        }

        sumCost.push_back(scalarisingSpaceRobust(part_myObjVec, part_myDecVec,
                                                 refSet, ideal, antiIdeal,
                                                 myBound, maxDist,
                                                 confidenceLevel));
        sumCost[index] = round(sumCost[index]*10000) / 10000;
        QCOMPARE(sumCost[index] == actualSumCost[index], true);

        count += 2;
        if (count > myObjVec.size()) {
            break;
        }
        index += 1;

    } while(true);
    QCOMPARE(index == (myObjVec.size()-nDoE)/2, true);
    QCOMPARE(count == myObjVec.size()+2, true);
}

void tst_convergence::test_ScalarisingSpaceMonteCarloUnitTest()
{
    // 2 sols, each sol has 2 obj var, each sol has 2 samples
    TVector<TVector<TVector<double> > > myObjVec = {{{1.2, 1.25}, {0.53, 0.5}},
                                                   {{1.4, 1.36}, {0.62, 0.66}}};

    ///Normalise the goal vector
    TVector<double> ideal = {1.2, 0.5};
    TVector<double> antiIdeal = {1.4, 0.66};
    TVector<TVector<double> > refSet = {{0,1}, {1,0}};
    double confidenceLevel = 0.9;

    /// Use reference directions in calculating robust cost
    double sumCost = scalarisingSpaceRandom(myObjVec, refSet, ideal,
                                            antiIdeal, confidenceLevel);
    sumCost = round(sumCost*10000) / 10000;
    QCOMPARE(sumCost == 0.3076, true);
}

QTEST_GUILESS_MAIN(tst_convergence)

#include "tst_convergence.moc"


