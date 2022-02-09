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
#ifndef ICOMPOSITE_H
#define ICOMPOSITE_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>


namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IComposite : public IOperator
{
    DECLARE_CLASS(Tigon::Operators::IComposite)

public:
    IComposite();
    IComposite(Tigon::Representation::IPSet* ipset);
    virtual ~IComposite();

    virtual void evaluateNode()  = 0;

    // Information about the node.
    virtual TString     name()                 = 0;
    virtual TString     description()          = 0;


private:

};

} // namespace Operators
} // namespace Tigon

#endif // ICOMPOSITE_H
