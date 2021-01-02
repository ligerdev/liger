/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef DACE_H
#define DACE_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/Kriging/IKriging.h>

namespace Tigon {

class LIGER_TIGON_EXPORT DACE : public IKriging
{
public:
    DACE();
    ~DACE();

    void updateData(const TVector<TVector<double>>& x,
                    const TVector<double>& y,
                    const TVector<double>& theta = TVector<double>());

    double interpolate(const TVector<double>& xstar);
    double errorEstimate();

    TVector<TVector<double> > inputData()  const;
    TVector<double>           outputData() const;

    // Properties (read and write)
    void defineBudgetPerVariable(int n);
    int  budgetPerVariable()      const;

    void defineInitialPopsizePerVariable(int n);
    int  initialPopsizePerVariable()      const;

    void defineStallIterations(int iter);
    int  stallIterations()         const;

    // read-only properties
    TVector<double> theta() const;
    bool isDecompositionSuccessfull() const;

private:
    void searchTheta();
    void configureDACE();
    void updateGramMatrix();

    int  m_xDim; // number of dimensions
    int  m_N;    // number of points

    TVector<TVector<double>> m_x; // x-data
    TVector<double>          m_y; // y-data
    TVector<double>      m_theta; // parameter for Kriging radial basis function

    // Determined by Kriging
    TMatrixReal m_U;
    TMatrixReal m_Ut;
    TMatrixReal m_U_solve_Ut_solve_residual;
    double m_mu;
    double m_sigmaSqr;
    double m_predictionErrorEstimate;
    TMatrixReal m_Gram; // Gram matrix
    bool m_isDecompositionSuccessfull;

    // ACROMUSE parameters
    int m_budgetPerVar;
    int m_initialPopsizePerVariable;
    int m_stallIterations;
};

} // namespace Tigon

#endif // DACE_H
