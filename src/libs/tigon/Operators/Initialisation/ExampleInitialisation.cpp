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
#include <tigon/Operators/Initialisation/ExampleInitialisation.h>

namespace Tigon {
namespace Operators {

ExampleInitialisation::ExampleInitialisation()
{

}

ExampleInitialisation::ExampleInitialisation(Tigon::Representation::IPSet *ipset)
    : IInitialisation(ipset)
{

}

ExampleInitialisation::~ExampleInitialisation()
{

}

void ExampleInitialisation::evaluateNode()
{

}

QString ExampleInitialisation::name()
{
    return QString("Example Initialisation");
}

QString ExampleInitialisation::description()
{
    return QString("This is an example implementation of an initialisation scheme.");
}

QStringList ExampleInitialisation::propertyNames()
{
    return QStringList();
}

QStringList ExampleInitialisation::propertyDescriptions()
{
    return QStringList();
}

} // namespace Operators
} // namespace Tigon
