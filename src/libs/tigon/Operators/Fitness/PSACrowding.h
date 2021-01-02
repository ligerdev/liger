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
#ifndef PSACROWDING_H
#define PSACROWDING_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Operators {
class PSAClustering;

class LIGER_TIGON_EXPORT PSACrowding : public IFitness
{   
    DECLARE_CLASS(Tigon::Operators::PSACrowding)
public:
    PSACrowding();
    PSACrowding(Tigon::Representation::IPSet* ipset);
    virtual ~PSACrowding();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
private:
    void initialise();
    PSAClustering* m_cluster;
};

} // namespace Operators
} // namespace Tigon

#endif // PSACROWDING_H
