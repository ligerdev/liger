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
#ifndef POLYNOMIALMUTATION_H
#define POLYNOMIALMUTATION_H
#include <tigon/Operators/Perturbations/IPerturbation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT PolynomialMutation : public IPerturbation
{
    HANDLE_READ_PROPERTIES_BEGIN(IPerturbation)
    READ(MutationDistributionIndex, TP_mutationDistributionIndex)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IPerturbation)
    WRITE(MutationDistributionIndex, double, TP_defineMutationDistributionIndex)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::PolynomialMutation)

public:
    PolynomialMutation();
    PolynomialMutation(Tigon::Representation::IPSet* ipset);
    virtual ~PolynomialMutation();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    /**************/
    /* Properties */
    /**************/

    // Distribution index
    double TP_mutationDistributionIndex()                 const;
    void  TP_defineMutationDistributionIndex(double        eta);

private:
    void initialise();

    double   m_eta;
};

} // namespace Operators
} // namespace Tigon

#endif // POLYNOMIALMUTATION_H
