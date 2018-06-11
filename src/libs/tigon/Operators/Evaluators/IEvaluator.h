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
#ifndef IEVALUATOR_H
#define IEVALUATOR_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/IOperator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IEvaluator : public IOperator
{
    HANDLE_READ_PROPERTIES_BEGIN(IOperator)
    READ(ParallelEvaluation, TP_isParallel)
    READ(CountEvaluations, TP_isCountEvaluations)
    READ(CountPerFunction, TP_isCountPerFunction)
    READ(SingleObjective, TP_isSingleObjective)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IOperator)
    WRITE(ParallelEvaluation, bool, TP_defineParallel)
    WRITE(ParallelEvaluation, bool, TP_defineCountEvaluations)
    WRITE(ParallelEvaluation, bool, TP_defineCountPerFunction)
    WRITE(ParallelEvaluation, bool, TP_defineSingleObjective)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::IEvaluator)

public:
    IEvaluator();
    IEvaluator(Representation::IPSet* ipset);
    virtual ~IEvaluator();

    /// Properties
    bool TP_isParallel()             const;
    void TP_defineParallel(bool         p);
    bool TP_isCountEvaluations()     const;
    void TP_defineCountEvaluations(bool c);
    bool TP_isCountPerFunction()     const;
    void TP_defineCountPerFunction(bool c);
    bool TP_isSingleObjective()      const;
    void TP_defineSingleObjective(bool  s);

private:
    void initialise();

    bool m_parallelEvaluations;
    bool m_countEvaluations;
    bool m_countPerFunction;
    bool m_singleObjective;
};

} // namespace Operators
} // namespace Tigon

#endif // IEVALUATOR_H
