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
#ifndef INTEGERMUTATION_H
#define INTEGERMUTATION_H
#include <tigon/Operators/Perturbations/IPerturbation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IntegerMutation : public IPerturbation
{
    HANDLE_READ_PROPERTIES_BEGIN(IPerturbation)
    READ(StepSize, TP_stepSize)
    READ(WidthPercent, TP_widthPercent)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IPerturbation)
    WRITE(StepSize, double, TP_defineStepSize)
    WRITE(WidthPercent, double, TP_defineWidthPercent)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IntegerMutation)

public:
    IntegerMutation();
    IntegerMutation(Tigon::Representation::IPSet* ipset);
    ~IntegerMutation();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    /**************/
    /* Properties */
    /**************/

    double TP_stepSize() const;
    void TP_defineStepSize(double s);

    double TP_widthPercent() const;
    void TP_defineWidthPercent(double wp);

private:
    void initialise();

    double m_s; // step size
    double m_wp;
};

} // namespace Operators
} // namespace Tigon

#endif // INTEGERMUTATION_H
