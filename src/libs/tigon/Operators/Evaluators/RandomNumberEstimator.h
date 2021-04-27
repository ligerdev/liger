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
#ifndef RANDOMNUMBERESTIMATOR_H
#define RANDOMNUMBERESTIMATOR_H
#include <tigon/Operators/Evaluators/IEvaluator.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RandomNumberEstimator : public IEvaluator
{
    Q_OBJECT
    Q_PROPERTY(qreal NeighbourhoodRadius
               READ  TP_neighbourhoodRadius
               WRITE TP_defineNeighbourhoodRadius)
    Q_PROPERTY(int MaxSolutions
               READ TP_maxSolutions
               WRITE TP_defineMaxSolutions)

public:
    RandomNumberEstimator();
    RandomNumberEstimator(IPSet* ipset);
    ~RandomNumberEstimator();
    void evaluateNode();

    // Information about the node.
    QString name();
    QString description();
    virtual QStringList propertyNames();
    virtual QStringList propertyDescriptions();

    // Properties
    void  TP_defineNeighbourhoodRadius(qreal r);
    qreal TP_neighbourhoodRadius()        const;

    void  TP_defineMaxSolutions(int n);
    int   TP_maxSolutions()      const;

private:
    qreal  m_neighbourhoodRadius;
    int    m_maxSolutions;
};

} // namespace Operators
} // namespace Tigon

#endif // RANDOMNUMBERESTIMATOR_H
