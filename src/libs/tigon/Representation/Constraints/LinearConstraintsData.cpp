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
#include <tigon/Representation/Constraints/LinearConstraintsData.h>

namespace Tigon {
namespace Representation {

LinearConstraintsData::LinearConstraintsData()
{
    m_nvar = 0;
}

LinearConstraintsData::LinearConstraintsData(const LinearConstraintsData& data)
{
    m_nvar      = data.nVar();
    m_rules     = data.linearConstraintRules();
    m_constants = data.linearConstraintConstants();
}

LinearConstraintsData::~LinearConstraintsData()
{

}

LinearConstraintsData* LinearConstraintsData::clone()
{
    return (new LinearConstraintsData(*this));
}

void LinearConstraintsData::setNVar(int nvar)
{
    m_nvar = nvar;
}

void LinearConstraintsData::addLinearConstraint(TMap<int, double> rule,
                                                double constant)
{
    m_rules.push_back(rule);
    m_constants.push_back(constant);
}

int LinearConstraintsData::nVar() const
{
    return m_nvar;
}

TMatrixReal LinearConstraintsData::linearConstraintsAMatrix()
{
    bool sOK = sizeConsistencyCheck();
    if(!sOK) {
//        throw Tigon::TigonException("Sizes are incosistent in LinearConstraintsData.");
    }
    generateAMatrix();
    return m_aMatrix;
}

TCVectorReal LinearConstraintsData::linearConstraintsBVector()
{
    bool sOK = sizeConsistencyCheck();
    if(!sOK) {
//        throw Tigon::TigonException("Sizes are inconsistent in LinearConstraintsData.");
    }
    generateBVector();
    return m_bVector;
}

TVector<TMap<int, double> > LinearConstraintsData::linearConstraintRules() const
{
    return m_rules;
}

TVector<double> LinearConstraintsData::linearConstraintConstants() const
{
    return m_constants;
}

void LinearConstraintsData::clearConstraints()
{
    m_rules.clear();
    m_constants.clear();
    m_nvar = 0;
    m_aMatrix.resize(1,1);
    m_bVector.resize(1,1);
}

void LinearConstraintsData::generateAMatrix()
{
    m_aMatrix.resize(m_rules.size(), m_nvar);
    m_aMatrix.setZero(m_rules.size(),m_nvar);
    for(int i=0; i<m_rules.size(); i++) {
        TMap<int, double> map = m_rules.at(i);
        for(TMap<int, double>::iterator itr = map.begin();
            itr != map.end(); ++itr) {
            m_aMatrix(i, itr->first) = itr->second;
        }
    }
}

void LinearConstraintsData::generateBVector()
{
    m_bVector.resize(m_constants.size(),1);
    m_bVector.setZero(m_constants.size(),1);
    for(int i=0; i<m_constants.size(); i++) {
        m_bVector(i) = m_constants.at(i);
    }
}

bool LinearConstraintsData::sizeConsistencyCheck() const
{
    bool ret = false;
    if(m_rules.size() == m_constants.size()) {
        ret = true;
    }
    return ret;
}

} // namespace Representation
} // namespace Tigon
