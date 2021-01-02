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
#include <tigon/Utils/CorrelationMatrix.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {

CorrelationMatrix::CorrelationMatrix()
{
    // default values
    m_x << 0.3, 0.4, 0.8,
           0.1, 0.7, 0.3;
    calculateCorrelationMatrix();
}

CorrelationMatrix::CorrelationMatrix(const TVector<TVector<double>>& x)
{
    // input is MxN
    insertData(x);
}

CorrelationMatrix::CorrelationMatrix(const TMatrixReal &x)
{
    // input is MxN
    insertData(x);
}

CorrelationMatrix::~CorrelationMatrix()
{
    m_x.resize(0,0);
    m_corrMatrix.resize(0,0);
}

void CorrelationMatrix::calculateCorrelationMatrix()
{
    int N=m_x.cols();
    double inverse = 1.0 / double(N-1);

    // 1) mean centre the matrix
    TMatrixReal centered = m_x.colwise() - m_x.rowwise().mean();

    // 2) determine the inverse of the standard deviation
    TCVectorReal variance = inverse*centered.rowwise().squaredNorm();
    TCVectorReal istd     = variance.cwiseSqrt().cwiseInverse();

    // 3) normalise the matrix by using the standard deviation
    TMatrixReal centered_std = istd.asDiagonal() * centered;

    // 4) the correlation is given by R = (1/N-1) XX'
    m_corrMatrix = inverse*(centered_std * centered_std.transpose());

    // 5) Ensures that each feature is perfectly correlated with itself
    for(int i=0; i<m_x.rows(); i++) {
        m_corrMatrix(i,i)=1.0;
    }
}

TMatrixReal CorrelationMatrix::inputData() const
{
    return m_x;
}

TMatrixReal CorrelationMatrix::correlationMatrix() const
{
    return m_corrMatrix;
}

int CorrelationMatrix::nSamples() const
{
    // the data matrix is MxN
    // where N is the number of samples
    if(m_x.cols()>0) {
        return m_x.cols();
    }
    else {
        return 0;
    }
}

void CorrelationMatrix::insertData(const TVector<TVector<double>>& x)
{
    // input is MxN
    int M=x.size();
    int N=x[0].size();

    if(M<2 || N<1) {
        return;
    }

    m_x.resize(M,N);
    for(int i=0; i<M; i++) {
        for(int j=0; j<N; j++) {
            m_x(i,j) = x[i].at(j);
        }
    }

    calculateCorrelationMatrix();
}

void CorrelationMatrix::insertData(const TMatrixReal& x)
{
    // input is MxN
    int M=x.rows();
    int N=x.cols();

    if(M<2 || N<1) {
        return;
    }

    m_x = x;

    calculateCorrelationMatrix();
}

} // namespace Tigon
