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
#include <tigon/Operators/Perturbations/CategoricalPerturpation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Constraints/BoxConstraintsData.h>
#include <tigon/Random/RandomGenerator.h>
#include <tigon/Utils/IElementUtils.h>



namespace Tigon {
namespace Operators {

CategoricalPerturpation::CategoricalPerturpation()
{
    initialise();
}

CategoricalPerturpation::CategoricalPerturpation(Tigon::Representation::IPSet* ipset)
    : IPerturbation(ipset)
{
    initialise();
}

void CategoricalPerturpation::initialise()
{
    TStringList types;
    types << Tigon::ETNominal << Tigon::ETOrdinal;
    defineSupportedElementTypes(types);
}


CategoricalPerturpation::~CategoricalPerturpation()
{

}

void CategoricalPerturpation::evaluateNode()
{
    TVector<IElement> lb = boxConstraints()->lowerBounds();
    TVector<IElement> ub = boxConstraints()->upperBounds();

	TVector<ElementType> dVecTypes = decisionVecTypes();
    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        for(int i=0; i<oSet->size(); i++) {
            IMappingSPtr imap = oSet->at(i);

            // probability of mutating a solution
            if( doSolutionMutation() ) {
                for(int j=0; j<decisionVecSize(); j++) {
                    if( isSupportedElementType(dVecTypes.at(j)) ) {
                    // probability of mutating a variable
                        if( doVariableMutation() ) {
                            int nVals = ub[j].value<int>() - lb[j].value<int>() + 1;
                            int newVal = TRAND.randInt(nVals, lb[j].value<int>());
                            imap->defineDecisionVar(j, newVal);
                        }
                    }
                }
            }
        }
    }
}

TString CategoricalPerturpation::name()
{
    return TString("Nominal Perturbation");
}

TString CategoricalPerturpation::description()
{
    return TString("Replace the value of a nominal variable with a "
                   "new randomly generated value within the range.");
}

} // namespace Operators
} // namespace Tigon
