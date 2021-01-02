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
#ifndef IPERTURBATION_H
#define IPERTURBATION_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IPerturbation : public IOperator
{
    HANDLE_READ_PROPERTIES_BEGIN(IOperator)
    READ(SolutionMutationProbability, TP_solutionMutationProbability)
    READ(VariableMutationProbability, TP_variableMutationProbability)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IOperator)
    WRITE(SolutionMutationProbability, double, TP_defineSolutionMutationProbability)
    WRITE(VariableMutationProbability, double, TP_defineVariableMutationProbability)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IPerturbation)

public:
    IPerturbation();
    IPerturbation(Tigon::Representation::IPSet* ipset);
    virtual ~IPerturbation();

    /// Properties
    // Solution Probability
    double TP_solutionMutationProbability()        const;
    void  TP_defineSolutionMutationProbability(double p);

    // Variable Probability
    double TP_variableMutationProbability()        const;
    void  TP_defineVariableMutationProbability(double p);

    virtual void evaluateNode() = 0;

    // Information about the node.
    virtual TString     name()          = 0;
    virtual TString     description()   = 0;
protected:
    bool doSolutionMutation();
    bool doVariableMutation();

private:
    void initialise();
    double   m_solutionMutationProbability;
    double   m_variableMutationProbability;
};

} // namespace Operators
} // namespace Tigon

#endif // IPERTURBATION_H
