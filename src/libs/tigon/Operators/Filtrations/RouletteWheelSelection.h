/****************************************************************************
**
** Copyright (C) 2012-2019 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef ROULETTEWHEELSELECTION_H
#define ROULETTEWHEELSELECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RouletteWheelSelection : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(OutputSetSize, TP_outputSetSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(OutputSetSize, int, TP_defineOutputSetSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::RouletteWheelSelection)

public:
    RouletteWheelSelection();
    RouletteWheelSelection(Tigon::Representation::IPSet* ipset);
    ~RouletteWheelSelection();

    void evaluateNode();

    // Properties
    void TP_defineOutputSetSize(int n);
    int  TP_outputSetSize() const;

    // Information about the node.
    TString     name();
    TString     description();

protected:
    void initialise();
    static TVector<double> cumulativeProbabilityDistribution(
            const TVector<double> &fitness);

private:
    int     m_outputSetSize;
};

} // namespace Operators
} // namespace Tigon

#endif // ROULETTEWHEELSELECTION_H
