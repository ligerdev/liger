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
#pragma once

#include <tigon/tigon_global.h>
#include <tigon/Operators/Evaluators/IEvaluator.h>

namespace Tigon {
namespace Operators {

/*!
 * \brief The BatchEvaluator class aims for Matlab only function evaluations.
 * For internal functions, functions from shared lib, a mix between Matlab and
 * other types of functions, use ```Evaluator``` (generic function evaluator)
 * instead.
 */
const TString BATCH_EVAL_COUNT_ERROR("Supports signle Matlab function "
                                    "evaluation only. For other types, "
                                    "use Evaluator instead.");

class LIGER_TIGON_EXPORT BatchEvaluator : public IEvaluator
{
    DECLARE_CLASS(Tigon::Operators::BatchEvaluator)

public:
    BatchEvaluator();
    BatchEvaluator(Tigon::Representation::IPSet *ipset);
    ~BatchEvaluator();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

private:
    bool m_hasChecked;
};

}
}
