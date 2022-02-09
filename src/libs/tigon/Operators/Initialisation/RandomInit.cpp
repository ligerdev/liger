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
#include <tigon/Operators/Initialisation/RandomInit.h>
#include <tigon/Operators/Initialisation/IInitialisation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/tigonengineregistry.h>
#include <stdlib.h>

namespace Tigon {
namespace Operators {

RandomInit::RandomInit()
{

}

RandomInit::RandomInit(Representation::IPSet* ipset)
    : IInitialisation(ipset)
{

}

RandomInit::~RandomInit()
{

}

TString RandomInit::name()
{
    return TString("Random Initialisation");
}

TString RandomInit::description()
{
    return TString("Generate random decision vectors with uniform "
                   "probability within the domain");
}

void RandomInit::evaluateNode()
{
    if( outputSets().empty() ) {
        ISet* oSet = appendOutputSet();
        for(int i=0; i<TP_setSize(); i++) {
            createMapping();
        }

        BoxConstraintsDataSPtr box = boxConstraints();

        for(int idx = 0; idx < decisionVecSize(); idx++) {
            IElement lb = box->lowerBounds().at(idx);
            IElement ub = box->upperBounds().at(idx);

            switch (decisionVecTypes()[idx]) {
            case RealType:
            default:
            {
                double interval = ub.value<double>() - lb.value<double>();
                double lbReal = lb.value<double>();
                for(int imap = 0; imap < TP_setSize(); imap++) {
                    oSet->at(imap)->
                            defineDecisionVar(idx, TRAND.randUni(interval, lbReal));
                }
                break;
            }
            case IntegerType:
            case OrdinalType:
            case NominalType:
            {
                int nValues = ub.value<int>() - lb.value<int>() + 1;
                int lbInt = lb.value<int>();
                for(int imap = 0; imap < TP_setSize(); imap++) {
                    oSet->at(imap)->
                            defineDecisionVar(idx, TRAND.randInt(nValues, lbInt));
                }
                break;
            }
            }

        }
        appendInitialSet();

    } else {
        appendInitialSet();
    }
}

} // namespace Operators
} // namespace Tigon
REGISTER_IPSET_FACTORY(Operators, RandomInit)

