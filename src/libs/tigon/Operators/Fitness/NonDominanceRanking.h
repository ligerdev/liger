/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef NONDOMINANCERANKING_H
#define NONDOMINANCERANKING_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Representation {
class DominanceRelation;
class DominanceRelationFactory;
}
}

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT NonDominanceRanking : public IFitness
{
    HANDLE_READ_PROPERTIES_BEGIN(IFitness)
    READ(IsConstrainedHandlingUsed, TP_isConstrainedHandlingUsed)
    READ(IsPreferabilityUsed, TP_isPreferabilityUsed)
    READ(IsWeakDom, TP_isWeakDom)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFitness)
    WRITE(IsConstrainedHandlingUsed, bool, TP_defineIsConstrainedHandlingUsed)
    WRITE(IsPreferabilityUsed, bool, TP_defineIsPreferabilityUsed)
    WRITE(IsWeakDom, bool, TP_defineIsWeakDom)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::NonDominanceRanking)

public:
    NonDominanceRanking();
    NonDominanceRanking(Tigon::Representation::IPSet* ipset);
    ~NonDominanceRanking();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    /**************/
    /* Properties */
    /**************/

    void TP_defineIsConstrainedHandlingUsed(bool b);
    bool TP_isConstrainedHandlingUsed() const;

    void TP_defineIsPreferabilityUsed(bool b);
    bool TP_isPreferabilityUsed() const;

    void TP_defineIsWeakDom(bool b);
    bool TP_isWeakDom() const;

protected:
    void initialise();

private:

    void defineRelationType(DominanceRelationType r);

    bool m_isConstrainedHandlingUsed;
    bool m_isPreferabilityUsed;
    bool m_isWeakDom;

    DominanceRelation*        m_domRelation;
    DominanceRelationType     m_domRelationType;
    DominanceRelationFactory* m_factory;
};

} // namespace Operators
} // namespace Tigon

#endif // NONDOMINANCERANKING_H
