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
#ifndef ITERATIONINTERUPTOR_H
#define ITERATIONINTERUPTOR_H

#include <visualisation/visualisation_global.h>
#include <visualisation/visualisationconstants.h>

#include <QObject>

namespace Visualisation {
class VISUALISATION_EXPORT IterationInteruptor : public QObject
{
    Q_OBJECT

public:
    IterationInteruptor();
    IterationInteruptor(int iterationToInterupt);
    ~IterationInteruptor();

    void setInteruptionIteration(int iterationToInterupt);
    void updateIteration(int currentIteration);

signals:
    void iterationReached();

private:
    bool isIterationToInteruptReached();

    int m_iterationToInterupt;
    int m_iterationToResetCounter;
    int m_lastIterationReceived;
};
}

#endif // ITERATIONINTERUPTOR_H
