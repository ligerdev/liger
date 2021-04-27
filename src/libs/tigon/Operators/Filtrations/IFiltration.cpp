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
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/Representation/PSets/IPSet.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

IFiltration::IFiltration()
{
    initialise();
}

IFiltration::IFiltration(Tigon::Representation::IPSet* ipset)
    : IOperator(ipset)
{
    initialise();
}

IFiltration::~IFiltration()
{

}

void IFiltration::initialise()
{

}

} // namespace Operators
} // namespace Tigon
