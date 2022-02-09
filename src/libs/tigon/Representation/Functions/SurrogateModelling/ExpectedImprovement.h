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
#ifndef EXPECTEDIMPROVEMENT_H
#define EXPECTEDIMPROVEMENT_H

#include <tigon/Representation/Functions/SurrogateModelling/KrigingSurrogate.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class NormalDistribution;

class LIGER_TIGON_EXPORT ExpectedImprovement : public KrigingSurrogate
{

public:
    ExpectedImprovement();
    ExpectedImprovement(KrigingSPtr model);
    ~ExpectedImprovement();

    // Information about the function(s)
    TString name();
    TString description();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

    void defineModel(KrigingSPtr model);

    double best()  const;
    double worst() const;

    virtual double value(const TVector<IElementSPtr>& inputs);
    virtual double error(const TVector<IElementSPtr>& inputs);

protected:
    void defineBest(double best);
    void defineWorst(double worst);

    void  updateBest();

    NormalDistribution* m_standardNormal;

private:
    double m_best;
    double m_worst;
    void  defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // EXPECTEDIMPROVEMENT_H
