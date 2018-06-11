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
#include <tigon/Operators/Evaluators/Evaluator.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/tigonengineregistry.h>

#include <future>
#include <thread>
using namespace std;

namespace Tigon {
namespace Operators {
using namespace Tigon::Representation;

Evaluator::Evaluator()
{

}

Evaluator::Evaluator(Tigon::Representation::IPSet *ipset)
    : IEvaluator(ipset)
{

}

Evaluator::~Evaluator()
{

}

void Evaluator::evaluateNode()
{
    while(hasNextOutputSet()) {
        ISet* oSet = nextOutputSet();
        int evalCost = 0;
        if(TP_isParallel()) {
            /// \todo Handle out of bounds for parallel evaluation
            TVector<future<int>> results;

            for(int idx=0; idx<oSet->size(); idx++) {
                IMappingSPtr imap = oSet->at(idx);
                results.push_back(std::async(launch::async,
                                             &IMapping::parallelEvaluate,
                                             imap.get()));
            }

            for (int i=0; i< oSet->size(); i++) {
                // Blocks until all evaluations have returned a value
                int res = results[i].get();
                if(res < 0) {
                    // Error!!
                } else if(res > 0){
                    if(TP_isCountEvaluations()) {
                        if(TP_isCountPerFunction()) {
                            evalCost += res;
                        } else {
                            evalCost++;
                        }
                    }

                    if(TP_isSingleObjective()) {
                        oSet->at(i)->defineCost(*(oSet->at(i)->objectiveVar(0)));
                    }
                }
            }

            for (int i=0; i< oSet->size(); i++) {
                updateIdealNadirVec(oSet->at(i));
            }

        } else {
            for(int idx=0; idx<oSet->size(); idx++) {
                IMappingSPtr imap = oSet->at(idx);
                int evaluationCost = imap->evaluate();

                /// if evaluationCost > 0 it means that at least one
                /// function is successfully evaluated
                if(evaluationCost > 0) {

                    // increase cost
                    if(TP_isCountEvaluations()) {
                        if(TP_isCountPerFunction()) {
                            evalCost += evaluationCost;
                        } else {
                            evalCost++;
                        }
                    }

                    // update cost for single objective
                    if(TP_isSingleObjective()) {
                        imap->defineCost(*(imap->objectiveVar(0)));
                    }

                    // update ideal, anti-ideal, and nadir vectors
                    if(imap->isSuccessfulEval()){
                        updateIdealNadirVec(imap);
                    } else {

                        /// \note This situation happens only when:
                        /// 1) there is more than one function in the problem;
                        /// 2) the outputs of at least one function violates
                        ///    the defined bounds; and
                        /// 3) the previous successful evaluation of the other
                        ///    function(s) makes evaluationCost>0

                        // save the budget before throwing exception
                        if(TP_isCountEvaluations()) {
                            if(TP_isCountPerFunction()) {
                                decrementBudget(evalCost);
                            }
                        }
                    }
                }
            }
        }

        decrementBudget(evalCost);
    }
}

TString Evaluator::name()
{
    return TString("Evaluator");
}

TString Evaluator::description()
{
    return TString("Iterates over all sets tagged for evaluation "
                   "and evaluates every IMapping in each set.");
}

} // namespace Operators
} // namespace Tigon
REGISTER_IPSET_FACTORY(Operators, Evaluator)
