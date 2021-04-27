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
#include <tigon/Operators/Convergence/IConvergence.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Operators/IOperator.h>
#include <tigon/Representation/Sets/ISet.h>

namespace Tigon {
namespace Operators {

IConvergence::IConvergence()
{
    initialise();
}

IConvergence::IConvergence(Tigon::Representation::IPSet* ipset)
    : IOperator(ipset)
{
    initialise();
}

IConvergence::~IConvergence()
{

}

double IConvergence::convergenceValue() const {
    return m_convIndicatorValue;
}

void IConvergence::TP_defineOperateOnFinal(bool o)
{
    m_operateOnFinal = o;
}

bool IConvergence::TP_operateOnFinal() const
{
    return m_operateOnFinal;
}

void IConvergence::initialise()
{
    addProperty("OperateOnFinal"
                , TString("Disable the operator during the optimization "
                                  "process, and only operates after the "
                                  "termination criteria are satisfied.")
                , getTType(bool));
    addInputTag(Tigon::TForConvergence);
    TP_defineOperateOnFinal(false);

    m_convIndicatorValue = 0.0;
}

} // namespace Operators
} // namespace Tigon
