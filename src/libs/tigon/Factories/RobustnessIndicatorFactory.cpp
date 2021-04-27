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
#include <tigon/Factories/RobustnessIndicatorFactory.h>
#include <tigon/Representation/Indicators/IRobustnessIndicator.h>
#include <tigon/Representation/Indicators/ConfidenceIndicator.h>
#include <tigon/Representation/Indicators/ThresholdIndicator.h>

namespace Tigon {
namespace Representation {

RobustnessIndicatorFactory& RobustnessIndicatorFactory::instance()
{
    static RobustnessIndicatorFactory singleFactory;
    return singleFactory;
}

IRobustnessIndicator *RobustnessIndicatorFactory::create(TString indicatorType,
                                                         double parameter) const
{
    IRobustnessIndicator* err(nullptr);

    if(indicatorType == Tigon::ConfidenceType) {
        if((parameter>=0.0) && (parameter<=1.0)) {
            return new ConfidenceIndicator(parameter);
        } else {
            // ERROR - invalid confidence level
            return err;
        }
    }
    else if(indicatorType == Tigon::ThresholdType) {
        return new ThresholdIndicator(parameter);
    }
    else {
        // ERROR - undefined indicator type
        return err;
    }
}

RobustnessIndicatorFactory::RobustnessIndicatorFactory()
{

}

RobustnessIndicatorFactory::~RobustnessIndicatorFactory()
{

}

}
}
