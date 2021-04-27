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
#include <visualisation/iterationinteruptor.h>

using namespace Visualisation;

IterationInteruptor::IterationInteruptor()
{
    m_iterationToInterupt = 0;
    m_iterationToResetCounter = 0;
    m_lastIterationReceived = 0;
}

IterationInteruptor::IterationInteruptor(int iterationToInterupt)
{
    m_iterationToInterupt = iterationToInterupt;
    m_iterationToResetCounter = 0;
    m_lastIterationReceived = 0;
}

IterationInteruptor::~IterationInteruptor()
{

}

void IterationInteruptor::setInteruptionIteration(int iterationToInterupt)
{
    m_iterationToInterupt = iterationToInterupt;
    m_iterationToResetCounter = 0;
}

void IterationInteruptor::updateIteration(int currentIteration)
{
    if(m_lastIterationReceived < currentIteration){
        m_iterationToResetCounter = m_iterationToResetCounter +
                currentIteration - m_lastIterationReceived;
    }
    m_lastIterationReceived = currentIteration;

    if(isIterationToInteruptReached()){
        m_iterationToResetCounter = 0;
        emit iterationReached();
    }
}

bool IterationInteruptor::isIterationToInteruptReached()
{
    if(m_iterationToInterupt!=0){
        return m_iterationToResetCounter >= (m_iterationToInterupt-1);
    } else {
        return false;
    }
}
