/****************************************************************************
**
** Copyright (C) 2012-2020 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef RANDEXPECTEDIMPROVEMENT_H
#define RANDEXPECTEDIMPROVEMENT_H

#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT RandExpectedImprovement : public ExpectedImprovement
{

public:
    RandExpectedImprovement();
    RandExpectedImprovement(KrigingSPtr model);
    RandExpectedImprovement(KrigingSPtr model,
                            const TVector<TVector<double>> &samples);
    ~RandExpectedImprovement();

    void defineSamples(const TVector<TVector<double>> &samples);
    TVector<TVector<double>> samples() const;

    void  defineBandwidth(double h);
    double bandwidth()       const;

    // Information about the function(s)
    TString name();
    TString description();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

    virtual double value(const TVector<IElementSPtr>& inputs);
    virtual double error(const TVector<IElementSPtr>& inputs);

private:
    void defineOutputPrpts();
    void defineDefaultBandwidth();
    TVector<TVector<double>> m_samples;
    double                   m_bandwidth;
};

} // namespace Representation
} // namespace Tigon

#endif // RANDEXPECTEDIMPROVEMENT_H
