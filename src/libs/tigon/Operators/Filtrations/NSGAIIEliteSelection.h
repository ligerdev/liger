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
#ifndef NSGAIIELITESELECTION_H
#define NSGAIIELITESELECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT NSGAIIEliteSelection : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(EliteRatio, TP_eliteRatio)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(EliteRatio, double, TP_defineEliteRatio)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::NSGAIIEliteSelection)

public:
    NSGAIIEliteSelection();
    NSGAIIEliteSelection(Tigon::Representation::IPSet* ipset);
    ~NSGAIIEliteSelection();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    // Properties
    double TP_eliteRatio()        const;
    void   TP_defineEliteRatio(double r);

private:
    void initialise();

    double m_eliteRatio;
};

} // namespace Operators
} // namespace Tigon

#endif // NSGAIIELITESELECTION_H
