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
#include <tigon/Operators/Initialisation/IInitialisation.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

IInitialisation::IInitialisation()
{
    initialise();
}

IInitialisation::IInitialisation(Tigon::Representation::IPSet* ipset)
    : IOperator(ipset)
{
    initialise();
}

IInitialisation::~IInitialisation()
{
    delete m_initialSet;
}

void IInitialisation::initialise()
{
    addProperty("OptimizationSetSize"
                , TString("Number of mappings to initialise. Default is one")
                , getTType(int));

    addOutputTag(Tigon::TMainOptimizationSet);
    addOutputTag(Tigon::TForEvaluation);

    m_initialSet = new ISet;

    m_setSize = 0;
}

void IInitialisation::TP_defineSetSize(int sz)
{
    if(sz >= 0) {
        m_setSize = sz;
    } else {
        throw TException(this->className(), DomainException);
    }
}

int IInitialisation::TP_setSize() const
{
    return static_cast<int>(m_setSize);
}

void IInitialisation::defineInitialSet(ISet* initSet)
{
    if(initSet != nullptr) {
        m_initialSet->define(initSet->all());
    } else {
        // ERROR
    }
}

ISet* IInitialisation::initialSet() const
{
    return m_initialSet;
}

void IInitialisation::defineInitialDVecs(ISet* initDVecs)
{
    TVector<TVector<double> > dVarArray(initDVecs->size());
    for(int i = 0; i < dVarArray.size(); i++) {
        dVarArray[i] = initDVecs->at(i)->doubleDecisionVec();
    }
    defineInitialDVecs(dVarArray);
}

void IInitialisation::defineInitialDVecs(const TVector<TVector<double> > &initDVecs)
{
    // clear data
    m_initialSet->clearMappings();
    m_dVec = initDVecs;
}

void IInitialisation::appendSolution(IMappingSPtr newImap)
{
    m_initialSet->append(newImap);
}

void IInitialisation::appendDVecs(const TVector<TVector<double> > &newSols)
{
    m_dVec.insert(m_dVec.end(), newSols.begin(), newSols.end());
}

void IInitialisation::appendSolutions(ISet* newSet)
{
    m_initialSet->append(newSet);
}

void IInitialisation::appendInitialSet()
{
    // Copy from ISet
    if(m_initialSet->size() > 0) {
        currentOutputSet()->append(m_initialSet);
        m_initialSet->clearMappings();
    }

    // Copy from DVec
    for(int i = 0; i < m_dVec.size(); i++) {
        IMappingSPtr imap = createMapping();

        for(int j=0; j<m_dVec[i].size(); ++j) {
            imap->decisionVar(j)->defineValue(m_dVec[i][j]);
        }

        m_dVec[i].clear();
    }
    m_dVec.clear();
}

} // namespace Operators
} // namespace Tigon
