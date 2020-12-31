/****************************************************************************
**
** Copyright (C) 2012-2017 The University of Sheffield (www.sheffield.ac.uk)
** Copyright (C) 2016 Joao A. Duro (www.sheffield.ac.uk)
**
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
#include <tigon/Operators/Filtrations/TruncateSets.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

TruncateSets::TruncateSets()
{
    initialise();
}

TruncateSets::TruncateSets(Tigon::Representation::IPSet* ipset) :
    IFiltration(ipset)
{
    initialise();
}

TruncateSets::~TruncateSets()
{

}

void TruncateSets::initialise()
{
    m_name = TString("Truncates the Output Sets");
    m_description = TString("Truncates the Output Sets by deleting elements");

    addProperty("SetSize"
                , TString("Specifies the size of the output set.")
                , getTType(int));

    addOutputTag(Tigon::TForResize);

    TP_defineSetSize(2);
}

void TruncateSets::evaluateNode()
{
    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        int sSize = oSet->size();

        if(sSize>m_setSize) {
            for(int i=0; i<(sSize-m_setSize); i++) {
                oSet->remove(sSize-1-i);
            }
        }
    }
}

TString TruncateSets::name()
{
    return m_name;
}

TString TruncateSets::description()
{
    return m_description;
}

int TruncateSets::TP_setSize() const
{
    return m_setSize;
}

void TruncateSets::TP_defineSetSize(int s)
{
    if(s>0) {
        m_setSize = s;
    }
}

} // namespace Operators
} // namespace Tigon
