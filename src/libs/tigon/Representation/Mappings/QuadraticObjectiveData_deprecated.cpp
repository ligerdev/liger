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
#include <tigon/Representation/Mappings/QuadraticObjectiveData.h>

namespace Tigon {
namespace Representation {

QuadraticObjectiveData::QuadraticObjectiveData()
{
    m_isQChanged = false;
    m_isCChanged = false;
    m_nvar = 1;
    QVector<qreal> tmpv;
    tmpv.append(0.0);
    m_Q.append(tmpv);
    m_c.append(0.0);
    generateQMatrix();
    generateCVector();
}

QuadraticObjectiveData::QuadraticObjectiveData(const QuadraticObjectiveData& data)
{
    m_isQChanged = false;
    m_isCChanged = false;
    m_nvar = data.nVar();
    m_Q = data.quadraticCoefficients();
    m_c = data.linearCoefficients();
    generateQMatrix();
    generateCVector();
}

QuadraticObjectiveData::~QuadraticObjectiveData()
{

}

QuadraticObjectiveData* QuadraticObjectiveData::clone()
{
    return (new QuadraticObjectiveData(*this));
}

void QuadraticObjectiveData::setNVar(int nvar)
{
    m_nvar = nvar;
}

void QuadraticObjectiveData::defineQuadraticCoeff(QVector<QVector<qreal> > Q)
{
    // check if m_Q is empty
    if(!m_Q.isEmpty()) {
        m_Q.clear();
    }
    m_Q = Q;
    m_isQChanged = true;
}

void QuadraticObjectiveData::defineLinearCoeff(QVector<qreal> c)
{
    // check if m_Q is empty
    if(!m_c.isEmpty()) {
        m_c.clear();
    }
    m_c = c;
    m_isCChanged = true;
}

int QuadraticObjectiveData::nVar() const
{
    return m_nvar;
}

TMatrixReal QuadraticObjectiveData::quadraticCoeffQMatrix()
{
    bool sOK = sizeConsistencyCheck();
    if(!sOK) {
        return TMatrixReal();
    }
    if(m_isQChanged) {
        generateQMatrix();
    }
    return m_qMatrix;
}

TCVectorReal QuadraticObjectiveData::linearCoeffCVector()
{
    bool sOK = sizeConsistencyCheck();
    if(!sOK) {
        return TCVectorReal();
    }
    if(m_isCChanged) {
        generateCVector();
    }
    return m_cVector;
}

QVector<QVector<qreal> > QuadraticObjectiveData::quadraticCoefficients() const
{
    return m_Q;
}

QVector<qreal> QuadraticObjectiveData::linearCoefficients() const
{
    return m_c;
}

void QuadraticObjectiveData::clearObjectiveData()
{
    m_Q.clear();
    m_c.clear();
    m_nvar = 0;
    m_qMatrix.resize(1,1);
    m_cVector.resize(1,1);
}

bool QuadraticObjectiveData::sizeConsistencyCheck() const
{
    // check Q size
    if(m_Q.size() != m_nvar) {
        return false;
    }
    for(int i = 0; i < m_Q.size(); i++) {
        if(m_Q[i].size() != m_nvar) {
            return false;
        }
    }
    // check c size
    if(m_c.size() != m_nvar) {
        return false;
    }
    return true;
}

void QuadraticObjectiveData::generateQMatrix()
{
    m_qMatrix.resize(m_nvar,m_nvar);
    m_qMatrix.setZero(m_nvar,m_nvar);
    for(int i = 0; i < m_nvar; i++) {
        for(int j = 0; j < m_nvar; j++) {
            m_qMatrix(i,j) = m_Q[i][j];
        }
    }
}

void QuadraticObjectiveData::generateCVector()
{
    m_cVector.resize(m_nvar);
    m_cVector.setZero(m_nvar);
    for(int i = 0; i < m_nvar; i++) {
        m_cVector(i) = m_c[i];
    }
}

} // namespace Representation
} // namespace Tigon
