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
#ifndef CONSTRAINTDIRECTIONFITNESSFILTRATION_H
#define CONSTRAINTDIRECTIONFITNESSFILTRATION_H
#include <tigon/Operators/AlgorithmSpecific/ParEGO/DirectionFitnessFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT ConstraintDirectionFitnessFiltration :
        public DirectionFitnessFiltration
{
public:
    ConstraintDirectionFitnessFiltration();
    ConstraintDirectionFitnessFiltration(Tigon::Representation::IPSet* ipset);
    ~ConstraintDirectionFitnessFiltration();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

private:
    TVector<int> sortViolation(ISet* set, const TVector<double>& threshVec);
    void initialise();
};

} // namespace Operators
} // namespace Tigon

#endif // CONSTRAINTDIRECTIONFITNESSFILTRATION_H
