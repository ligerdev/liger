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
#ifndef BATCHSOLVEREGISTER_H
#define BATCHSOLVEREGISTER_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class IFunction;
}

namespace Log {
class LogManager;
}

class LIGER_TIGON_EXPORT BatchSolveRegister
{
    struct SolveData {
        TVector<TVector<Representation::IElementSPtr> > inputs;
        TVector<TVector<Representation::IElementSPtr> > outputs;
    };

public:
    BatchSolveRegister();
    ~BatchSolveRegister(){}
    void registerForBatchSolve(Representation::IFunctionSPtr func,
                               TVector<Representation::IElementSPtr> inputs,
                               TVector<Representation::IElementSPtr> outputs);
    void setOptimizationType(Representation::IFunctionSPtr func,
                             const TVector<OptimizationType> &optTypes);
    int evaluate(Log::LogManagerSPtr log);

private:
    TMap<Representation::IFunctionSPtr, SolveData> m_funcDataMap;
    TMap<Representation::IFunctionSPtr, TVector<OptimizationType> > m_funcOptType;
};
}
#endif // BATCHSOLVEREGISTER_H
