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
#include <tigon/Operators/Evaluators/BatchEvaluator.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Utils/BatchSolveRegister.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/tigonengineregistry.h>

namespace Tigon {
namespace Operators {
using namespace Tigon::Representation;

BatchEvaluator::BatchEvaluator()
    : m_hasChecked(false)
{

}

BatchEvaluator::BatchEvaluator(IPSet *ipset)
    : IEvaluator(ipset)
    , m_hasChecked(false)
{

}

BatchEvaluator::~BatchEvaluator()
{

}

void BatchEvaluator::evaluateNode()
{
    /// [] Check - for now only accept a single matlab function.
    /// This function can have more than one output.
    if(!m_hasChecked) {
        // Check if matlab function only
        TVector<IFunctionSPtr> funcs = this->problem()->functionVec();
        if(funcs.size() > 1) {
            throw TException(this->className(),
                             BATCH_EVAL_COUNT_ERROR);
        }
        m_hasChecked = true;
    }

    while(hasNextOutputSet()) {
        BatchSolveRegister batchReg;

        ISet* oSet = nextOutputSet();
        int evalCost = 0;
        for(int idx=0; idx<oSet->size(); idx++) {
            IMappingSPtr imap = oSet->at(idx);
            imap->registerForBatchEvaluation(&batchReg);
        }

        if(TP_isCountEvaluations()) {
            evalCost = batchReg.evaluate(log());
        } else {
            batchReg.evaluate(log());
        }

        ///[] Set the counters to proper value
        for(int idx=0; idx<oSet->size(); idx++) {
            IMappingSPtr imap = oSet->at(idx);

            // Update evaluation status
            imap->defineEvaluated(true);
            imap->defineScalarised(false);

            if(TP_isSingleObjective()) {
                imap->defineCost(*(imap->objectiveVar(0)));
            }

            updateIdealNadirVec(imap);
        }

        decrementBudget(evalCost);
    }
}

TString BatchEvaluator::name()
{
    return TString("Batch Evaluator");
}

TString BatchEvaluator::description()
{
    return TString("Evaluator designed specifically for Matlab Function. "
                   "Use this operator for Matlab function evaluation only.");
}

} // namespace Operators
} // namespace Tigon
REGISTER_IPSET_FACTORY(Operators, BatchEvaluator)
