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
#include <tigon/Operators/Directions/ExampleCrossOver.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Operators/Random/RGenerator.h>
#include <ctime>


namespace Tigon {
namespace Operators {

ExampleCrossOver::ExampleCrossOver()
{
    initialise();
}

ExampleCrossOver::ExampleCrossOver(Tigon::Representation::IPSet* ipset)
    : IDirection(ipset)
{
    initialise();
}

ExampleCrossOver::~ExampleCrossOver()
{

}

void ExampleCrossOver::initialise()
{
    m_name = QString("Example Cross-over operation");
    m_description = QString("For every pair of parent values a, b "
                            "the offspring values are a+0.25(b-a) and a+0.75(b-a)");

    QStringList types;
    types << Tigon::ETReal << Tigon::ETInteger;
    TP_defineSupportedElementTypes(types);
}

void ExampleCrossOver::evaluateNode()
{
    qreal ratio = 0.25;
    while(hasNextOutputSet()) {

        ISet* oSet = nextOutputSet();
        IMappingSPtr imapA = oSet->at(0);
        IMappingSPtr imapB = oSet->at(1);
        for(int var=0; var<decisionVecSize(); var++) {
            IElement aVar = *(imapA->decisionVar(var));
            IElement bVar = *(imapB->decisionVar(var));
            Tigon::ElementType vType = decisionVecTypes().at(var);
            if( isSupportedElementType(vType) ) {
                IElement xVar = aVar + (bVar - aVar)*ratio;
                IElement yVar = aVar + (bVar - aVar)*(1.0-ratio);
                imapA->setDecisionVar(var, xVar);
                imapB->setDecisionVar(var, yVar);
            }
        }
    }
}

QString ExampleCrossOver::name()
{
    return m_name;
}

QString ExampleCrossOver::description()
{
    return m_description;
}

QStringList ExampleCrossOver::propertyNames()
{
    return m_propertyNames;
}

QStringList ExampleCrossOver::propertyDescriptions()
{
    return m_propertyDescriptions;
}

} // namespace Operators
} // namespace Tigon
