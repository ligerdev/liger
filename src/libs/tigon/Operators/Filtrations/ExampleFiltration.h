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
#ifndef EXAMPLEFILTRATION_H
#define EXAMPLEFILTRATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT ExampleFiltration : public IFiltration
{

public:
    ExampleFiltration();
    ExampleFiltration(Tigon::Representation::IPSet* ipset);
    virtual ~ExampleFiltration();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    //Properties
    int  TP_clusterSize()   const;
    void TP_defineClusterSize(int s);

private:
    TString m_name;
    TString m_description;
    int     m_clusterSize;

};

} // namespace Operators
} // namespace Tigon

#endif // EXAMPLEFILTRATION_H
