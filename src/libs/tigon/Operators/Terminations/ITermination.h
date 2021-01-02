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
#pragma once
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {
class LIGER_TIGON_EXPORT ITermination : public IOperator
{
    HANDLE_READ_PROPERTIES_BEGIN(IOperator)
    READ(maxIter, TP_maxIteration)
    READ(budget, TP_budget)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IOperator)
    WRITE(maxIter, int, TP_defineMaxIteration)
    WRITE(budget, int, TP_defineBudget)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::ITermination)

public:
    ITermination();
    ITermination(Tigon::Representation::IPSet* ipset);
    ~ITermination();

    void TP_defineMaxIteration(int iter);
    void TP_defineBudget(int budget);
    int  TP_maxIteration() const;
    int  TP_budget() const;

    void resetBudget();
    void resetMaxIter();

    // Information about the node.
    TString  name();
    TString  description();

    virtual void evaluateNode();

protected:
    void initialise();
};

} // Operators
} // Tigon
