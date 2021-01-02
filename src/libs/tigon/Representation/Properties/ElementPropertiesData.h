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
#ifndef ELEMENTPROPERTIESDATA_H
#define ELEMENTPROPERTIESDATA_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ElementPropertiesData
{
public:
    ElementPropertiesData();
    ElementPropertiesData(const ElementPropertiesData& data)
        : m_elemIdx(data.m_elemIdx),
          m_elemID(data.m_elemID),
          m_elemName(data.m_elemName),
          m_elemDescription(data.m_elemDescription),
          m_elemUnit(data.m_elemUnit),
          m_elemType(data.m_elemType),
          m_optimizationType(data.m_optimizationType)
    {}
    ~ElementPropertiesData();

    int     m_elemIdx;
    TString m_elemID;
    TString m_elemName;
    TString m_elemDescription;
    TString m_elemUnit;
    ElementType m_elemType;
    OptimizationType m_optimizationType;
};


} // namespace Representation
} // namespace Tigon

#endif // ELEMENTPROPERTIESDATA_H
