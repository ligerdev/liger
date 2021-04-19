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
#ifndef NSGAIIINICHING_H
#define NSGAIIINICHING_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT NSGAIIINiching : public IFitness
{
    DECLARE_CLASS(Tigon::Operators::NSGAIIINiching)

public:
    NSGAIIINiching();
    NSGAIIINiching(Tigon::Representation::IPSet* ipset);
    ~NSGAIIINiching();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Properties
    double TP_eliteRatio()        const;
    void  TP_defineEliteRatio(double r);

private:
    void initialise();
    double m_eliteRatio;

};

} // namespace Operators
} // namespace Tigon

#endif // NSGAIIINICHING_H
