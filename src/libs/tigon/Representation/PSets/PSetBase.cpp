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
#include <tigon/Representation/PSets/PSetBase.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Representation/PSets/IPSetPrivate.h>

namespace Tigon {
namespace Representation {

PSetBase::PSetBase()
    : IPSet()
{

}

PSetBase::PSetBase(const PSetBase& pset)
    : IPSet()
{
    T_UNUSED(pset)
}

PSetBase* PSetBase::clone()
{
    PSetBase* ret = new PSetBase();
    TSharedPointer<IPSetPrivate> nd = this->getInternal()->clone();
    ret->setInternal(nd);
    return ret;
}

} // namespace Representation
} // namespace Tigon

