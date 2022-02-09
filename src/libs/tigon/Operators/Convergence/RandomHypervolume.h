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
#ifndef RANDOMHYPERVOLUME_H
#define RANDOMHYPERVOLUME_H
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

class LIGER_TIGON_EXPORT RandomHypervolume : public IConvergence
{
    HANDLE_READ_PROPERTIES_BEGIN(IConvergence)
    READ(IndicatorType, TP_indicatorType)
    READ(IndicatorParameter, TP_indicatorParameter)
    READ(NSamples, TP_nSamples)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IConvergence)
    WRITE(IndicatorType, TString, TP_defineIndicatorType)
    WRITE(IndicatorParameter, double, TP_defineIndicatorParameter)
    WRITE(NSamples, int, TP_defineNSamples)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::RandomHypervolume)

public:
    RandomHypervolume();
    RandomHypervolume(Tigon::Representation::IPSet* ipset);
    virtual ~RandomHypervolume();

    // Properties
    TString TP_indicatorType()                 const;
    void    TP_defineIndicatorType(const TString &t);
    double  TP_indicatorParameter()            const;
    void    TP_defineIndicatorParameter(double     p);
    int     TP_nSamples()                      const;
    void    TP_defineNSamples(int                 n);

    void defineIndicator(TString indicatorType, double indParam);

    void evaluateNode();

    void defineRefPoint(TVector<double> ref);
    TVector<double> ref()              const;

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();

    bool                        m_isDefinedRef;
    TVector<double>              m_reference;
    IRobustnessIndicator*       m_indicator;
    TString                     m_indicatorType;
    double                       m_indicatorParameter;
    int                         m_nSamples;
    RobustnessIndicatorFactory* m_factory;
};

} // namespace Operators
} // namespace Tigon

#endif // RANDOMHYPERVOLUME_H
