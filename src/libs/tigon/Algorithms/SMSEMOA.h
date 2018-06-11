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
#ifndef SMSEMOA_H
#define SMSEMOA_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT SMSEMOA : public IAlgorithm
{
    DECLARE_CLASS(Tigon::Algorithms::SMSEMOA)

public:
        SMSEMOA();
        SMSEMOA(Tigon::Representation::IPSet* ipset);
        ~SMSEMOA();

        // Information about the node.
        TString name();
        TString description();

private:
        void initialise();
};

} // namespace Algorithms
} // namespace Tigon

#endif // SMSEMOA_H
