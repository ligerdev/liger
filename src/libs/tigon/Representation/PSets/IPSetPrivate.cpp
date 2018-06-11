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
#include <tigon/Representation/PSets/IPSetPrivate.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Log/LogManager.h>

namespace Tigon {
namespace Representation {

IPSetPrivate::IPSetPrivate()
    : m_problem(new Problem)
    , m_maxIter(0)
    , m_currentIter(0)
    , m_budget(0)
    , m_usedBudget(0)
    , m_keepArchive(false)
    , m_errorState(NoError)
    , m_log(new Log::LogManager)
{

}

IPSetPrivate::~IPSetPrivate()
{
    if(!(m_sets.empty())) {
        for(int i=0; i<m_sets.size(); i++) {
            delete m_sets[i];
        }
        m_sets.clear();
    }

    m_tags.clear();

    m_problem.reset();

    m_log.reset();
}

TSharedPointer<IPSetPrivate> IPSetPrivate::clone()
{
    TSharedPointer<IPSetPrivate> ret = TSharedPointer<IPSetPrivate>(new IPSetPrivate());
    ret->m_problem.reset();
    ret->m_problem      = m_problem->clone();

    ret->m_maxIter      = m_maxIter;
    ret->m_currentIter  = m_currentIter;
    ret->m_budget       = m_budget;
    ret->m_usedBudget   = m_usedBudget;

    ret->m_keepArchive  = m_keepArchive;

    ret->m_errorState   = m_errorState;

    ret->m_direction    = m_direction;
    ret->m_refSet       = m_refSet;

    ret->m_multiPack    = m_multiPack;

    for(int i=0; i<m_sets.size(); i++) {
        ret->m_sets.push_back(m_sets.at(i)->clone());
    }
    ret->m_tags         = m_tags;

    ret->m_log.reset();
    ret->m_log          = m_log->clone();

    return ret;
}

} // namespace Representation
} // namespace Tigon
