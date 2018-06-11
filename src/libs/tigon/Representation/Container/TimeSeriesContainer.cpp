/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Container/TimeSeriesContainer.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Representation/Container/SampleVectors.h>

namespace Tigon {
namespace Representation {

TimeSeriesContainer::TimeSeriesContainer()
    : m_run(1)
    , m_alpha(0.7)
{

}

TimeSeriesContainer::~TimeSeriesContainer()
{
    m_data.clear();
}

void TimeSeriesContainer::addData(SampleVectorsSPtr data)
{
    // the key is determine by m_run (which is always unique)
    m_data[m_run]=data;

    // increment the run counter
    m_run++;

    // update the current set of keys for the map
    m_runKeys = this->getRunKeys();

    // update CDF
    this->updateCDF();
}

int TimeSeriesContainer::sampleIdx() const
{
    int nRuns = this->nRuns();

    // only one element to be sampled
    if(nRuns==1) {
        return 0;
        //return m_data.at(m_genKeys[0]);
    }

    // Generate a random number between 0 and 1
    double r = TRAND.randUni();

    // Select the generation according to the random number
    TVector<double>::const_iterator it = std::find_if(m_CDF.begin(), m_CDF.end(),
                      [r](double elem){ return (r<elem); });
    int runIndex = it - m_CDF.begin(); // the selected run

    //return m_data.at(m_genKeys[iteration]);
    return runIndex;
}

SampleVectorsSPtr TimeSeriesContainer::sampledVal(size_t run_idx)
{
    if(m_runKeys.empty()) {
        throw TException("Tigon::Representation::Container::TimeSeriesContainer",
                         NullPointerException);
    }

    if(run_idx>=m_runKeys.size()) {
        throw TException("Tigon::Representation::Container::TimeSeriesContainer",
                         DomainException);
    }

    return m_data.at(m_runKeys[run_idx]);
}

bool TimeSeriesContainer::empty() const
{
    return m_runKeys.empty();
}

int TimeSeriesContainer::nRuns() const
{
    return m_runKeys.size();
}

void TimeSeriesContainer::assignAlpha(double val)
{
    if((val>0.0) && (val<1.0)) {
        m_alpha=val;
    } else {
        m_alpha=1.0;
    }
}

void TimeSeriesContainer::updateCDF()
{
    int nRuns = this->nRuns();

    m_CDF.resize(nRuns);

    // generate a set of time preference weights
    TVector<double> s = timePreferenceWeights(nRuns, m_alpha);

    // calculate the sum
    double Sum = std::accumulate(s.begin(), s.end(), 0.0);

    // normalise by dividing by the sum
    std::transform(s.begin(), s.end(), s.begin(),
                   [Sum](double elem){ return elem/Sum; });

    // calculate the CDF
    std::partial_sum(s.begin(), s.end(), m_CDF.begin());
}

TVector<int> TimeSeriesContainer::getRunKeys() const
{
    TVector <int> res = getMapKeys(m_data);
    return res;
}

} // namespace Representation
} // namespace Tigon
