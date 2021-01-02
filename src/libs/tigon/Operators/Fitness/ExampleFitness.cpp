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
#include <tigon/Operators/Fitness/ExampleFitness.h>

#include <QString>
#include <QStringList>

namespace Tigon {
namespace Operators {

ExampleFitness::ExampleFitness()
{

}

ExampleFitness::ExampleFitness(Tigon::Representation::IPSet *ipset)
    : IFitness(ipset)
{

}

ExampleFitness::~ExampleFitness()
{

}

void ExampleFitness::evaluateNode()
{

}

QString ExampleFitness::name()
{
    return QString("Example Fitness Assignment");
}

QString ExampleFitness::description()
{
    return QString("This is an example implementation of a fitness assignment operator.");
}

QStringList ExampleFitness::propertyNames()
{
    return QStringList();
}

QStringList ExampleFitness::propertyDescriptions()
{
    return QStringList();
}

} // namespace Operators
} // namespace Tigon
