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
#ifndef CORRELATIONMATRIX_H
#define CORRELATIONMATRIX_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {

class LIGER_TIGON_EXPORT CorrelationMatrix
{
public:
    CorrelationMatrix();
    CorrelationMatrix(const TVector<TVector<double>>& x);
    CorrelationMatrix(const TMatrixReal& x);
    ~CorrelationMatrix();

    void insertData(const TVector<TVector<double>>& x);
    void insertData(const TMatrixReal& x);

    TMatrixReal correlationMatrix() const;
    TMatrixReal inputData() const;

    int nSamples() const;

private:
    void calculateCorrelationMatrix();

    TMatrixReal m_corrMatrix;
    TMatrixReal m_x;
};

} // namespace Tigon

#endif // CORRELATIONMATRIX_H
