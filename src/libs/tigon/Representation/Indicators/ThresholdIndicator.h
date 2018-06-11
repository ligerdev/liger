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
#ifndef THRESHOLDINDICATOR_H
#define THRESHOLDINDICATOR_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Indicators/IRobustnessIndicator.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ThresholdIndicator : public IRobustnessIndicator
{
public:
    ThresholdIndicator();
    ThresholdIndicator(double q);
    ~ThresholdIndicator();

    double evaluate(IDistribution* dist) const;

    void  defineThreshold(double q);
    double threshold()        const;

private:
    double m_threshold;
};

} // namespace Representation
} // namespace Tigon

#endif // THRESHOLDINDICATOR_H
