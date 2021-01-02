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
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IPMQPAlgorithm.h>
#include <tigon/Operators/Initialisation/IPMQPInitialisation.h>
#include <tigon/Operators/Evaluators/IPMQPEvaluator.h>
#include <tigon/Operators/Directions/IPMQPDirection.h>

namespace Tigon {
namespace Algorithms {

using namespace Tigon::Operators;

IPMQPAlgorithm::IPMQPAlgorithm()
{

}

IPMQPAlgorithm::IPMQPAlgorithm(Representation::IPSet* ipset)
    : IAlgorithm(ipset)
{
    IPMQPInitialisation* initialisation = new IPMQPInitialisation(ipset);
    IPMQPEvaluator*      evaluation     = new IPMQPEvaluator(initialisation);
    IPMQPDirection*      direction      = new IPMQPDirection(evaluation);

    appendOperator(initialisation);
    appendOperator(evaluation);
    appendOperator(direction);

    m_finalOperator = direction;
}

IPMQPAlgorithm::~IPMQPAlgorithm()
{

}

void IPMQPAlgorithm::evaluateNode()
{
    m_finalOperator->evaluate();
}

QString IPMQPAlgorithm::name()
{
    return QString("IPMQPAlgorithm");
}

QString IPMQPAlgorithm::description()
{
    return QString("Interior Point Solver for Quadratic programming problem.\n"
                   "Accepted Problem formulation:\n"
                   "min 0.5 x'Qx + c'x \n"
                   "s.t. Ax = b, Hx <= d.\n"
                   "Predictor-Corrector is applied to calculate the Newton step.");
}

QStringList IPMQPAlgorithm::propertyNames()
{
    return QStringList();
}

QStringList IPMQPAlgorithm::propertyDescriptions()
{
    return QStringList();
}
} // namespace Algorithms
} // namespace Tigon

