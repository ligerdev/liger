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
#ifndef UNCERTAINROBUSTNESS_H
#define UNCERTAINROBUSTNESS_H
#include <tigon/Operators/Fitness/RobustnessAssignment.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT UncertainMonteCarloRobustness
        : public RobustnessAssignment
{
    DECLARE_CLASS(Tigon::Operators::RobustnessAssignment)
public:
    UncertainMonteCarloRobustness();
    UncertainMonteCarloRobustness(Tigon::Representation::IPSet* ipset);
    virtual ~UncertainMonteCarloRobustness();

   void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    double TP_percentile()        const;
    void  TP_definePercentile(double p);
    int   TP_sampleSize()        const;
    void  TP_defineSampleSize(int   n);

private:
    void initialise();

    TString m_name;
    TString m_description;
    double   m_percentile;
    int     m_sampleSize;
};

} // namespace Operators
} // namespace Tigon

#endif // UNCERTAINROBUSTNESS_H
