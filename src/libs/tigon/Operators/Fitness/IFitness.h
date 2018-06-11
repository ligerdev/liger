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
#ifndef IFITNESS_H
#define IFITNESS_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>


namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IFitness : public IOperator
{   

public:
    IFitness();
    IFitness(Representation::IPSet* ipset);
    virtual ~IFitness();

    virtual void evaluateNode()  = 0;

    // Information about the node.
    virtual TString     name()          = 0;
    virtual TString     description()   = 0;

private:
    void initialise();
};

} // namespace Operators
} // namespace Tigon

#endif // IFITNESS_H
