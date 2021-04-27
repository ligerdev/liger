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
#ifndef SWAPMUTATION_H
#define SWAPMUTATION_H
#include <tigon/Operators/Perturbations/IPerturbation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SwapMutation : public IPerturbation
{
    DECLARE_CLASS(Tigon::Operators::SwapMutation)
public:
    SwapMutation();
    SwapMutation(Tigon::Representation::IPSet* ipset);
    virtual ~SwapMutation();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

private:
    void initialise();

};

} // namespace Operators
} // namespace Tigon


#endif // SWAPMUTATION_H
