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
#include <tigon/Algorithms/IAlgorithm.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Operators/IOperator.h>

using namespace Tigon::Operators;

namespace Tigon {
namespace Algorithms {

IAlgorithm::IAlgorithm()
{

}

IAlgorithm::IAlgorithm(Tigon::Representation::IPSet* ipset)
    : IPSetDecorator(ipset)
{

}

IAlgorithm::~IAlgorithm()
{
    clearOperators();
}

void IAlgorithm::evaluateNode()
{
    for (IOperator* op : m_operators) {
        op->evaluateOnlyThisNode();
    }
}

// Algorithm specific actions
TVector<IOperator*> IAlgorithm::operators() const
{
    return m_operators;
}

void IAlgorithm::appendOperator(IOperator* op)
{
    m_operators.push_back(op);
}

void IAlgorithm::clearOperators()
{
    clearPointerVector(m_operators);
}

} // namespace Algorithms
} // namespace Tigon

