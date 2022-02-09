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

    // Algorithm-level properties
    void TP_defineErrorMethod(Tigon::ErrorHandlingMethod err);
    Tigon::ErrorHandlingMethod TP_errorMethod()  const;

    void TP_defineConstraintHandlingMethod(Tigon::ConstraintHandlingMethod chm);
    Tigon::ConstraintHandlingMethod TP_constraintHandlingMethod();

    // Properties found in the operators of the Algorithm
    void TP_defineMaxSolutions(int n);
    int  TP_maxSolutions()      const;

    // General functions
    TVector<IMappingSPtr> evaluatedMappings() const;
    TVector<IMappingSPtr> newMappings()       const;
    TVector<IMappingSPtr> surrogateMappings() const;

    // These methods are the operators' properties, and are exposed in Liger
    // through introspection. They are here for easy interfacing in tests.
    void  defineReferenceSetSize(int     s);
    int   referenceSetSize()          const;

    // Parameters for the optimizer to search the surrogate
    void  defineBudgetPerVariableSS(int    n);
    int   budgetPerVariableSS()         const;

    void defineInitialPopsizePerVariableSS(int popsize);
    int  initialPopsizePerVariableSS()            const;

    void defineStallIterationsSS(int iter);
    int  stallIterationsSS()         const;

    // Parameters for the optimizer to train the surrogate
    void  defineBudgetPerVariableTS(int    n);
    int   budgetPerVariableTS()         const;

    void defineInitialPopsizePerVariableTS(int popsize);
    int  initialPopsizePerVariableTS()            const;

    void defineStallIterationsTS(int iter);
    int  stallIterationsTS()         const;


    void defineNumberIterations(int iter);
    int  numberIterations()         const;

    void defineMultiObjectivization(bool isMultiObjectivizationUsedOrNot);
    bool multiObjectivization() const;

    // TODO: change this property according to the remaining budget
    void  defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType scalarisingFunction()                      const;

    void defineOptimizationSearchQuality(int mode);
    int  optimizationSearchQuality() const;

private:
    void initialise();

    Tigon::ConstraintHandlingMethod m_constraintHandlingMethod;
};

} // namespace Algorithms
} // namespace Tigon

#endif // PAREGO_H
