/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef VALIDATIONWITHSCALARISATION_H
#define VALIDATIONWITHSCALARISATION_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Evaluators/Validation.h>

namespace Tigon {
namespace Operators {
using namespace Tigon::Representation;

class LIGER_TIGON_EXPORT ValidationWithScalarisation : public Validation
{
    HANDLE_READ_PROPERTIES_BEGIN(Validation)
    READ(IsGeneralisedDecomposition, TP_isGeneralisedDecomposition)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IEvaluator)
    WRITE(IsGeneralisedDecomposition, bool, TP_defineIsGeneralisedDecomposition)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::ValidationWithScalarisation)

public:
    ValidationWithScalarisation();
    ValidationWithScalarisation(Tigon::Representation::IPSet *ipset);
    ~ValidationWithScalarisation();

    // Properties
    void TP_defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType TP_scalarisingFunction() const;

    void TP_defineIsGeneralisedDecomposition(bool b=false);
    bool TP_isGeneralisedDecomposition() const;

    void TP_defineWeightScopeApproach(Tigon::WeightScope s);
    Tigon::WeightScope TP_weightScopeApproach() const;

    double pNorm() const;

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

private:
    void  initialise();

    Tigon::ScalarisationType m_scalarisingFunc;
    double                   m_pNorm;
    bool                     m_isGeneralisedDecomposition;
    Tigon::WeightScope       m_weightScopeApproach;
    TVector<double>          m_currentDirVec;
    bool                     m_isNewDirVec;
};

} // namespace Operators
} // namespace Tigon
#endif // VALIDATIONWITHSCALARISATION_H
