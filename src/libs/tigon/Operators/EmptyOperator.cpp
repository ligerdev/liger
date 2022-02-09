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
#include <tigon/Operators/EmptyOperator.h>

namespace Tigon {
namespace Operators {

EmptyOperator::EmptyOperator()
{
    initialise();
}

EmptyOperator::EmptyOperator(Representation::IPSet* ipset)
    : IOperator(ipset)
{
    initialise();
}

EmptyOperator::~EmptyOperator()
{

}

void EmptyOperator::initialise()
{
    m_name = TString("Empty Operator");
    m_description = TString("An operator without any settings or defalut "
                            "operations. Can be used to store data and pass to "
                            "other operators as input.");
}

void EmptyOperator::evaluateNode()
{

}

TString EmptyOperator::name()
{
    return m_name;
}

TString EmptyOperator::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
