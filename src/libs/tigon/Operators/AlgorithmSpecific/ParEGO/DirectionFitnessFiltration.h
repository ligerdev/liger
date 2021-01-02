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
#ifndef DIRECTIONFITNESSFILTRATION_H
#define DIRECTIONFITNESSFILTRATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT DirectionFitnessFiltration : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(MaxSolutions, TP_maxSolutions)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(MaxSolutions, int, TP_defineMaxSolutions)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::DirectionFitnessFiltration)

public:
    DirectionFitnessFiltration();
    DirectionFitnessFiltration(Tigon::Representation::IPSet* ipset);
    virtual ~DirectionFitnessFiltration();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Properties
    void  TP_defineMaxSolutions(int n);
    int   TP_maxSolutions()      const;

protected:
    TVector<int> selection(ISet* set, int maxSolutions);

private:
    void initialise();
    int  m_maxSolutions;
};

} // namespace Operators
} // namespace Tigon

#endif // DIRECTIONFITNESSFILTRATION_H
