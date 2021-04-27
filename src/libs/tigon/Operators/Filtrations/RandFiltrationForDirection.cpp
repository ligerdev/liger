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
#include <tigon/Operators/Filtrations/RandFiltrationForDirection.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Operators {

RandFiltrationForDirection::RandFiltrationForDirection()
{
    initialise();
}

RandFiltrationForDirection::RandFiltrationForDirection(Tigon::Representation::IPSet* ipset)
    : FiltrationForDirection(ipset)
{
    initialise();
}

RandFiltrationForDirection::~RandFiltrationForDirection()
{

}

void RandFiltrationForDirection::initialise()
{
    m_name = TString("Random filtration for Direction");
    m_description = TString("Creates sets of solutions selected randomly "
                            "from the set tagged \"For Modification\".\n"
                            "The sets are tagged \"For Direction\" and \"Output Sets Size\".\n"
                            "Every set has the same number of solutions. "
                            "This should match the target Direction operator.\n"
                            "A solution may be selected more than once.");

    /*
    m_propertyNames.append("Ratio of new solutions");
    m_propertyDescriptions.append("Number of new solution created by the operator "
                                  "divided by the number of solutions in the input set. "
                                  "Default is 1");

    m_propertyNames.append("New solutions per sub-set");
    m_propertyDescriptions.append("Number of new solutions created by the direction operator from each set. "
                                  "Default is sub-set size");


    TP_defineRatioOfNewSolutions(1.0);
    TP_defineNewSolutionsPerOutputSet(TP_outputSetSize());
    */
}

void RandFiltrationForDirection::evaluateNode()
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

        //TVector<TVector<int> > permIdx = latinHypercubeSampling(1, iSetSize);
        TVector<int> permIdx = randomPermutationSampling(iSetSize);

        int i = 0;
        while(i<maxSolutionsPerInputSet) {
            appendOutputSet();
            for(int j=0; j<TP_outputSetSize(); j++) {
                cloneMapping(iSet->at(permIdx[i]));
                i++;
                if(i >= maxSolutionsPerInputSet) {
                    break;
                }
            }
        }
    }

    /*
    if(hasNextInputSet()) {

        ISet* iSet = inputSet(0);

        int sSize = iSet->size();
        // TODO: Thing of a clever way to maintain the population size
        //       when several modification operators are used.
        //       Currently, all operators use ceil, and the merge operator
        //       doesn't use the extra solutions (produced by the last operator
        //       in its list)


        int  nNew = std::ceil(sSize * m_ratioOfSolutions);
        int  c    = 0;
        bool cont = true;
        double inOutRatio = m_newSolutionsPerOutputSet / (double)m_outputSetSize;
        while(cont) {
            appendOutputSet();
            for(int j=0; j<m_outputSetSize; j++) {
                int idx = TRAND.randInt(sSize);
                cloneMapping(iSet->at(idx));
                c++;
                if(c * inOutRatio >= nNew) {
                    cont = false;
                    break;
                }
            }
        }
    }
    */
}

/*
void RandFiltrationForDirection::TP_defineRatioOfNewSolutions(double r)
{
    if(r > 0.0) {
        m_ratioOfSolutions = r;
    }
}

double RandFiltrationForDirection::TP_ratioOfNewSolutions() const
{
    return m_ratioOfSolutions;
}

void RandFiltrationForDirection::TP_defineNewSolutionsPerOutputSet(int n)
{
    if(n>=0) {
        m_newSolutionsPerOutputSet = n;
    }
}

int RandFiltrationForDirection::TP_newSolutionsPerOutputSet() const
{
    return m_newSolutionsPerOutputSet;
}
*/

TString RandFiltrationForDirection::name()
{
    return m_name;
}

TString RandFiltrationForDirection::description()
{
    return m_description;
}

} // namespace Operators
} // namespace Tigon
