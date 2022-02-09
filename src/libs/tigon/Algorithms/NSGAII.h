/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef NSGAII_H
#define NSGAII_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT NSGAII : public IAlgorithm
{
    DECLARE_CLASS(Tigon::Algorithms::NSGAII)

public:
    NSGAII();
    NSGAII(Tigon::Representation::IPSet* ipset);
    ~NSGAII();

    // Information about the node.
    TString     name();
    TString     description();

protected:
    void initialise();
};

} // namespace Algorithms
} // namespace Tigon

#endif // NSGAII_H
