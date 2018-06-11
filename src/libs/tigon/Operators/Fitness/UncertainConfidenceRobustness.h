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
#ifndef UNCERTAINCONFIDENCEROBUSTNESS_H
#define UNCERTAINCONFIDENCEROBUSTNESS_H
#include <tigon/Operators/Fitness/RobustnessAssignment.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT UncertainConfidenceRobustness : public RobustnessAssignment
{
    HANDLE_READ_PROPERTIES_BEGIN(RobustnessAssignment)
    READ(Percentile, TP_percentile)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(RobustnessAssignment)
    WRITE(Percentile, double, TP_definePercentile)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::UncertainConfidenceRobustness)

public:
    UncertainConfidenceRobustness();
    UncertainConfidenceRobustness(Tigon::Representation::IPSet* ipset);
    virtual ~UncertainConfidenceRobustness();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    double TP_percentile()        const;
    void  TP_definePercentile(double p);

private:
    void initialise();

    TString m_name;
    TString m_description;
    double   m_percentile;
};

} // namespace Operators
} // namespace Tigon

#endif // UNCERTAINCONFIDENCEROBUSTNESS_H
