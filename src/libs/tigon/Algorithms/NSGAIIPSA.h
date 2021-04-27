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
#ifndef NSGAIIPSA_H
#define NSGAIIPSA_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Operators {
class IOperator;
}
}

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT NSGAIIPSA : public IAlgorithm
{
    DECLARE_CLASS(Tigon::Algorithms::NSGAIIPSA)

public:
    NSGAIIPSA();
    NSGAIIPSA(Tigon::Representation::IPSet* ipset);
    ~NSGAIIPSA();

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();
};

} // namespace Algorithms
} // namespace Tigon

#endif // NSGAIIPSA_H
