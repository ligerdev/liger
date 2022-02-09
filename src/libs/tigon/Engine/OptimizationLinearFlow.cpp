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
#include <tigon/Engine/OptimizationLinearFlow.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/PSets/IPSetDecorator.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {

OptimizationLinearFlow::OptimizationLinearFlow()
    : m_currentIndex(-1)
{

}

OptimizationLinearFlow::~OptimizationLinearFlow()
{
    clearFlow();
}

void OptimizationLinearFlow::setCurrentIndex(int index)
{
    if(index < m_nodes.size() && index > -1) {
        m_currentIndex = index;
    }
}

Representation::IPSet* OptimizationLinearFlow::previousNode()
{
    if(m_nodes.size() == 0 || m_currentIndex == 0) {
        return 0;
    }
    return m_nodes[--m_currentIndex];
}

Representation::IPSet* OptimizationLinearFlow::currentNode() const
{
    if(m_currentIndex < m_nodes.size() && m_currentIndex > -1) {
        return m_nodes[m_currentIndex];
    } else {
        return 0;
    }
}

Representation::IPSet* OptimizationLinearFlow::nextNode()
{
    if(m_nodes.size() == 0 || m_currentIndex == m_nodes.size()-1) {
        return 0;
    }
    return m_nodes[++m_currentIndex];
}

Representation::IPSet* OptimizationLinearFlow::finalNode() const
{
    return m_nodes.back();
}

Representation::IPSet* OptimizationLinearFlow::node(int index) const
{
    if(index < m_nodes.size() && index > -1) {
        return m_nodes[index];
    } else {
        return 0;
    }
}

int OptimizationLinearFlow::currentIndex() const
{
    return m_currentIndex;
}

void OptimizationLinearFlow::evaluate()
{
    m_nodes.back()->evaluate();
}

void OptimizationLinearFlow::clearFlow()
{
    for(size_t i=0; i<m_nodes.size(); i++) {
        if(m_nodes[i]) {
            delete m_nodes[i];
        }
    }
    m_nodes.clear();
}

void OptimizationLinearFlow::incrementIteration()
{
    m_nodes.back()->incrementIteration();
}

void OptimizationLinearFlow::appendNode(Representation::IPSet* node)
{
    m_nodes.push_back(node);
    m_currentIndex = m_nodes.size()-1;
}

int OptimizationLinearFlow::size() const
{
    return m_nodes.size();
}

bool OptimizationLinearFlow::isTerminate() const
{
    return m_nodes.back()->isTerminate();
}

int OptimizationLinearFlow::currentIteration() const
{
    return m_nodes.back()->currentIteration();
}

int OptimizationLinearFlow::remainingIterations() const
{
    return m_nodes.back()->remainingIterations();
}

int OptimizationLinearFlow::maxIteration() const
{
    return m_nodes.back()->maxIteration();
}

int OptimizationLinearFlow::budget() const
{
    return m_nodes.back()->budget();
}

int OptimizationLinearFlow::usedBudget() const
{
    return m_nodes.back()->usedBudget();
}

int OptimizationLinearFlow::remainingBudget() const
{
    return m_nodes.back()->remainingBudget();
}

} // namespace Tigon

