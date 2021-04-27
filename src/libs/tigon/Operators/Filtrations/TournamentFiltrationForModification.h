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
#ifndef TOURNAMENTFILTRATIONFORMODIFICATION_H
#define TOURNAMENTFILTRATIONFORMODIFICATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT TournamentFiltrationForModification : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(ChildrenRatio, TP_childrenRatio)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(ChildrenRatio, double, TP_defineChildrenRatio)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::TournamentFiltrationForModification)

public:
    TournamentFiltrationForModification();
    TournamentFiltrationForModification(Tigon::Representation::IPSet* ipset);
    virtual ~TournamentFiltrationForModification();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    // Properties
    void  TP_defineChildrenRatio(double r);
    double TP_childrenRatio()        const;

private:
    double m_childrenRatio;
};

} // namespace Operators
} // namespace Tigon

#endif // TOURNAMENTFILTRATIONFORMODIFICATION_H
