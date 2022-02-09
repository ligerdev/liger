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
#include <tigon/Operators/Initialisation/LHSInit.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/tigonengineregistry.h>

namespace Tigon {
namespace Operators {

LHSInit::LHSInit()
{
    initialise();
}

LHSInit::LHSInit(Representation::IPSet* ipset)
    : IInitialisation(ipset)
{
    initialise();
}

LHSInit::~LHSInit()
{

}

void LHSInit::evaluateNode()
{
    if( outputSets().empty() ) {
        ISet* oSet = appendOutputSet();
        for(int i=0; i<TP_setSize(); i++) {
            createMapping();
        }

        TVector<TVector<int> > LHS =
                latinHypercubeSampling(decisionVecSize(), TP_setSize());

        BoxConstraintsDataSPtr box = boxConstraints();

        for(int idx = 0; idx < decisionVecSize(); idx++) {
            IElement lb = box->lowerBounds().at(idx);
            IElement ub = box->upperBounds().at(idx);

            switch (decisionVecTypes()[idx]) {
            case RealType:
            default:
            {
                double segmentSize =
                        (ub.value<double>() - lb.value<double>()) / TP_setSize();
                double lbReal = lb.value<double>();
                for(int imap = 0; imap < TP_setSize(); imap++) {
                    double rnd = TRAND.randUni();
                    oSet->at(imap)->defineDecisionVar(
                                idx, lbReal + segmentSize * (LHS[imap][idx] + rnd));
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
                    oSet->at(imap)->defineDecisionVar(
                                idx, lbInt + LHS[imap][idx] % nValues);
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

TString LHSInit::name()
{
    return TString("Latin Hypercube Initialisation");
}

TString LHSInit::description()
{
    return TString("Generate decision vectors using "
                   "Latin hypercube sampling.");
}


void LHSInit::initialise()
{

}

} // namespace Operators
} // namespace Tigon
REGISTER_IPSET_FACTORY(Operators, LHSInit)
