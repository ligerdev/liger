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
#include <tigon/Operators/Filtrations/ExampleFiltration.h>
#include <tigon/Representation/Sets/ISet.h>

namespace Tigon {
namespace Operators {

ExampleFiltration::ExampleFiltration()
{
    m_name = QString("Example Filtration");
    m_description = QString("Partition a set to multiple subsets");

    TP_defineClusterSize(4);
}

ExampleFiltration::ExampleFiltration(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{   
    m_name = TString("Example Filtration");
    m_description = TString("Partition a set to multiple subsets");

    TP_defineClusterSize(4);
}

ExampleFiltration::~ExampleFiltration()
{

}

int  ExampleFiltration::TP_clusterSize() const
{
    return m_clusterSize;
}

void ExampleFiltration::TP_defineClusterSize(int s)
{
    m_clusterSize = s;
}

void ExampleFiltration::evaluateNode()
{
    // Init
    clearOutputSets();
    ISet* oSet;
    ISet* iSet = nextInputSet();
    if(iSet != Q_NULLPTR) {
        int sSize = iSet->size();
        int idx = 0;
        while(sSize > 0) {
            oSet = appendOutputSet();
            if(sSize >= m_clusterSize) {
                for(int j=0; j<m_clusterSize; j++) {
                    oSet->append(iSet->at(idx++));
                }
                sSize -= m_clusterSize;
            } else if(sSize > 0) {
                for(int j=0; j<sSize; j++) {
                    oSet->append(iSet->at(idx++));
                }
                sSize = 0;
            }
        }
    }
}

TString ExampleFiltration::name()
{
    return m_name;
}

TString ExampleFiltration::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
