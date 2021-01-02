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
#include <tigon/Core/TObject.h>
#include <algorithm>

namespace Tigon {
TObject::TObject()
{

}

TString TObject::className() const
{
    ///\todo Remove leading number. return just the class name.
    return TString(typeid(*this).name());
}

TStringList TObject::propertyNames() const
{
    TStringList names;
    std::transform(m_propertiesInfo.begin(),
                   m_propertiesInfo.end(),
                   std::back_inserter(names),
                   [](const TPropertyInfo& infoItem) {return std::get<0>(infoItem);});

    return names;
}

TStringList TObject::propertyDescriptions() const
{
    TStringList descriptions;
    std::transform(m_propertiesInfo.begin(),
                   m_propertiesInfo.end(),
                   std::back_inserter(descriptions),
                   [](const TPropertyInfo& infoItem) {return std::get<1>(infoItem);});

    return descriptions;
}

TVector<TTypeHash> TObject::propertyTypes() const
{
    TVector<TTypeHash> types;
    std::transform(m_propertiesInfo.begin(),
                   m_propertiesInfo.end(),
                   std::back_inserter(types),
                   [](const TPropertyInfo& infoItem) {return std::get<2>(infoItem);});

    return types;
}

int TObject::propertyCount() const
{
    return static_cast<int>(m_propertiesInfo.size());
}

void TObject::addProperty(const TString &name, const TString &description,
                          TTypeHash type)
{
    m_propertiesInfo.push_back(std::make_tuple(name, description, type));
}
}
