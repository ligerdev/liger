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
#ifndef EMO2019P1_H
#define EMO2019P1_H

#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {

class LIGER_TIGON_EXPORT EMO2019P1 : public IFunction
{
    DECLARE_CLASS(Tigon::Representation::EMO2019P1)
public:
    EMO2019P1();
    EMO2019P1(const EMO2019P1& func);
    ~EMO2019P1();

    void evaluate(const TVector<IElementSPtr> &inputs,
                          const TVector<IElementSPtr> &outputs);

    void defineNumDirVars(int k);
    int  numDirVars()      const;

private:
    void defineInputPrpts();
    void defineOutputPrpts();

    int m_nDirVar;
};

} // namespace Representation
} // namespace Tigon

#endif // EMO2019P1_H
