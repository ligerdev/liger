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
#include <tigon/Operators/ObjectiveReduction/IObjectiveReduction.h>

namespace Tigon {
namespace Operators {

IObjectiveReduction::IObjectiveReduction()
{
    initialise();
}

IObjectiveReduction::IObjectiveReduction(Tigon::Representation::IPSet* ipset)
    : IOperator(ipset)
{
    initialise();
}

IObjectiveReduction::~IObjectiveReduction()
{
    delete m_objData;
}

void IObjectiveReduction::initialise()
{
    addProperty("OperateOnFinal"
                , TString("Disable the operator during the optimization "
                                  "process, and only operates after the "
                                  "termination criteria are satisfied.")
                , getTType(bool));
    addInputTag(Tigon::TMainOptimizationSet);
    TP_defineOperateOnFinal(false);
    m_objData = new ObjectiveReductionData();
}

void IObjectiveReduction::TP_defineOperateOnFinal(bool o)
{
    m_operateOnFinal = o;
}

bool IObjectiveReduction::TP_operateOnFinal() const
{
    return m_operateOnFinal;
}

ObjectiveReductionData* IObjectiveReduction::objectiveReductionData() const
{
    return m_objData;
}


} // namespace Operators
} // namespace Tigon
