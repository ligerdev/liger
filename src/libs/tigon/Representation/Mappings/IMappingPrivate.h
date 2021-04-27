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
#ifndef IMAPPINGPRIVATE_H
#define IMAPPINGPRIVATE_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT IMappingPrivate
{
public:
    explicit IMappingPrivate();
    ~IMappingPrivate();

    TVector<IElementSPtr>  m_dvec;
    TVector<IElementSPtr>  m_pvec;
    TVector<IElementSPtr>  m_ovec;
    TVector<IElementSPtr>  m_cvec;
    TVector<IElementSPtr>  m_uvec;

    TVector<double>        m_wvec;

    IElementSPtr           m_cost;
    bool                   m_isOptMap;
    bool                   m_isScalarised;
    ValidationStatus       m_isValidated;
    bool                   m_isSuccessfulEval;
    TVector<bool>          m_isFuncEvaluated;
    TVector<bool>          m_isObjEvaluated;
    TVector<bool>          m_isConstEvaluated;
    TVector<bool>          m_isUusedEvaluated;

    int                    m_runNumber;
};

} // namespace Representation
} // namespace Tigon

#endif // IMAPPINGPRIVATE_H
