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
#ifndef MOEAD_H
#define MOEAD_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT MOEAD : public IAlgorithm
{
    HANDLE_READ_PROPERTIES_BEGIN(IAlgorithm)
    //READ(RegularisationMethod, TP_regularisationMethod)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IAlgorithm)
    //WRITE(RegularisationMethod, Tigon::RegularisationApproach, TP_defineRegularisationMethod)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Algorithms::MOEAD)

public:
    MOEAD();
    MOEAD(Tigon::Representation::IPSet* ipset);
    ~MOEAD();

    void evaluateNode();

    void TP_defineRegularisationMethod(Tigon::RegularisationApproach r);
    Tigon::RegularisationApproach TP_regularisationMethod() const;

    Tigon::RegularisationApproach m_regularisationMethod;

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();
};

} // namespace Algorithms
} // namespace Tigon


#endif // MOEAD_H
