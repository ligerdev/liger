/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Operators/Initialisation/TSPOrderedInit.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/tigonengineregistry.h>


namespace Tigon {
namespace Operators {

TSPOrderedInit::TSPOrderedInit()
{
    initialise();
}

TSPOrderedInit::TSPOrderedInit(Representation::IPSet* ipset)
    : IInitialisation(ipset)
{
    initialise();
}

TSPOrderedInit::~TSPOrderedInit()
{

}

void TSPOrderedInit::evaluateNode()
{
    if( outputSets().empty() ) {
        ISet* oSet = appendOutputSet();
        for(int i=0; i<TP_setSize(); i++) {
            createMapping();
        }

        BoxConstraintsDataSPtr box = boxConstraints();

        TVector<IElement> vlb = box->lowerBounds();
        auto i1 = std::adjacent_find(vlb.begin(), vlb.end(),
                                     [](const IElement &e1,
                                     const IElement &e2){return e1 != e2;});
        if(i1 != vlb.end()) {
            throw TException(this->className(), "Not all lower bounds of the "
                                                "decision variable are equal");
        }

        TVector<IElement> vub = box->upperBounds();
        auto i2 = std::adjacent_find(vub.begin(), vub.end(),
                                     [](const IElement &e1,
                                     const IElement &e2){return e1 != e2;});
        if(i2 != vub.end()) {
            throw TException(this->className(), "Not all upper bounds of the "
                                                "decision variable are equal");
        }

        IElement lb = box->lowerBounds().at(0);
        IElement ub = box->upperBounds().at(0);

        int n = ub.value<int>() - lb.value<int>() + 1;
        int lbInt = lb.value<int>();

        switch (decisionVecTypes()[0]) {
        case IntegerType:
        case OrdinalType:
        case NominalType:
        {
            std::vector <int> permutation(n);

            for(int imap = 0; imap < TP_setSize(); imap++) {

                for(int i=0; i<n; i++) {
                    permutation[i] = lbInt+i;
                }

                // swap the elements of the vector
                for(int i=0; i<=n-2; i++) {
                    // generate a rand integer such that 0 <= j < n-i
                    int j = TRAND.randInt(n-i);
                    // Swap an existing element [i+j] to position [i]
                    std::iter_swap(permutation.begin() + i,
                                   permutation.begin() + (i+j));
                }

                for(int idx = 0; idx < decisionVecSize(); idx++) {
                    oSet->at(imap)->defineDecisionVar(idx, permutation[idx]);
                }
            }
        }
        case RealType:
        default:
        {

        }
        }
        appendInitialSet();
    } else {
        appendInitialSet();
    }
}

TString TSPOrderedInit::name()
{
    return TString("Ordered Initialisation");
}

TString TSPOrderedInit::description()
{
    return TString("Initialises a vector where the elements are all different. "
                   "Different vectors are created by applying random swap "
                   "operations between the elements of the vector. "
                   "The algorithm employed is known as Knuth shuffle.");
}


void TSPOrderedInit::initialise()
{

}


} // namespace Operators
} // namespace Tigon

//REGISTER_IPSET_FACTORY(Operators, OrderedInit)
