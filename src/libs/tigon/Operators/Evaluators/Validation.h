/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef VALIDATION_H
#define VALIDATION_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Evaluators/IEvaluator.h>

namespace Tigon {
namespace Operators {
using namespace Tigon::Representation;

class LIGER_TIGON_EXPORT Validation : public IEvaluator
{
    HANDLE_READ_PROPERTIES_BEGIN(IEvaluator)
    READ(NEvaluations, TP_nEvaluations)
    READ(OperateOnFinal, TP_operateOnFinal)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IEvaluator)
    WRITE(NEvaluations, int, TP_defineNEvaluations)
    WRITE(OperateOnFinal, bool, TP_defineOperateOnFinal)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::Validation)

public:
    Validation();
    Validation(Tigon::Representation::IPSet *ipset);
    ~Validation();

    // Properties
    void TP_defineNEvaluations(int    n);
    int  TP_nEvaluations()         const;
    void TP_defineOperateOnFinal(bool o);
    bool TP_operateOnFinal()       const;

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

private:
    void  initialise();
    int   m_nEvaluations;
    bool  m_operateOnFinal;
};

} // namespace Operators
} // namespace Tigon

#endif /* VALIDATION_H */
