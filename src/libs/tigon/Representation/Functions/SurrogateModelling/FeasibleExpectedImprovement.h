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
#ifndef FEASIBLEEXPECTEDIMPROVEMENT_H
#define FEASIBLEEXPECTEDIMPROVEMENT_H
#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT FeasibleExpectedImprovement : public ExpectedImprovement
{
public:
    FeasibleExpectedImprovement();
    FeasibleExpectedImprovement(KrigingSPtr model);
    FeasibleExpectedImprovement(KrigingSPtr model,
                                const TVector<double>& violation);
    ~FeasibleExpectedImprovement();

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

#endif // FEASIBLEEXPECTEDIMPROVEMENT_H
