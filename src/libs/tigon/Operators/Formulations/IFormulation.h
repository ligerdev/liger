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
#ifndef IFORMULATION_H
#define IFORMULATION_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IFormulation : public IOperator
{
    DECLARE_CLASS(Tigon::Operators::IFormulation)

public:
    IFormulation();
    IFormulation(Tigon::Representation::IPSet* ipset);
    virtual ~IFormulation();

    virtual void initialise();

    virtual void evaluateNode();

    // Problem properties

    // Information about the node.
    TString     name();
    TString     description();

    void        defineProblem(ProblemSPtr    prob);

    // TODO: consider removing this functionality
    void        appendFunction(IFunctionSPtr func);
};

} // namespace Operators
} // namespace Tigon

#endif // IFORMULATION_H
