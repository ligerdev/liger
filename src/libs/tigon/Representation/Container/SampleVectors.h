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
#ifndef SAMPLEVECTORS_H
#define SAMPLEVECTORS_H

#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

typedef TVector<double> RealVector;

class LIGER_TIGON_EXPORT SampleVectors
{
public:
    SampleVectors();
    ~SampleVectors();

    void addSample(const RealVector &vector);
    bool empty() const;
    int nSamples() const;
    RealVector sample() const;
    RealVector vector(int pos) const;

private:
    TVector<RealVector> m_samples;

    // m_samples = [[70 60 40 30 10],[65 40 30 20 10],...]
};

} // namespace Representation
} // namespace Tigon

#endif // SAMPLEVECTORS_H
