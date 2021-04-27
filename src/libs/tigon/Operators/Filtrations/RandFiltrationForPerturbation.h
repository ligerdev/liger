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
#ifndef RANDFILTRATIONFORPERTURBATION_H
#define RANDFILTRATIONFORPERTURBATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RandFiltrationForPerturbation : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(RatioOfNewSolutions, TP_ratioOfNewSolutions)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(RatioOfNewSolutions, double, TP_defineRatioOfNewSolutions)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::RandFiltrationForPerturbation)

public:
    RandFiltrationForPerturbation();
    RandFiltrationForPerturbation(Tigon::Representation::IPSet* ipset);
    virtual ~RandFiltrationForPerturbation();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    // Properties
    void  TP_defineRatioOfNewSolutions(double r);
    double TP_ratioOfNewSolutions()        const;

private:
    double   m_ratioOfSolutions;
    TString m_name;
    TString m_description;
};

} // namespace Operators
} // namespace Tigon

#endif // RANDFILTRATIONFORPERTURBATION_H
