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
#include <tigon/Operators/Evaluators/ExampleEvaluator.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>

namespace Tigon {
namespace Operators {

ExampleEvaluator::ExampleEvaluator()
{    
    m_name = QString("ExampleEvaluator");
    m_description = QString("This is a multirow description of ExampleEvaluator.");
}

ExampleEvaluator::ExampleEvaluator(Tigon::Representation::IPSet *ipset)
    : IEvaluator(ipset)
{
    m_name = QString("ExampleEvaluator");
    m_description = QString("This is a multirow description of ExampleEvaluator.");
}

ExampleEvaluator::~ExampleEvaluator() {

}

void ExampleEvaluator::evaluateNode()
{
    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();

        for(int idx=0; idx<oSet->size(); idx++) {
            IMappingSPtr imap = oSet->at(idx);
            imap->evaluate();
        }
    }
}

QString ExampleEvaluator::name()
{
    return m_name;
}

QString ExampleEvaluator::description()
{
    return m_description;
}


QStringList ExampleEvaluator::propertyNames()
{
    return m_propertyNames;
}

QStringList ExampleEvaluator::propertyDescriptions()
{
    return m_propertyDescriptions;
}

} // namespace Operators
} // namespace Tigon
