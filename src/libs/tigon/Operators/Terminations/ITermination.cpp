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
#include <tigon/Operators/Terminations/ITermination.h>
#include <tigon/tigonengineregistry.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Operators {
ITermination::ITermination()
{
    initialise();
}

ITermination::ITermination(Representation::IPSet *ipset)
    : IOperator(ipset)
{
    initialise();
}

ITermination::~ITermination()
{

}

void ITermination::TP_defineMaxIteration(int iter)
{
    if(iter>0) {
        IPSet::defineMaxIteration(iter);
    }
}

void ITermination::TP_defineBudget(int budget)
{
    if(budget>0) {
        IPSet::defineBudget(budget);
    }
}

int ITermination::TP_maxIteration() const
{
    return IPSet::maxIteration();
}

int ITermination::TP_budget() const
{
    return IPSet::budget();
}

void ITermination::resetBudget()
{
    IPSet::defineBudget(0);
}

void ITermination::resetMaxIter()
{
    IPSet::defineMaxIteration(0);
}

TString ITermination::name()
{
    return TString("End Node");
}

TString ITermination::description()
{
    return TString("");
}

void ITermination::evaluateNode()
{

}

void ITermination::initialise()
{
    addProperty("maxIter"
                , TString("Max number of iterations")
                , typeid(int).hash_code());

    addProperty("budget"
                , TString("Maximum number of function evaluations")
                , typeid(int).hash_code());
}

} // namespace Operators
} // namepsace Tigon

REGISTER_IPSET_FACTORY(Operators, ITermination)

