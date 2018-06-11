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
#ifndef SIMPLEXLATTICEDIRECTIONITERATOR_H
#define SIMPLEXLATTICEDIRECTIONITERATOR_H
#include <tigon/Operators/Directions/IDirection.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SimplexLatticeDirectionIterator : public IDirection
{
    HANDLE_READ_PROPERTIES_BEGIN(IDirection)
    READ(ReferenceSetSize, TP_referenceSetSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IDirection)
    WRITE(ReferenceSetSize, int, TP_defineReferenceSetSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::SimplexLatticeDirectionIterator)

public:
    SimplexLatticeDirectionIterator();
    SimplexLatticeDirectionIterator(Tigon::Representation::IPSet* ipset);
    ~SimplexLatticeDirectionIterator();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Properties
    void  TP_defineReferenceSetSize(int s);
    int   TP_referenceSetSize()      const;

    // General functions
    int  actualReferenceSetSize()           const;

private:
    void initialise();
    void createReferenceSet();
    void nextDirection();
    bool isNewGoalVec();
    int                         m_refSetSize;
    int                         m_currentVec;
    int                   m_actualRefSetSize;
    TVector<TVector<double>>  m_referenceSet;
    TVector<double>            m_normGoalVec;
};

} // namespace Operators
} // namespace Tigon

#endif // SIMPLEXLATTICEDIRECTIONITERATOR_H
