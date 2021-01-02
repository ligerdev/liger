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
#include <tigon/Representation/Properties/ProblemProperties.h>
#include <tigon/Representation/Properties/ProblemPropertiesData.h>
#include <tigon/Representation/Elements/IElement.h>

using namespace std;

namespace Tigon {
namespace Representation {

ProblemProperties::ProblemProperties()
    : d(new ProblemPropertiesData())
{

}

ProblemProperties::ProblemProperties(const ProblemProperties& el)
    : d(el.d)
{

}

ProblemProperties::ProblemProperties(TString name)
    : d(new ProblemPropertiesData())
{
    d->m_probName = name;
}

ProblemProperties::ProblemProperties(TString name, TString desc)
    : d(new ProblemPropertiesData())
{
    d->m_probName = name;
    d->m_probDescription = desc;
}

ProblemProperties::ProblemProperties(TString name, TString desc, ProblemType ptype)
    : d(new ProblemPropertiesData())
{
    d->m_probName = name;
    d->m_probDescription = desc;
    d->m_probType = ptype;
}

void ProblemProperties::setProblemID(TString id)
{
    d->m_probID = id;
}

ProblemProperties::~ProblemProperties()
{

}

void ProblemProperties::setType(ProblemType type)
{
    d->m_probType = type;
}

TString ProblemProperties::ID() const
{
    return d->m_probID;
}

TString ProblemProperties::name() const
{
    return d->m_probName;
}

TString ProblemProperties::description() const
{
    return d->m_probDescription;
}

ProblemType ProblemProperties::type() const
{
    return d->m_probType;
}

} // namespace Representation
} // namespace Tigon
