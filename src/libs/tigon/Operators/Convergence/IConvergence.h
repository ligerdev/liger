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
#ifndef ICONVERGENCE_H
#define ICONVERGENCE_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IConvergence : public IOperator
{
    HANDLE_READ_PROPERTIES_BEGIN(IOperator)
    READ(OperateOnFinal, TP_operateOnFinal)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IOperator)
    WRITE(OperateOnFinal, bool, TP_defineOperateOnFinal)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IConvergence)

public:
    IConvergence();
    IConvergence(Tigon::Representation::IPSet* ipset);
    virtual ~IConvergence();

    double convergenceValue() const;

    void TP_defineOperateOnFinal(bool o);
    bool TP_operateOnFinal()       const;

protected:
    void  initialise();
    double m_convIndicatorValue;

private:
    bool m_operateOnFinal;

};

} // namespace Operators
} // namespace Tigon

#endif // ICONVERGENCE_H
