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
#include <tigon/Operators/Perturbations/ExamplePerturbation.h>


namespace Tigon {
namespace Operators {

ExamplePerturbation::ExamplePerturbation()
{

}

ExamplePerturbation::ExamplePerturbation(Tigon::Representation::IPSet *ipset)
    : IPerturbation(ipset)
{

}

ExamplePerturbation::~ExamplePerturbation()
{

}

void ExamplePerturbation::evaluateNode()
{

}

QString ExamplePerturbation::name()
{
    return QString("Example Perturbation");
}

QString ExamplePerturbation::description()
{
    return QString("This is an example implementation of a perturbation.");
}

QStringList ExamplePerturbation::propertyNames()
{
    return QStringList();
}

QStringList ExamplePerturbation::propertyDescriptions()
{
    return QStringList();
}

} // namespace Operators
} // namespace Tigon
