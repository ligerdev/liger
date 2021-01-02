/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef TIMESERIESCONTAINER_H
#define TIMESERIESCONTAINER_H

#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT TimeSeriesContainer
{
public:
    TimeSeriesContainer();
    ~TimeSeriesContainer();

    void addData(SampleVectorsSPtr data);

    int sampleIdx() const;
    SampleVectorsSPtr sampledVal(size_t run_idx);

    bool empty() const;
    int nRuns() const;
    void assignAlpha(double val);

private:

    void updateCDF();
    TVector<int> getRunKeys() const;

    TMap<int,SampleVectorsSPtr> m_data;
    int m_run;
    double m_alpha;
    TVector<int> m_runKeys;
    TVector<double> m_CDF;

    // m_data = {1,[[70 60 40 30 10],[65 40 30 20 10]]},
    //          {2,[[70 63 46 32 12],[64 47 43 27 11]]}
};

} // namespace Representation
} // namespace Tigon

#endif // TIMESERIESCONTAINER_H
