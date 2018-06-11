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
#ifndef NOMINALPERTURPATION_H
#define NOMINALPERTURPATION_H
#include <tigon/Operators/Perturbations/IPerturbation.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT CategoricalPerturpation : public IPerturbation
{    
    DECLARE_CLASS(Tigon::Operators::CategoricalPerturpation)

public:
    CategoricalPerturpation();
    CategoricalPerturpation(Tigon::Representation::IPSet* ipset);
    virtual ~CategoricalPerturpation();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

private:
    void initialise();
};

} // namespace Operators
} // namespace Tigon

#endif // NOMINALPERTURPATION_H
