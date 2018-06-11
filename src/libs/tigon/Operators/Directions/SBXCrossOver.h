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
#ifndef SBXCROSSOVER_H
#define SBXCROSSOVER_H
#include <tigon/Operators/Directions/IDirection.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SBXCrossOver : public IDirection
{

    HANDLE_READ_PROPERTIES_BEGIN(IDirection)
    READ(CrossoverDistributionIndex, TP_crossoverDistributionIndex)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IDirection)
    WRITE(CrossoverDistributionIndex, double, TP_defineCrossoverDistributionIndex)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::SBXCrossOver)

public:
    SBXCrossOver();
    SBXCrossOver(Tigon::Representation::IPSet* ipset);
    ~SBXCrossOver();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    /**************/
    /* Properties */
    /**************/

    // Distribution Index
    void  TP_defineCrossoverDistributionIndex(double  eta);
    double TP_crossoverDistributionIndex()           const;

private:
    void initialise();

    double   m_eta;
};

} // namespace Operators
} // namespace Tigon

#endif // SBXCROSSOVER_H
