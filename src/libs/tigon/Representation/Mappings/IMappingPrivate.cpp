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
#include <tigon/Representation/Mappings/IMappingPrivate.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Representation {

IMappingPrivate::IMappingPrivate()
    : m_cost(new IElement(RealType))
    , m_isOptMap        (false)
    , m_isScalarised    (false)
    , m_isValidated     (NotValidated)
    , m_isSuccessfulEval(true)
    , m_runNumber(1)
{

}

IMappingPrivate::~IMappingPrivate()
{
    if(!m_dvec.empty()) {
        clearVector(m_dvec);
    }
    if(!m_pvec.empty()) {
        clearVector(m_pvec);
    }
    if(!m_ovec.empty()) {
        clearVector(m_ovec);
    }
    if(!m_cvec.empty()) {
        clearVector(m_cvec);
    }
    if(!m_uvec.empty()) {
        clearVector(m_uvec);
    }
    m_cost.reset();
}

} // namespace Representation
} // namepsace Tigon
