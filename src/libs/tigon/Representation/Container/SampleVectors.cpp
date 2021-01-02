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
#include <tigon/Representation/Container/SampleVectors.h>
#include <tigon/tigonconstants.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Representation {


SampleVectors::SampleVectors()
{

}

SampleVectors::~SampleVectors()
{

}

void SampleVectors::addSample(const RealVector &vector)
{
    // if there is no vectors yet
    if(m_samples.empty()) {
        m_samples.push_back(vector);
        return;
    }

    // vectors need to have the same sime
    if(vector.size()!=m_samples[0].size()) {
        return;
    }

    // do not add a vector if it exists already
    for(auto elem: m_samples) {

        bool flag = true; // assume two vecs are the same

        for(size_t i=0; i<elem.size(); i++) {
            if(!areDoublesEqual(elem[i],vector[i])) {
                flag=false;
                break;
            }
        }

        if(flag) {
            return;
        }
    }

    m_samples.push_back(vector);
}

bool SampleVectors::empty() const
{
    return m_samples.empty();
}

int SampleVectors::nSamples() const
{
    return m_samples.size();
}

RealVector SampleVectors::sample() const
{
    // select a sample randomly according to an
    // uniformly distribution
    int nSamples = this->nSamples();
    int R = TRAND.randInt(nSamples);
    return m_samples[R];
}

RealVector SampleVectors::vector(int pos) const
{
    if(pos>=m_samples.size()) {
        throw TException("Tigon::Representation::Container::SampleVectors",
                         DomainException);
    }

    return m_samples[pos];
}

} // namespace Representation
} // namespace Tigon
