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
#ifndef IINITIALISATION_H
#define IINITIALISATION_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

/*!
 * \brief The IInitialisation class
 */
class LIGER_TIGON_EXPORT IInitialisation : public IOperator
{
    HANDLE_READ_PROPERTIES_BEGIN(IOperator)
    READ(OptimizationSetSize, TP_setSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IOperator)
    WRITE(OptimizationSetSize, int, TP_defineSetSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IInitialisation)

public:
    IInitialisation();
    IInitialisation(Tigon::Representation::IPSet* ipset);
    virtual ~IInitialisation();

    // Properties
    void TP_defineSetSize(int sz);
    int  TP_setSize()       const;

    void  defineInitialSet(ISet* initSet);

    ISet* initialSet()              const;

    void defineInitialDVecs(ISet*                           initDVecs);
    void defineInitialDVecs(const TVector<TVector<double> > &initDVecs);

    void appendDVecs(const TVector<TVector<double> > &newSols);
    void appendSolution(IMappingSPtr newImap);
    void appendSolutions(ISet*       newSet);

protected:
    virtual void appendInitialSet();

private:
    void initialise();
    int64                     m_setSize;
    TVector<TVector<double> > m_dVec;
    ISet*                     m_initialSet;
};

} // namespace Operators
} // namespace Tigon

#endif // IINITIALISATION_H
