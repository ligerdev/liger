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
#ifndef OBJECTIVEREDUCTIONDATA_H
#define OBJECTIVEREDUCTIONDATA_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

struct CorrelatedObjectives {
    int objectiveIdx;
    double correlationStrength;
};

class LIGER_TIGON_EXPORT ObjectiveReductionData
{
public:
    ObjectiveReductionData(const TVector<bool>& set=TVector<bool>{true});
    ObjectiveReductionData(const TVector<bool>& set, const TVector<double>& err);
    ObjectiveReductionData(const TVector<bool>& set, const TVector<double>& err,
                           const TVector<double>& var);
    ObjectiveReductionData(const TVector<bool>& set, const TVector<double>& err,
                           const TVector<double>& var,
                           const TVector<double>& varPCA);
    ObjectiveReductionData(const TVector<bool>& set, const TVector<double>& err,
                           const TVector<double>& var,
                           const TVector<double>& varPCA,
                           const TMap<int, TVector<CorrelatedObjectives>>& corObjs);
    ObjectiveReductionData(const ObjectiveReductionData& data);
    ~ObjectiveReductionData();

    ObjectiveReductionData& operator=(const ObjectiveReductionData& other);

    ObjectiveReductionData* clone() const;

    TVector<bool>                            essentialObjectiveSet() const;
    TVector<double>                          error()                 const;
    TVector<double>                          variance()              const;
    TVector<double>                          variancePerPCA()        const;
    TMap<int, TVector<CorrelatedObjectives>> correlatedObjectives()  const;
    int                                      size()                  const;

    void defineEssentialObjectiveSet(const TVector<bool>& set);
    void defineError(const TVector<double>& error);
    void defineVariance(const TVector<double>& var);
    void defineVariancePerPCA(const TVector<double>& var);
    void defineCorrelatedObjectives(
            const TMap<int, TVector<CorrelatedObjectives>>& corObjs);

private:
    TVector<bool> m_essentialObjectiveSet;
    TVector<double> m_error;
    TVector<double> m_variance;
    TVector<double> m_variancePerPCA;
    TMap<int, TVector<CorrelatedObjectives>> m_correlatedObjectives;
};

} // namespace Representation
} // namespace Tigon

#endif // OBJECTIVEREDUCTIONDATA_H
