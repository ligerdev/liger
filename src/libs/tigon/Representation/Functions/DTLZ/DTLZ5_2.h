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
#ifndef DTLZ5_2_H
#define DTLZ5_2_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT DTLZ5_2 : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::DTLZ5_2)
public:
    DTLZ5_2();
    DTLZ5_2(const DTLZ5_2& func);
    ~DTLZ5_2();

    void evaluate(const TVector<IElementSPtr> &inputs,
                  const TVector<IElementSPtr> &outputs);

private:
    void defineInputPrpts();
    void defineOutputPrpts();
};

} // namespace Representation
} // namespace Tigon

#endif // DTLZ5_2_H
