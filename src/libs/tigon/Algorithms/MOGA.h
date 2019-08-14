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
#ifndef MOGA_H
#define MOGA_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT MOGA : public IAlgorithm
{
    DECLARE_CLASS(Tigon::Algorithms::MOGA)

public:
    MOGA();
    MOGA(Tigon::Representation::IPSet* ipset);
    ~MOGA();

    // Information about the node.
    TString     name();
    TString     description();

protected:
    void initialise();
};

} // namespace Algorithms
} // namespace Tigon


#endif // MOGA_H
