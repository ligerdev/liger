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
#ifndef ZDT2_H
#define ZDT2_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT ZDT2 : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::ZDT2)
public:
    ZDT2();
    ZDT2(const ZDT2& func);
    ~ZDT2();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

private:
    void defineInputPrpts();
    void defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // ZDT2_H
