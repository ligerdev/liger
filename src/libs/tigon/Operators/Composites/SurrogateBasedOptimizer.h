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
#ifndef SURROGATEBASEDOPTIMIZER_H
#define SURROGATEBASEDOPTIMIZER_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Composites/IComposite.h>

using Tigon::ErrorHandlingMethod;

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SurrogateBasedOptimizer : public IComposite
{
    HANDLE_READ_PROPERTIES_BEGIN(IComposite)
    READ(BudgetPerVariable, TP_budgetPerVariable)
    READ(NeighbourhoodRadius, TP_neighbourhoodRadius)
    //READ(ErrorMethod, TP_errorMethod)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IComposite)
    WRITE(BudgetPerVariable, int, TP_defineBudgetPerVariable)
    WRITE(NeighbourhoodRadius, double, TP_defineNeighbourhoodRadius)
    //WRITE(ErrorMethod, ErrorHandlingMethod, TP_defineErrorMethod)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::SurrogateBasedOptimizer)

public:
    SurrogateBasedOptimizer();
    SurrogateBasedOptimizer(Representation::IPSet* ipset);
    virtual ~SurrogateBasedOptimizer();

    void evaluateNode();

    // properties
    void TP_defineBudgetPerVariable(int n);
    int  TP_budgetPerVariable()      const;

    void  TP_defineNeighbourhoodRadius(double r);
    double TP_neighbourhoodRadius()        const;

    void TP_defineErrorMethod(ErrorHandlingMethod err);
    ErrorHandlingMethod TP_errorMethod() const;

    // Information about the node
    TString     name();
    TString     description();

private:
    void initialise();

    int                        m_budgetPerVar;
    Tigon::ErrorHandlingMethod m_errMethod;
    double                      m_neighbourhoodRadius;
};

} // namespace Operators
} // namespace Tigon

#endif // SURROGATEBASEDOPTIMIZER_H
