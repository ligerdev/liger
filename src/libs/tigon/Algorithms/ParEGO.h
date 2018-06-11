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
#ifndef PAREGO_H
#define PAREGO_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Algorithms/IAlgorithm.h>

using namespace Tigon::Representation;

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT ParEGO : public IAlgorithm
{
    HANDLE_READ_PROPERTIES_BEGIN(IAlgorithm)
    READ(maxSolutions, TP_maxSolutions)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IAlgorithm)
    WRITE(maxSolutions, int, TP_defineMaxSolutions)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Algorithms::ParEGO)

public:
    ParEGO();
    ParEGO(IPSet* ipset);
    ~ParEGO();

    // Information about the node.
    TString     name();
    TString     description();

    // Algrithm-level properties
    void  TP_defineMaxSolutions(int         n);
    int   TP_maxSolutions()              const;

    void  TP_defineNeighbourhoodRadius(double r);
    double TP_neighbourhoodRadius()        const;

    // General functions
    TVector<IMappingSPtr> evaluatedMappings() const;
    TVector<IMappingSPtr> newMappings()       const;

    // These methods are the operators' properties, and are exposed in Liger
    // through introspection. They are here for easy interfacing in tests.
    void  defineReferenceSetSize(int     s);
    int   referenceSetSize()          const;

    void  defineBudgetPerVariable(int    n);
    int   budgetPerVariable()         const;

    // TODO: change this property according to the remaining budget
    void  defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType scalarisingFunction()                     const;

private:
    void initialise();
};

} // namespace Algorithms
} // namespace Tigon

#endif // PAREGO_H
