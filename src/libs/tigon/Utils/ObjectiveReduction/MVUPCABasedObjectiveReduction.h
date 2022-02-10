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
#ifndef MVUPCABASEDOBJECTIVEREDUCTION_H
#define MVUPCABASEDOBJECTIVEREDUCTION_H

#include <tigon/Utils/ObjectiveReduction/PCABasedObjectiveReduction.h>

namespace Tigon {

class LIGER_TIGON_EXPORT MVUPCABasedObjectiveReduction : public PCABasedObjectiveReduction
{
public:
    MVUPCABasedObjectiveReduction();
    ~MVUPCABasedObjectiveReduction();

    void defineStandardize(bool standardize);
    bool getStandardize() const;

private:
    void runObjectiveReductionAlgorithm();

    TMatrixReal learnKernel(int k, bool standardize) const;
    TVector<TVector<int>> kNearestNeighbours(
            const TVector<TVector<double>>& matrix, int k) const;
    TVector<TVector<int>> kNearestNeighbours(
            const TMatrixReal& matrix, int k) const;
    TVector<double> constraintsRightHandSide(const TVector<TVector<int>>& a,
                                             const TMatrixReal& G, int k) const;
    TVector<TVector<int>> constraintsLeftHandSide(
            const TVector<TVector<int>>& a, int k) const;

    TMatrixReal callCSDP(const TVector<double>& c,
                         const TVector<TVector<int>>& At,
                         const TVector<double>& b_in, int k) const;

    bool m_standardize;

};

} // namespace Tigon

#endif // MVUPCABASEDOBJECTIVEREDUCTION_H
