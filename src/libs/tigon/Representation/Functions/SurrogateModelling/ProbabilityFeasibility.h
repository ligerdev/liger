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
#ifndef PROBABILITYOFFEASIBILITY_H
#define PROBABILITYOFFEASIBILITY_H

#include <tigon/Representation/Functions/SurrogateModelling/KrigingSurrogate.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class NormalDistribution;

class LIGER_TIGON_EXPORT ProbabilityFeasibility : public KrigingSurrogate
{

public:
    ProbabilityFeasibility();
    ProbabilityFeasibility(const KrigingSPtr &model);
    ~ProbabilityFeasibility();

    // Information about the function(s)
    TString name();
    TString description();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

    void defineModel(KrigingSPtr model);

    virtual double value(const TVector<IElementSPtr>& inputs);
    virtual double error(const TVector<IElementSPtr>& inputs);

protected:
    NormalDistribution* m_standardNormal;

private:
    void  defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // PROBABILITYOFFEASIBILITY_H
