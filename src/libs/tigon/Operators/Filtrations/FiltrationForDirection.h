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
#ifndef FILTRATIONFORDIRECTION_H
#define FILTRATIONFORDIRECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT FiltrationForDirection : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(OutputSetSize, TP_outputSetSize)
    READ(InputSetSize, TP_inputSetSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(OutputSetSize, int, TP_defineOutputSetSize)
    WRITE(InputSetSize, int, TP_defineInputSetSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::FiltrationForDirection)

public:
    FiltrationForDirection();
    FiltrationForDirection(Tigon::Representation::IPSet* ipset);
    virtual ~FiltrationForDirection();
    void    initialise();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    // Properties
    void  TP_defineOutputSetSize(int            n);
    int   TP_outputSetSize()                 const;

    void  TP_defineInputSetSize(int n);
    int   TP_inputSetSize()      const;

private:
    int     m_outputSetSize;
    TString m_name;
    TString m_description;

    int     m_inputSetSize;
};

} // namespace Operators
} // namespace Tigon

#endif // FILTRATIONFORDIRECTION_H
