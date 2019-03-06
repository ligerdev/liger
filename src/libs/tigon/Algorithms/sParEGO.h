/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef SPAREGO_H
#define SPAREGO_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Operators {
class IOperator;
}
namespace Representation {
class IRobustnessIndicator;
}
}

using namespace Tigon::Representation;

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT sParEGO : public IAlgorithm
{
    HANDLE_READ_PROPERTIES_BEGIN(IAlgorithm)
    //READ(ErrorMethod, TP_errorMethod)
    READ(maxSolutions, TP_maxSolutions)
    READ(neighbourhoodRadius, TP_neighbourhoodRadius)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IAlgorithm)
    //WRITE(ErrorMethod, Tigon::ErrorHandlingMethod, TP_defineErrorMethod)
    WRITE(maxSolutions, int, TP_defineMaxSolutions)
    WRITE(neighbourhoodRadius, double, TP_defineNeighbourhoodRadius)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Algorithms::sParEGO)

public:
    sParEGO();
    sParEGO(IPSet* ipset);
    virtual ~sParEGO();

    void initialise();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    // Algrithm-level properties
    void TP_defineErrorMethod(Tigon::ErrorHandlingMethod err);
    Tigon::ErrorHandlingMethod TP_errorMethod()         const;

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
    void  defineIndicator(TString indicatorType, double indParam=0);

    double percentile()        const;
    void  definePercentile(double p);

    void  defineBudgetPerVariable(int    n);
    int   budgetPerVariable()         const;

    // TODO: change this property according to the remaining budget
    void  defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType scalarisingFunction()                     const;

private:
    Tigon::ErrorHandlingMethod    m_errMethod;
};

} // namespace Algorithms
} // namespace Tigon

#endif // SPAREGO_H
