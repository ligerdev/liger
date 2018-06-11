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
#ifndef IDIRECTION_H
#define IDIRECTION_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IDirection : public IOperator
{
    HANDLE_READ_PROPERTIES_BEGIN(IOperator)
    READ(SolutionCrossoverProbability, TP_solutionCrossoverProbability)
    READ(VariableCrossoverProbability, TP_variableCrossoverProbability)
    READ(VariableSwapCrossoverProbability, TP_variableSwapCrossoverProbability)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IOperator)
    WRITE(SolutionCrossoverProbability, double, TP_defineSolutionCrossoverProbability)
    WRITE(VariableCrossoverProbability, double, TP_defineVariableCrossoverProbability)
    WRITE(VariableSwapCrossoverProbability, double, TP_defineVariableSwapCrossoverProbability)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IDirection)

public:
    IDirection();
    IDirection(Representation::IPSet *ipset);
    virtual ~IDirection();

    virtual void evaluateNode()  = 0;

    // Information about the node.
    virtual TString     name()          = 0;
    virtual TString     description()   = 0;

    /**************/
    /* Properties */
    /**************/

    // Solution Probability
    double TP_solutionCrossoverProbability()        const;
    void  TP_defineSolutionCrossoverProbability(double p);

    // Variable Probability
    double TP_variableCrossoverProbability() const;
    void  TP_defineVariableCrossoverProbability (double p);

    // Variable Swap Probability
    double TP_variableSwapCrossoverProbability() const;
    void  TP_defineVariableSwapCrossoverProbability (double p);

protected:
    bool doSolutionCrossover();
    bool doVariableCrossover();
    bool doVariableSwap();

private:
    void initialise();
    double   m_solutionCrossoverProbability;
    double   m_variableCrossoverProbability;
    double   m_variableSwapCrossoverProbability;

};

} // namespace Operators
} // namespace Tigon

#endif // IDIRECTION_H
