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
#ifndef OPTIMIZATIONLINEARFLOW_H
#define OPTIMIZATIONLINEARFLOW_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {
class IPSet;
}
}

namespace Tigon {

class LIGER_TIGON_EXPORT OptimizationLinearFlow
{
public:
    OptimizationLinearFlow();
    ~OptimizationLinearFlow();

    void setCurrentIndex(int index);
    Representation::IPSet* previousNode();
    Representation::IPSet* currentNode()  const;
    Representation::IPSet* nextNode();
    Representation::IPSet* finalNode()    const;
    Representation::IPSet* node(int index)  const;
    int  currentIndex()                     const;
    void appendNode(Representation::IPSet* node);
    int  size() const;

    bool isTerminate() const;

    int  currentIteration() const;
    int  remainingIterations() const;
    int  maxIteration() const;
    int  budget() const;
    int  usedBudget() const;
    int  remainingBudget() const;

    void evaluate();
    void clearFlow();
    void incrementIteration();

private:
    TVector<Representation::IPSet*> m_nodes;
    int m_currentIndex;
};

} // namespace Tigon

#endif // OPTIMIZATIONLINEARFLOW_H
