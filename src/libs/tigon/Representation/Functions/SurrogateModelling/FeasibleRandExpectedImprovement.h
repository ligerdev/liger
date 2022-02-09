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
#ifndef FEASIBLERANDEXPECTEDIMPROVEMENT_H
#define FEASIBLERANDEXPECTEDIMPROVEMENT_H

#include <tigon/Representation/Functions/SurrogateModelling/RandExpectedImprovement.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT FeasibleRandExpectedImprovement :
        public RandExpectedImprovement
{

public:
    FeasibleRandExpectedImprovement();
    FeasibleRandExpectedImprovement(KrigingSPtr model);
    FeasibleRandExpectedImprovement(KrigingSPtr model,
                                    const TVector<TVector<double>> &samples);
    FeasibleRandExpectedImprovement(KrigingSPtr model,
                                    const TVector<TVector<double>> &samples,
                                    const TVector<double>& violation);

    ~FeasibleRandExpectedImprovement();

    // Information about the function(s)
    TString name();
    TString description();

private:
    void defineModelAndViolation(KrigingSPtr model,
                                 const TVector<double>& violation);
    void defineViolation(const TVector<double>& violation);
    void updateBest();
    void defineOutputPrpts();

    TVector<double> m_violation;
};

} // namespace Representation
} // namespace Tigon

#endif // FEASIBLERANDEXPECTEDIMPROVEMENT_H
