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
#ifndef IPSETPRIVATE_H
#define IPSETPRIVATE_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class ISet;

class IPSetPrivate
{
public:
    explicit IPSetPrivate();
    ~IPSetPrivate();
    TSharedPointer<IPSetPrivate> clone();

    ProblemSPtr                     m_problem;

    int                             m_maxIter;
    int                             m_currentIter;
    int                             m_budget;
    int                             m_usedBudget;
    bool                            m_keepArchive;
    TErrorState                     m_errorState;

    TMap<TString, TVector<double> > m_multiPack;
    TMap<TString, TVector<ISet*> >  m_tags;
    TVector<ISet*>                  m_sets;
    TVector<double>                 m_direction;
    TVector<TVector<double> >       m_refSet;

    Log::LogManagerSPtr             m_log;
};

} // namespace Representation
} // namespace Tigon

#endif // IPSETPRIVATE_H
