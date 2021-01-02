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
#ifndef HYPERVOLUME_H
#define HYPERVOLUME_H
#include <tigon/Operators/Convergence/IConvergence.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class IRobustnessIndicator;
class RobustnessIndicatorFactory;
}
}

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT Hypervolume : public IConvergence
{
    
public:
    Hypervolume();
    Hypervolume(Tigon::Representation::IPSet* ipset);
    virtual ~Hypervolume();

    void defineReferencePoint(TVector<double> refPoint);
    TVector<double> referencePoint()              const;

    void evaluateNode();


    // Information about the node.
    TString     name();
    TString     description();
    
    

private:
    void initialise();
    bool                        m_isDefinedRef;
    TVector<double>              m_reference;
};

} // namespace Operators
} // namespace Tigon

#endif // HYPERVOLUME_H
