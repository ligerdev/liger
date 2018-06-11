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
#ifndef ROBUSTNESSASSIGNMENT_H
#define ROBUSTNESSASSIGNMENT_H
#include <tigon/Operators/Fitness/Scalarization.h>

namespace Tigon {
namespace Representation {
class IRobustnessIndicator;
class RobustnessIndicatorFactory;
class IDistribution;
}
}

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RobustnessAssignment : public Scalarization
{
    HANDLE_READ_PROPERTIES_BEGIN(IFitness)
    READ(IndicatorType, TP_indicatorType)
    READ(IndicatorParameter, TP_indicatorParameter)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFitness)
    WRITE(IndicatorType, TString, TP_defineIndicatorType)
    WRITE(IndicatorParameter, double, TP_defineIndicatorParameter)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::RobustnessAssignment)

public:
    RobustnessAssignment();
    RobustnessAssignment(Tigon::Representation::IPSet* ipset);
    virtual ~RobustnessAssignment();

    void evaluateNode();

    // Properties
    TString TP_indicatorType()                 const;
    void    TP_defineIndicatorType(const TString &t);
    double   TP_indicatorParameter()            const;
    void    TP_defineIndicatorParameter(double     p);

    // Information about the node.
    TString     name();
    TString     description();
    
    

    void defineIndicator(TString indicatorType, double indParam=0);

protected:
    double robustnessOf(IDistribution* dist) const;

private:
    void initialise();

    IRobustnessIndicator* m_indicator;
    TString               m_indicatorType;
    double                 m_indicatorParameter;
    RobustnessIndicatorFactory* m_factory;
};

} // namespace Operators
} // namespace Tigon

#endif // ROBUSTNESSASSIGNMENT_H
