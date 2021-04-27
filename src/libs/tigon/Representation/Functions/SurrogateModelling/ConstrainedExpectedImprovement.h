/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef CONSTRAINEDEXPECTEDIMPROVEMENT_H
#define CONSTRAINEDEXPECTEDIMPROVEMENT_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Functions/SurrogateModelling/ExpectedImprovement.h>
#include <tigon/Representation/Functions/SurrogateModelling/ProbabilityFeasibility.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ConstrainedExpectedImprovement : public IFunction
{
public:
    ConstrainedExpectedImprovement();
    ConstrainedExpectedImprovement(ExpectedImprovement* ei,
                 const TVector<ProbabilityFeasibility*>& vpf);
    ~ConstrainedExpectedImprovement();

    // Information about the function(s)
    TString name();
    TString description();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

    void defineExpectedImprovement(ExpectedImprovement* ei);
    void defineProbabilityFeasibility(const TVector<ProbabilityFeasibility*>& vpf);


protected:
    void defineInputPrpts();
    void defineOutputPrpts();

private:
    ExpectedImprovement* m_ei;
    TVector<ProbabilityFeasibility*> m_vpf;
};

} // namespace Representation
} // namespace Tigon

#endif // CONSTRAINEDEXPECTEDIMPROVEMENT_H
