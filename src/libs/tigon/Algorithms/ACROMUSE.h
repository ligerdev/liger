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
#ifndef ACROMUSE_H
#define ACROMUSE_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Operators {
class IOperator;
}
}

using namespace Tigon::Representation;

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT ACROMUSE : public IAlgorithm
{
    DECLARE_CLASS(Tigon::Algorithms::ACROMUSE)

public:
    ACROMUSE();
    ACROMUSE(IPSet* ipset);
    virtual ~ACROMUSE();

    // Information about the node.
    TString     name();
    TString     description();
    
    IMappingSPtr bestMapping() const;

private:
    void initialise();
};

} // namespace Algorithms
} // namespace Tigon

#endif // ACROMUSE_H
