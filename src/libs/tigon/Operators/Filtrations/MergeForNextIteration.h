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
#ifndef MERGEFORNEXTITERATION_H
#define MERGEFORNEXTITERATION_H
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT MergeForNextIteration : public IFiltration
{

    DECLARE_CLASS(Tigon::Operators::MergeForNextIteration)

public:
    MergeForNextIteration();
    MergeForNextIteration(Tigon::Representation::IPSet* ipset);
    virtual ~MergeForNextIteration();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

private:
    void initialise();

    TString m_name;
    TString m_description;

};

} // namespace Operators
} // namespace Tigon

#endif // MERGEFORNEXTITERATION_H
