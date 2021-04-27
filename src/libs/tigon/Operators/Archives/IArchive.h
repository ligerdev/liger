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
#ifndef IARCHIVE_H
#define IARCHIVE_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IArchive : public IOperator
{

public:
    IArchive();
    IArchive(Tigon::Representation::IPSet* ipset);
    virtual ~IArchive();

    virtual void evaluateNode()  = 0;

    // Information about the node.
    virtual TString     name()          = 0;
    virtual TString     description()   = 0;

private:


};

} // namespace Operators
} // namespace Tigon

#endif // IARCHIVE_H
