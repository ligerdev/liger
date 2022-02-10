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
#ifndef PCABASEDOBJECTIVEREDUCTION_H
#define PCABASEDOBJECTIVEREDUCTION_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/CorrelationMatrix.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

#include <eigen/Eigenvalues>

using Tigon::Representation::ISet;

namespace Tigon {

class LIGER_TIGON_EXPORT PCABasedObjectiveReduction
{
public:
    PCABasedObjectiveReduction();
    virtual ~PCABasedObjectiveReduction();

    void updateData(ISet* data);
    void updateData(const TVector<TVector<double>>& data);
    void updateData(const TMatrixReal& data);

    int numberObjectives() const;
    TVector<int> essentialSet() const;
    TVector<int> redundantSet() const;
    TVector<double> error() const;
    TVector<double> variance() const;
    TMap<int, TVector<int>> correlatedObjectiveSubSets() const;

    CorrelationMatrix* correlationMatrix() const;
    TVector<double> eigenValues() const;
    TVector<TVector<double>> eigenVectors() const;

protected:
    void virtual runObjectiveReductionAlgorithm();
    void algorithmCommonCode();

    void defineCorrelatedObjectiveSubSets(const TMap<int, TVector<int>>& corr);
    void defineEssentialSet(const TVector<int>& set);
    void defineRedundantSet(const TVector<int>& set);
    void defineError(const TVector<double>& err);
    void defineVariance(const TVector<double>& var);
    void defineEigenValues(const TVector<double>& lambda);
    void defineEigenVectors(const TVector<TVector<double>>& V);

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

private:
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

    // variance accounted by each objective
    TVector<double> m_variance;

    // correlated sets of objectives
    TMap<int, TVector<int>> m_corrSubsets;

    // number of objectives
    int m_M;
};

} // namespace Tigon

#endif // PCABASEDOBJECTIVEREDUCTION_H
