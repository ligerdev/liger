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
#include <tigon/Operators/Filtrations/FiltrationForDirection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>

namespace Tigon {
namespace Operators {

FiltrationForDirection::FiltrationForDirection()
{
    FiltrationForDirection::initialise();
}

FiltrationForDirection::FiltrationForDirection(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    FiltrationForDirection::initialise();
}

FiltrationForDirection::~FiltrationForDirection()
{

}

void FiltrationForDirection::initialise()
{
    m_name = TString("Filtration for Direction");
    m_description = TString("Creates sets of solutions from the set tagged "
                            "\"for modification\". All solutions in the set are "
                            "stored in sets of size defined by the Sub-set size "
                            "property.\nThe sets are tagged \"For Direction\" "
                            "and \"Output Sets Size\".\n"
                            "Every set has the same number of solutions. "
                            "This should match the target Direction operator.\n"
                            "A solution may be selected more than once.");

    addProperty("InputSetSize"
                , TString("Number of solutions to select for each sub-set. Default is 2")
                , getTType(int));
    addProperty("OutputSetSize"
                , TString("Maximum number of new solutions created by the direction operator "
                          "from each input set. "
                          "Default is 0")
                , getTType(int));

    addInputTag(Tigon::TForSelection);
    addOutputTag(Tigon::TForDirection);

    TP_defineOutputSetSize(2);
    TP_defineInputSetSize(0);
}

void FiltrationForDirection::evaluateNode()
{
    // Init
    clearOutputSets();

    while(hasNextInputSet()) {

        ISet* iSet = nextInputSet();
        int iSetSize = iSet->size();

        int maxSolutionsPerInputSet;
        if(TP_inputSetSize()<=0) {
            maxSolutionsPerInputSet = iSetSize;
        }
        else {
            maxSolutionsPerInputSet = std::min(iSetSize,TP_inputSetSize());
        }

        int i = 0;
        while(i<maxSolutionsPerInputSet) {
            appendOutputSet();
            for(int j=0; j<TP_outputSetSize(); j++) {
                cloneMapping(iSet->at(i));
                i++;
                if(i >= maxSolutionsPerInputSet) {
                    break;
                }
            }
        }
    }
}

void FiltrationForDirection::TP_defineOutputSetSize(int n) {

    if(n>0) {
        m_outputSetSize = n;
    }

}

int  FiltrationForDirection::TP_outputSetSize() const {
    return m_outputSetSize;
}

void FiltrationForDirection::TP_defineInputSetSize(int n)
{
    m_inputSetSize = n;
}

int FiltrationForDirection::TP_inputSetSize() const
{
    return m_inputSetSize;
}

TString FiltrationForDirection::name()
{
    return m_name;
}

TString FiltrationForDirection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
