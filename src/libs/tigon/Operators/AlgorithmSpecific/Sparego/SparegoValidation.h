/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef SPAREGOVALIDATION_H
#define SPAREGOVALIDATION_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Evaluators/IEvaluator.h>

namespace Tigon {

namespace Representation {
class IDistribution;
}

namespace Operators {
using namespace Tigon::Representation;

class LIGER_TIGON_EXPORT SparegoValidation : public IEvaluator
{
    HANDLE_READ_PROPERTIES_BEGIN(IEvaluator)
    READ(NeighbourhoodRadius, TP_neighbourhoodRadius)
    READ(Percentile, TP_percentile)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IEvaluator)
    WRITE(NeighbourhoodRadius, double, TP_defineNeighbourhoodRadius)
    WRITE(Percentile, double, TP_definePercentile)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::SparegoValidation)

public:
    SparegoValidation();
    SparegoValidation(Tigon::Representation::IPSet *ipset);
    ~SparegoValidation();

    // Properties
    double TP_neighbourhoodRadius()        const;
    void  TP_defineNeighbourhoodRadius(double r);

    ConfidenceInSampleType TP_confidenceInSample()       const;
    void TP_defineConfidenceInSample(ConfidenceInSampleType t);

    double TP_percentile()        const;
    void  TP_definePercentile(double p);

    void evaluateNode();

    double        sampleSize(int idx)   const;
    TVector<int> neighboursOf(int idx) const;


    // Information about the node.
    TString     name();
    TString     description();

private:
    void  initialise();

    IDistribution* modifyDistParams(double nomMean, double nomStd,
                                      double sampSize) const;

    ConfidenceInSampleType          m_confType;
    double                           m_neighbourhoodRadius;
    double                           m_percentile;
    TVector<TVector<int> >          m_neighbours;
    TVector<TVector<double> >        m_weights;
    TVector<double>                  m_sampleSizes;
};

} // namespace Operators
} // namespace Tigon

#endif /* SPAREGOVALIDATION_H */
