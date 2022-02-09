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
#include <tigon/Operators/Filtrations/MergeForNextIteration.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Sets/ISet.h>

namespace Tigon {
namespace Operators {

MergeForNextIteration::MergeForNextIteration()
{
    initialise();
}

MergeForNextIteration::MergeForNextIteration(Tigon::Representation::IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

MergeForNextIteration::~MergeForNextIteration()
{

}

void MergeForNextIteration::initialise()
{
    defineInputTags(TStringList({Tigon::TForNextIteration}));

    defineOutputTags(TStringList({Tigon::TMainOptimizationSet}));
}

void MergeForNextIteration::evaluateNode()
{
    // Init
    ISet* output = outputSet(0);

    int outputSetSize = output->size();
    int ind = 0;
    while(hasNextInputSet()) {
        ISet* input = nextInputSet();
        for(int i=0; i<input->size(); i++) {
            if(ind == outputSetSize) {
                return;
            }
            output->define(ind++, input->at(i));
        }
    }
}

TString MergeForNextIteration::name()
{
    return TString("Merge for Next Iteration");
}

TString MergeForNextIteration::description()
{
    return TString("Merges all sets tagged \"For Next Iteration\" into "
                   "the main optimization set.");
}
} // namespace Operators
} // namespace Tigon
