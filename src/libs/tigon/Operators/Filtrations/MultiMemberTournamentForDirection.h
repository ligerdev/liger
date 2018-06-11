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
#ifndef MULTIMEMBERTOURNAMENTFORDIRECTION_H
#define MULTIMEMBERTOURNAMENTFORDIRECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT MultiMemberTournamentForDirection : public IFiltration
{

    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(RatioOfNewSolutions, TP_ratioOfNewSolutions)
    READ(OutputSetSize, TP_outputSetSize)
    READ(TournamentSize, TP_tournamentSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(RatioOfNewSolutions, double, TP_defineRatioOfNewSolutions)
    WRITE(OutputSetSize, int, TP_defineOutputSetSize)
    WRITE(TournamentSize, int, TP_defineTournamentSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::MultiMemberTournamentForDirection)

public:
    MultiMemberTournamentForDirection();
    MultiMemberTournamentForDirection(Tigon::Representation::IPSet* ipset);
    virtual ~MultiMemberTournamentForDirection();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    // Properties
    void  TP_defineRatioOfNewSolutions(double    r);
    double TP_ratioOfNewSolutions()           const;
    void  TP_defineOutputSetSize(int            n);
    int   TP_outputSetSize()                 const;
    void  TP_defineTournamentSize(int           s);
    int   TP_tournamentSize()                const;

private:
    double   m_ratioOfNewSolutions;
    int     m_outputSetSize;
    int     m_tournamentSize;
    TString m_name;
    TString m_description;

};

} // namespace Operators
} // namespace Tigon

#endif // MULTIMEMBERTOURNAMENTFORDIRECTION_H
