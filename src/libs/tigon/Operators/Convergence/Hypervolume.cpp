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
#include <tigon/Operators/Convergence/Hypervolume.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Utils/HypervolumeUtils.h>
#include <tigon/Utils/IElementUtils.h>

namespace Tigon {
namespace Operators {

Hypervolume::Hypervolume()
{
    initialise();
}

Hypervolume::Hypervolume(Tigon::Representation::IPSet* ipset)
    : IConvergence(ipset)
{
    initialise();
}

Hypervolume::~Hypervolume()
{

}

void Hypervolume::initialise()
{
    m_isDefinedRef = false;
}

void Hypervolume::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    ISet* iSet = setsUnion(inputSets());
    iSet->shuffle();

    int nSolutions = iSet->size();

    if(nSolutions == 0) {
        // ERROR?
        return;
    }

    // Reference point
    if(!m_isDefinedRef) {
        m_reference = IElementVecToRealVec(antiIdealVec());
    }

    // Calculate hypervolume
    TVector<TVector<double> > approxSet(nSolutions,
                                        TVector<double>(objectiveVecSize()));
    for(int j = 0; j < nSolutions; j++) {
        for(int k = 0; k < objectiveVecSize(); k++) {
            approxSet[j][k] = iSet->at(j)->objectiveVar(k)->sample();
        }        
    }
    m_convIndicatorValue = hypervolume(approxSet, m_reference);

    delete iSet;
}

void Hypervolume::defineReferencePoint(TVector<double> refPoint)
{
    m_reference = refPoint;
    m_isDefinedRef = true;
}

TVector<double> Hypervolume::referencePoint() const
{
    return m_reference;
}

TString Hypervolume::name()
{
    return TString("Hypervolume Convergence");
}

TString Hypervolume::description()
{
    return TString("A measure for convergence of the solutions to a "
                   "multiobjective problem.\n"
                   "The convergence value is the hypervolume enclosed between "
                   "the set of solutions and a reference point.\n"
                   "If no reference point is defined, the anti-ideal vector "
                   "is used as the reference point.");
}

} // namespace Operators
} // namespace Tigon
