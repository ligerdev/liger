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
#include <tigon/Representation/Functions/CODeM/CODeMProblems.h>
#include <tigon/Representation/Functions/CODeM/UncertaintyKernel.h>
#include <tigon/Representation/Functions/CODeM/CODeMOperators.h>
#include <tigon/Representation/Functions/CODeM/CODeMDistribution.h>
#include <tigon/Representation/Distributions/MergedDistribution.h>
#include <tigon/Representation/Distributions/UniformDistribution.h>
#include <tigon/Representation/Distributions/PeakDistribution.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/NormalisationUtils.h>
#include <tigon/Representation/Functions/WFG/wfg_toolkit/ExampleProblems.h>
#include <tigon/Representation/Functions/DTLZ/DTLZProblems.h>

using namespace Tigon;
using namespace WFGT::Toolkit::Examples::Problems;

namespace CODeM {

TVector<double> GECCOExample(TVector<double> iVec, int nObj)
{
    // Evaluate the decision vector
    TVector<double> oVec = DTLZ::DTLZ1Modified(iVec, nObj);

    return GECCOExamplePerturb(iVec, oVec)[0];
}

TVector<TVector<double>> GECCOExample(TVector<double> iVec, int nObj, int nSamp)
{
    // Evaluate the decision vector
    TVector<double> oVec = DTLZ::DTLZ1Modified(iVec, nObj);

    return GECCOExamplePerturb(iVec, oVec, nSamp);
}

TVector<TVector<double>> GECCOExamplePerturb(int iVecSize, TVector<double> oVec,
                                             int nSamp)
{
    // Set the uncertainty kernel
    TVector<double> ideal(oVec.size(), 0.0);

    double maxVal = 1.125 * (iVecSize - oVec.size() + 1) + 0.5;
    TVector<double> antiIdeal(oVec.size(), maxVal);

    TVector<double> normVec(oVec);
    magnitudeAndDirectionP(normVec, 2.0);
    double sFactor = magnitudeAndDirectionP(normVec, 1);

    double ub = 1.0 / sFactor;
    double lb = 0.5 / maxVal / sFactor;

    UncertaintyKernel uk(oVec, lb, ub, ideal, antiIdeal);

    // Evaluate the uncertainty parameters
    double uniLB, uniUB, uniLoc, dirPertRad;

    uniLB  = uk.remoteness();
    uniLoc = 0.9 + 0.1 * lowOnValue(uk.remoteness(), 0.0, 1.0);
    uniUB  = 1.0 -  uniLoc * (1.0-uniLB);

    double distanceNorm = 1.0;

    dirPertRad = 0.02 + 0.1 * linearDecrease(uk.symmetry());

    // Create the CODeM distribution
    UniformDistribution* d = new UniformDistribution(uniLB, uniUB);

    CODeMDistribution cd(d, oVec, lb, ub, ideal, antiIdeal, dirPertRad, distanceNorm);

    // Sample the distribution
    TVector<TVector<double> > samples;
    for(int i=0; i<nSamp; i++) {
        samples.push_back(cd.sampleDistribution());
    }
    return samples;
}

TVector<TVector<double>> GECCOExamplePerturb(TVector<double> iVec,
                                             TVector<double> oVec, int nSamp)
{
    return GECCOExamplePerturb(iVec.size(), oVec, nSamp);
}

TVector<double> deterministicOVec(int prob, TVector<double> iVec, int nObj, int k)
{
    TVector<double> oVec;

    switch(prob) {
    case 1: case 2: case 3:
        oVec = WFG4(iVec, k, nObj);
        break;

    case 4:
        oVec = WFG6(iVec, k, nObj);
        break;

    case 5:
        oVec = WFG8(iVec, k, nObj);
        break;

    case 6: case 7:
        oVec = DTLZ::DTLZ1Modified(iVec, nObj);
        break;
    default:
        break;
    }

    return oVec;
}



TVector<double> EMO2019P1(TVector<double> iVec, int k, int nObj)
{
    // Evaluate the decision vector
    TVector<double> oVec = WFG4Tunable(iVec, k, nObj, 5, 10.0, 3, 1);

    return EMO2019P1Perturb(oVec)[0];
}

TVector<TVector<double> > EMO2019P1(TVector<double> iVec,
                                int k, int nObj, int nSamp)
{
    // Evaluate the decision vector
    TVector<double> oVec = WFG4Tunable(iVec, k, nObj, 5, 10.0, 3, 1);

    return EMO2019P1Perturb(oVec, nSamp);
}

TVector<TVector<double> > EMO2019P1Perturb(TVector<double> oVec, int nSamp)
{
    // Set the uncertainty kernel
    TVector<double> ideal(oVec.size(), 0.0);
    TVector<double> antiIdeal(oVec.size());
    for(int i=0; i<oVec.size(); i++) {
        antiIdeal[i] = (double) 3.0*(i+1);
    }
    double lb = 2.0/3.0;
    double ub = 1.0;

    UncertaintyKernel uk(oVec, lb, ub, ideal, antiIdeal);

    // Evaluate the uncertainty parameters
    double uniLB, uniUB, dirPertRad;

    uniLB = uk.remoteness();
    uniUB = uniLB;

    double distanceNorm = 2.0;

    dirPertRad =0.0;

    // Create the CODeM distribution
    UniformDistribution* d = new UniformDistribution(uniLB, uniUB);

    CODeMDistribution cd(d, oVec, lb, ub, ideal, antiIdeal, dirPertRad,
                         distanceNorm);

    // Sample the distribution
    TVector<TVector<double> > samples;
    for(int i=0; i<nSamp; i++) {
        samples.push_back(cd.sampleDistribution());
    }
    return samples;
}




TVector<double> EMO2019P2(TVector<double> iVec, int k, int nObj)
{
    // Evaluate the decision vector
    TVector<double> oVec = WFG4Tunable(iVec, k, nObj, 0, 8.0, 0, 2);

    return EMO2019P2Perturb(oVec)[0];
}

TVector<TVector<double> > EMO2019P2(TVector<double> iVec,
                                int k, int nObj, int nSamp)
{
    // Evaluate the decision vector
    TVector<double> oVec = WFG4Tunable(iVec, k, nObj, 0, 8.0, 0, 2);

    return EMO2019P2Perturb(oVec, nSamp);
}

TVector<TVector<double> > EMO2019P2Perturb(TVector<double> oVec, int nSamp)
{
    // Set the uncertainty kernel
    TVector<double> ideal(oVec.size(), 0.0);
    TVector<double> antiIdeal(oVec.size());
    for(int i=0; i<oVec.size(); i++) {
        antiIdeal[i] = (double) 3.0*(i+1);
    }
    double lb = 2.0/3.0;
    double ub = 1.0;

    UncertaintyKernel uk(oVec, lb, ub, ideal, antiIdeal);

    // Evaluate the uncertainty parameters
    double uniLB, uniLoc, uniUB, dirPertRad;

    uniLB = uk.remoteness();
    uniLoc = skewedIncrease(uk.remoteness(), 0.4);
    uniUB  = 1.0 - uniLoc * (1.0-uniLB);

    double distanceNorm = 2.0;

    dirPertRad = 0.0;

    // Create the CODeM distribution
    UniformDistribution* d = new UniformDistribution(uniLB, uniUB);

    CODeMDistribution cd(d, oVec, lb, ub, ideal, antiIdeal, dirPertRad,
                         distanceNorm);

    // Sample the distribution
    TVector<TVector<double> > samples;
    for(int i=0; i<nSamp; i++) {
        samples.push_back(cd.sampleDistribution());
    }
    return samples;
}



} // namespace CODeM
