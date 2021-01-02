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
#ifndef OBJECTIVEREDUCTION_H
#define OBJECTIVEREDUCTION_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/CorrelationMatrix.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

DISABLE_WARNINGS
#include <eigen/Eigenvalues>
ENABLE_WARNINGS

using Tigon::Representation::ISet;
using Tigon::Representation::IMappingSPtr;

namespace Tigon {

class LIGER_TIGON_EXPORT ObjectiveReduction
{
public:
    ObjectiveReduction();
    ObjectiveReduction(ISet* set);
    ObjectiveReduction(const TVector<IMappingSPtr>& sols);
    ~ObjectiveReduction();

    TVector<int> essentialSet() const;
    TVector<int> redundantSet() const;
    TVector<double> error() const;
    TMap<int, TVector<int>> correlationsBetweenObjectives() const;

private:

    void framework(const TMatrixReal& objectives);


    // error measure
    TVector<double> error_measure(const TMap<int, TVector<int> > &corrSubsets);

    // further reduces the objective set based on the correlated subsets
    TVector<bool> finalReductionRCM(const TMap<int, TVector<int>>& corrSubsets,
                                    const TVector<int> &subSet,
                                    int Nv);

    // identifies correlated subsets of objectives
    TMap<int, TVector<int>> correlatedSubsets(const TMatrixReal& corr,
                                              const TVector<int> &subSet,
                                              double Tcor);

    // performs the first objective reduction by analysing the eigenvalues and
    // eigenvectors
    TVector<int> objectiveReductionByEigenAnalysis(int Nv);

    // determines the number of significant principal components with a
    // cumulative variance that account for at least theta
    int significantPCs(double theta);

    // eigen-decomposes the matrix (mx), provided as input, and updates the
    // eigenvalues (m_lambda) and corresponding eivenvectors (m_V)
    void eigenDecomposition(const TMatrixReal& mx);

    // correlation matrix of the objective vectors from the population of
    // solutions provided as input
    CorrelationMatrix* m_corObject;

    // normalised eigenvalues, sorted in descending order
    TVector<double> m_lambda;

    // Matrix of eigenvectors, where each row is an eigenvector
    TVector<TVector<double>> m_V;

    // set of essential objectives for describing the POF
    TVector<int> m_essentialSet;

    // Set of redundant objectives
    TVector<int> m_redundantSet;

    // error in eliminating objectives
    TVector<double> m_error;

    // correlated sets of objectives
    TMap<int, TVector<int>> m_corrSubsets;

    // number of objectives
    int m_M;
};

} // namespace Tigon

#endif // OBJECTIVEREDUCTION_H
