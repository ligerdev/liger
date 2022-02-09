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
#ifndef ACROMUSEFiltrationFORDIRECTION_H
#define ACROMUSEFiltrationFORDIRECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT AcromuseFiltrationForDirection : public IFiltration
{

public:
    AcromuseFiltrationForDirection();
    AcromuseFiltrationForDirection(Tigon::Representation::IPSet* ipset);
    virtual ~AcromuseFiltrationForDirection();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Information on adaptive parameters
    double ratioOfNewSolutions()  const;
    int   outputSetSize()        const;
    int   tournamentSize()       const;

private:
    void  defineRatioOfNewSolutions(double r);
    void  defineOutputSetSize(int         n);
    void  defineTournamentSize(int        s);

    int tournament(ISet* set) const;

    double   m_ratioOfNewSolutions;
    int     m_outputSetSize;
    int     m_tournamentSize;
    TString m_name;
    TString m_description;

};

} // namespace Operators
} // namespace Tigon

#endif // ACROMUSEFiltrationFORDIRECTION_H
