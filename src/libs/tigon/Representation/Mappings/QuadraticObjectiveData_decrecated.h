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
#ifndef QUADRATICOBJECTIVEDATA_H
#define QUADRATICOBJECTIVEDATA_H

#include <tigon/tigon_global.h>
#include <QVector>

namespace Tigon {
namespace Representation {
class QuadraticObjectiveData;
}
}

namespace Tigon {
namespace Representation {
/* =======================================
 * Problem formulation:
 * 0.5*x'Qx + c'x
 * Q: coefficients for the quadratic terms
 * c: coefficients for the linear terms
 *
 * If Q is empty, it represents a linear
 * function, i.e. c'x
 * =======================================
 */
class LIGER_TIGON_EXPORT QuadraticObjectiveData
{
public:
    QuadraticObjectiveData();
    QuadraticObjectiveData(const QuadraticObjectiveData& data);
    ~QuadraticObjectiveData();
    QuadraticObjectiveData* clone();

    void setNVar(int nvar);
    void defineQuadraticCoeff(QVector<QVector<qreal> > Q);
    void defineLinearCoeff(QVector<qreal> c);
    int  nVar() const;
    TMatrixReal  quadraticCoeffQMatrix();
    TCVectorReal linearCoeffCVector();
    QVector<QVector<qreal> > quadraticCoefficients() const;
    QVector<qreal> linearCoefficients() const;
    void clearObjectiveData();

private:
    int m_nvar;
    bool m_isQChanged;
    bool m_isCChanged;
    QVector<QVector<qreal> > m_Q;
    QVector<qreal>           m_c;
    TMatrixReal  m_qMatrix;
    TCVectorReal m_cVector;
    void generateQMatrix();
    void generateCVector();
    bool sizeConsistencyCheck() const;
};

} // namespace Representation
} // namespace Tigon

#endif // QUADRATICOBJECTIVEDATA_H
