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
#include <tigon/Operators/Filtrations/ExampleRandSelection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Random/RandomGenerator.h>
#include <cstdlib>
#include <ctime>


namespace Tigon {
namespace Operators {

ExampleRandSelection::ExampleRandSelection()
{
    m_propertyNames.append("Pool Size");
    m_propertyDescriptions.append("Number of solutions to select. Default is 1");
    m_name = QString("Random selection");
    m_description = QString("Creates a set of solutions selected randomly from another set.\n");
    m_description += QString("A solution may be selected more than once");

    TP_definePoolSize(1);
}

ExampleRandSelection::ExampleRandSelection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    m_propertyNames.append("Pool Size");
    m_propertyDescriptions.append("Number of solutions to select. Default is 1");
    m_name = QString("Random selection");
    m_description = QString("Creates a set of solutions selected randomly from another set.\n");
    m_description += QString("A solution may be selected more than once");


    TP_definePoolSize(1);
}

ExampleRandSelection::~ExampleRandSelection()
{

}

void ExampleRandSelection::evaluateNode()
{
    // Init
    clearOutputSets();
    ISet* oSet = appendOutputSet();
    ISet* iSet = nextInputSet();

    int sSize = iSet->size();

    for(int i=0; i<m_poolSize; i++) {
        int idx = TRAND.randInt(sSize);
        oSet->append(iSet->at(idx));
    }
}

void ExampleRandSelection::TP_definePoolSize(int n) {
    m_poolSize = n;
}

int ExampleRandSelection::TP_poolSize() const {
    return m_poolSize;
}

QString ExampleRandSelection::name()
{
    return m_name;
}

QString ExampleRandSelection::description()
{
    return m_description;
}


QStringList ExampleRandSelection::propertyNames()
{
    return m_propertyNames;
}

QStringList ExampleRandSelection::propertyDescriptions()
{
    return m_propertyDescriptions;
}

} // namespace Operators
} // namespace Tigon
