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
#ifndef SURROGATEBASEDOPTIMIZERWITHPERTURBATION_H
#define SURROGATEBASEDOPTIMIZERWITHPERTURBATION_H
#include <tigon/Operators/Composites/SurrogateBasedOptimizer.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SurrogateBasedOptimizerWithPerturbation
        : public SurrogateBasedOptimizer
{
    HANDLE_READ_PROPERTIES_BEGIN(IComposite)
    READ(NeighbourhoodRadius, TP_neighbourhoodRadius)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IComposite)
    WRITE(NeighbourhoodRadius, double, TP_defineNeighbourhoodRadius)
    HANDLE_WRITE_PROPERTIES_END


    DECLARE_CLASS(Tigon::Operators::SurrogateBasedOptimizerWithPerturbation)

public:
    SurrogateBasedOptimizerWithPerturbation();
    SurrogateBasedOptimizerWithPerturbation(Representation::IPSet* ipset);
    ~SurrogateBasedOptimizerWithPerturbation();

    void evaluateNode();

    // Properties
    void  TP_defineNeighbourhoodRadius(double r);
    double TP_neighbourhoodRadius()        const;

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();

    double m_neighbourhoodRadius;
};

} // namespace Operators
} // namespace Tigon


#endif // SURROGATEBASEDOPTIMIZERWITHPERTURBATION_H
