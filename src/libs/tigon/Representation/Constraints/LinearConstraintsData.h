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
#ifndef LINEARCONSTRAINTSDATA_H
#define LINEARCONSTRAINTSDATA_H

#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class LinearConstraintsData;
}
}

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT LinearConstraintsData
{
public:
    LinearConstraintsData();
    LinearConstraintsData(const LinearConstraintsData& data);
    ~LinearConstraintsData();
    LinearConstraintsData* clone();

    void setNVar(int nvar);
    void addLinearConstraint(TMap<int, double> rule, double constant);
    int  nVar() const;
    TMatrixReal  linearConstraintsAMatrix();
    TCVectorReal linearConstraintsBVector();
    TVector<TMap<int, double> > linearConstraintRules() const;
    TVector<double>  linearConstraintConstants() const;
    void clearConstraints();

private:
    // The TMap stores: (variable number, multiplier)
    int m_nvar;
    TVector<TMap<int, double> > m_rules;
    TVector<double> m_constants;
    TMatrixReal  m_aMatrix;
    TCVectorReal m_bVector;
    void generateAMatrix();
    void generateBVector();
    bool sizeConsistencyCheck() const;
};

} // namespace Representation
} // namespace Tigon

#endif // LINEARCONSTRAINTSDATA_H
