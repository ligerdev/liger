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
#include <tigon/Operators/Evaluators/IEvaluator.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

IEvaluator::IEvaluator()
{
    initialise();
}

IEvaluator::IEvaluator(Representation::IPSet *ipset)
    : IOperator(ipset)
{
    initialise();
}

IEvaluator::~IEvaluator()
{

}

bool IEvaluator::TP_isParallel() const
{
    return m_parallelEvaluations;
}

void IEvaluator::TP_defineParallel(bool p)
{
    m_parallelEvaluations = p;
}

bool IEvaluator::TP_isCountEvaluations() const
{
    return m_countEvaluations;
}

void IEvaluator::TP_defineCountEvaluations(bool c)
{
    m_countEvaluations = c;
}

bool IEvaluator::TP_isCountPerFunction() const
{
    return m_countPerFunction;
}

void IEvaluator::TP_defineCountPerFunction(bool c)
{
    m_countPerFunction = c;
}

bool IEvaluator::TP_isSingleObjective() const
{
    return m_singleObjective;
}

void IEvaluator::TP_defineSingleObjective(bool s)
{
    m_singleObjective = s;
    if(s) {
        addAdditionalOutputTag(Tigon::TFitness);
    } else {
        removeAdditionalOutputTag(Tigon::TFitness);
    }
}

void IEvaluator::initialise()
{
    addProperty("ParallelEvaluation"
                , TString("Every IMapping is evaluated in a "
                          "separate thread.")
                , getTType(bool));
    addProperty("CountEvaluations"
                , TString("The evaluation counter is incremented "
                          "after the evaluation of each solution.")
                , getTType(bool));
    addProperty("CountPerFunction"
                , TString("Every function increments the "
                          "evaluation counter by one.")
                , getTType(bool));
    addProperty("SingleObjective"
                , TString("The fitness is set to the first "
                          "objective after evaluation.")
                , getTType(bool));

    TP_defineParallel(false);
    TP_defineCountEvaluations(true);
    TP_defineCountPerFunction(false);
    TP_defineSingleObjective(false);
    addOutputTag(Tigon::TForEvaluation);
}

} // namespace Operators
} // namespace Tigon
