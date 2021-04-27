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
#ifndef ALPINE2_H
#define ALPINE2_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT Alpine2 : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::Alpine2)
public:
    Alpine2();
    Alpine2(const Alpine2& func);
    virtual ~Alpine2();

    // Information about the function(s)
    TString name();
    TString description();

    void evaluate(const TVector<IElementSPtr> &inputs,
                          const TVector<IElementSPtr> &outputs);

private:
    void defineInputPrpts();
    void defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // ALPINE2_H
