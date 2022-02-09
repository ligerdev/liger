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
#pragma once
#include <tigon/Operators/Convergence/IConvergence.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class IRobustnessIndicator;
class RobustnessIndicatorFactory;
}
}

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RobustScalarisingSpace : public IConvergence
{
    HANDLE_READ_PROPERTIES_BEGIN(IConvergence)
    READ(neighbourhoodRadius, TP_neighbourhoodRadius)
    READ(ReferenceSetSize, TP_referenceSetSizeRobustScalarisingSpace)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IConvergence)
    WRITE(neighbourhoodRadius, double, TP_defineNeighbourhoodRadius)
    WRITE(ReferenceSetSize, int, TP_defineReferenceSetSizeRobustScalarisingSpace)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::RobustScalarisingSpace)

public:
    RobustScalarisingSpace();
    RobustScalarisingSpace(Tigon::Representation::IPSet* ipset);
    ~RobustScalarisingSpace();

    // Properties
    double TP_neighbourhoodRadius()       const;
    void TP_defineNeighbourhoodRadius(double r);
    void TP_defineReferenceSetSizeRobustScalarisingSpace(int s);
    int  TP_referenceSetSizeRobustScalarisingSpace()          const;

    void defineIndicator(const TString &indicatorType, double indParam);

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();
    void createReferenceSet();
    TVector<double>                m_ideal;
    TVector<double>            m_antiIdeal;
    TVector<TVector<double> > m_initRefSet;
    double           m_neighbourhoodRadius;
    int                       m_refSetSize;

    IRobustnessIndicator*      m_indicator;
    TString                m_indicatorType;
    double            m_indicatorParameter;
    RobustnessIndicatorFactory*  m_factory;

};

} // namespace Operators
} // namespace Tigon
