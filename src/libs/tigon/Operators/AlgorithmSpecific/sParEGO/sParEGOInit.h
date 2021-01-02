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
#ifndef SPAREGOINIT_H
#define SPAREGOINIT_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Initialisation/IInitialisation.h>


namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT sParEGOInit : public IInitialisation
{

    HANDLE_READ_PROPERTIES_BEGIN(IInitialisation)
    READ(PerturbedRatio, TP_perturbedRatio)
    READ(NeighbourhoodRadius, TP_neighbourhoodRadius)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IInitialisation)
    WRITE(PerturbedRatio, double, TP_definePerturbedRatio)
    WRITE(NeighbourhoodRadius, double, TP_defineNeighbourhoodRadius)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::sParEGOInit)

public:
    sParEGOInit();
    sParEGOInit(Representation::IPSet* ipset);
    virtual ~sParEGOInit();


    // Properties
    void  TP_definePerturbedRatio(double ratio);
    double TP_perturbedRatio()            const;
    void  TP_defineNeighbourhoodRadius(double r);
    double TP_neighbourhoodRadius()        const;

    void evaluateNode();

    // Information about the node
    TString     name();
    TString     description();

private:
    void initialise();

    double   m_perturbRatio;
    double   m_neighbourhoodRadius;
};

} // namespace Operators
} // namespace Tigon

#endif // SPAREGOINIT_H
