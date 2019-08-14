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
#ifndef SHAREDFITNESS_H
#define SHAREDFITNESS_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SharedFitness : public IFitness
{
    HANDLE_READ_PROPERTIES_BEGIN(IFitness)
    READ(SharingFunctionAlpha, TP_sharingFunctionAlpha)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFitness)
    WRITE(SharingFunctionAlpha, double, TP_defineSharingFunctionAlpha)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::SharedFitness)

public:
    SharedFitness();
    SharedFitness(Tigon::Representation::IPSet* ipset);
    ~SharedFitness();

    void evaluateNode();

    // Properties
    double TP_sharingFunctionAlpha()           const;
    void TP_defineSharingFunctionAlpha(double alpha);

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();

    /**
     * @brief sigmaShareEstimation: Uses the Bisection method to estimate the
     * value of \sigma_{share}. The function given to the Bisection method is
     * taken from Fonseca and Fleming (1993). The estimated \sigma_{share} is
     * based on the principle that the Pareto front is divided into equally
     * spaced regions.
     * @param N Number of solutions
     * @param M Number of objectives
     * @return \sigma_{share}
     */
    static double sigmaShareEstimation(int N, int M);

    double m_alpha;
};

} // namespace Operators
} // namespace Tigon

#endif // SHAREDFITNESS_H
