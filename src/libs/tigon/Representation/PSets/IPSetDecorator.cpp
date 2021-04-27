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
#include <tigon/Representation/PSets/IPSetDecorator.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Representation {

IPSetDecorator::IPSetDecorator()
    : m_ipset(nullptr)
{
}

IPSetDecorator::IPSetDecorator(Tigon::Representation::IPSet* ipset)
    : m_ipset(ipset)
{
    syncConfiguration();
}

IPSetDecorator::~IPSetDecorator()
{

}

void IPSetDecorator::syncConfiguration()
{
    // Load the state of the previous node
    // to the current node.
    setFromIPSet(m_ipset);
}

void IPSetDecorator::evaluate()
{
    if(errorState() == Tigon::NoError) {
        m_ipset->evaluate();
        updateSets();

        try {
            evaluateNode();
        } catch(const TException &e) {
            defineErrorState(UndefinedError);
            throw;
        } catch(...) {
            throw;
        }
    }
}

void IPSetDecorator::evaluateOnlyThisNode()
{
    if(errorState() == Tigon::NoError) {
        updateSets();

        try {
            evaluateNode();
        } catch(const TException &e) {
            defineErrorState(UndefinedError);
            throw;
        } catch(...) {
            throw;
        }
    }
}

} // namespace Representation
} // namespace Tigon
