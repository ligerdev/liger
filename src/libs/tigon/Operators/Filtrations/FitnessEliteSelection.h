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
#ifndef FITNESSELITESELECTION_H
#define FITNESSELITESELECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT FitnessEliteSelection : public IFiltration
{   
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(EliteRatio, TP_eliteRatio)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(EliteRatio, double, TP_defineEliteRatio)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::FitnessEliteSelection)

public:
    FitnessEliteSelection();
    FitnessEliteSelection(Tigon::Representation::IPSet* ipset);
    virtual ~FitnessEliteSelection();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Properties
    double TP_eliteRatio()        const;
    void  TP_defineEliteRatio(double r);

private:
    TString        m_name;
    TString        m_description;
    double          m_eliteRatio;
};

} // namespace Operators
} // namespace Tigon

#endif // FITNESSELITESELECTION_H
