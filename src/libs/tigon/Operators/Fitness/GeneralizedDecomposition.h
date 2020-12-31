/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef GENERALIZEDDECOMPOSITION_H
#define GENERALIZEDDECOMPOSITION_H
#include <tigon/Operators/Fitness/Scalarization.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT GeneralizedDecomposition : public Scalarization
{
    DECLARE_CLASS(Tigon::Operators::GeneralizedDecomposition)

public:
    GeneralizedDecomposition();
    GeneralizedDecomposition(Tigon::Representation::IPSet* ipset);
    ~GeneralizedDecomposition();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
private:
    void initialise();
};

} // namespace Operators
} // namespace Tigon

#endif // GENERALIZEDDECOMPOSITION_H
