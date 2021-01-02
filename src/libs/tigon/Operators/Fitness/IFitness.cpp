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
#include <tigon/Operators/Fitness/IFitness.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

IFitness::IFitness()
{
    initialise();
}

IFitness::IFitness(Representation::IPSet *ipset)
    : IOperator(ipset)
{
    initialise();
}

IFitness::~IFitness()
{

}

void IFitness::initialise()
{
    addOutputTag(Tigon::TForEvaluation);
    addAdditionalOutputTag(Tigon::TFitness);
}

} // namespace Operators
} // namespace Tigon
