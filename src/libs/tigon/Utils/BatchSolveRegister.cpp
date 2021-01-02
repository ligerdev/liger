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
#include <tigon/Utils/BatchSolveRegister.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Log/LogManager.h>

namespace Tigon {
using namespace Representation;

BatchSolveRegister::BatchSolveRegister()
{

}

void BatchSolveRegister::registerForBatchSolve(IFunctionSPtr func,
                                               TVector<Representation::IElementSPtr> inputs,
                                               TVector<Representation::IElementSPtr> outputs)
{
    if(!mapHasKey(m_funcDataMap, func)) {
        SolveData data;
        data.inputs.push_back(inputs);
        data.outputs.push_back(outputs);
        m_funcDataMap[func] = data;
    } else {
        m_funcDataMap[func].inputs.push_back(inputs);
        m_funcDataMap[func].outputs.push_back(outputs);
    }
}

void BatchSolveRegister::setOptimizationType(Representation::IFunctionSPtr func,
                                             const TVector<OptimizationType> &optTypes)
{
    m_funcOptType[func] = optTypes;
}

int BatchSolveRegister::evaluate(Log::LogManagerSPtr log)
{
    int evalCount = 0;
    TVector<IFunctionSPtr> funcs = getMapKeys(m_funcDataMap);
    for(int i=0; i<funcs.size(); i++) {
        IFunctionSPtr func = funcs[i];
        func->batchEvaluate(m_funcDataMap[func].inputs,
                            m_funcDataMap[func].outputs);

        // Negate the maximization outputs
        for(int k=0; k<m_funcDataMap[func].outputs.size(); k++) {
            for(int j = 0; j < m_funcDataMap[func].outputs[k].size(); j++) {
                if(m_funcOptType[func][j] == Maximization) {
                    m_funcDataMap[func].outputs[k][j]->negate();
                }
            }
        }

        // Log
        if(log && log->isLogEvaluation()) {
            for (int j=0; j<m_funcDataMap[func].inputs.size(); j++) {
                log->logEvaluation(func,
                                   m_funcDataMap[func].inputs[j],
                                   m_funcDataMap[func].outputs[j]);
            }
        }

        ++evalCount;
    }
    return evalCount;
}
}
