/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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

#pragma once

#include <tigon/tigon_global.h>
#include <tigon/Representation/PSets/IPSet.h>

namespace Tigon {
namespace Representation {
class IPSetDecorator;
}
}

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT IPSetDecorator : public IPSet
{

public:
    IPSetDecorator();
    IPSetDecorator(Tigon::Representation::IPSet* ipset);
    virtual ~IPSetDecorator();

    void evaluate();
    void evaluateOnlyThisNode();

    // Information about the node.
    virtual TString  name()          = 0;
    virtual TString  description()   = 0;

protected:
    virtual void evaluateNode() = 0;
    virtual void updateSets()   = 0;
    virtual void initialise()   = 0;

private:
    void syncConfiguration();
    Tigon::Representation::IPSet* m_ipset;
};

} // namespace Representation
} // namespace Tigon
