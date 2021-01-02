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
#ifndef SCALARIZATION_H
#define SCALARIZATION_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT Scalarization : public IFitness
{
    DECLARE_CLASS(Tigon::Operators::Scalarization)

public:
    Scalarization();
    Scalarization(Tigon::Representation::IPSet* ipset);
    ~Scalarization();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    // Properties
    void  TP_defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType TP_scalarisingFunction() const;

    void TP_defineWeightScopeApproach(Tigon::WeightScope s);
    Tigon::WeightScope TP_weightScopeApproach() const;

    double pNorm() const;

private:
    void initialise();

    Tigon::ScalarisationType m_scalarisingFunc;
    double                   m_pNorm;
    Tigon::WeightScope       m_weightScopeApproach;
};

} // namespace Operators
} // namespace Tigon

#endif // SCALARIZATION_H
