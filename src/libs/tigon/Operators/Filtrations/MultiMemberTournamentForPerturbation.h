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
#ifndef MULTIMEMBERTOURNAMENTFORPERTURBATION_H
#define MULTIMEMBERTOURNAMENTFORPERTURBATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT MultiMemberTournamentForPerturbation : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(RatioOfNewSolutions, TP_ratioOfNewSolutions)
    READ(TournamentSize, TP_tournamentSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(RatioOfNewSolutions, double, TP_defineRatioOfNewSolutions)
    WRITE(TournamentSize, int, TP_defineTournamentSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::MultiMemberTournamentForPerturbation)

public:
    MultiMemberTournamentForPerturbation();
    MultiMemberTournamentForPerturbation(Tigon::Representation::IPSet* ipset);
    virtual ~MultiMemberTournamentForPerturbation();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    // Properties
    void  TP_defineRatioOfNewSolutions(double r);
    double TP_ratioOfNewSolutions()        const;
    void  TP_defineTournamentSize(int        s);
    int   TP_tournamentSize()             const;

private:
    void initialise();
    double   m_ratioOfNewSolutions;
    int     m_tournamentSize;
    TString m_name;
    TString m_description;
};

} // namespace Operators
} // namespace Tigon

#endif // MULTIMEMBERTOURNAMENTFORPERTURBATION_H
