/****************************************************************************
**
** Copyright (C) 2012-2014 The University of Sheffield (www.sheffield.ac.uk)
** Copyright (C) 2012-2014 Ioannis Giagkiozis
**
**
** This file is part of Liger.
**
** Liger is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version
**
** Liger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Liger.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#ifndef WFG4_H
#define WFG4_H

#include <metal/metal_global.h>
#include <metal/core/IProblem.h>
#if defined(Q_OS_WIN)
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <metal/encodings/solutionType/BinaryRealSolutionType.h>
#include <metal/encodings/solutionType/RealSolutionType.h>
#include <metal/encodings/solutionType/ArrayRealSolutionType.h>
#include <metal/util/wrapper/XReal.h>
#include <metal/core/Solution.h>

class LIGER_METAL_EXPORT WFG4 : public IProblem {
public:
    WFG4(QString solutionType, int numberOfVariables = 7, int numberOfObjectives = 3);
    void evaluate(Solution *solution);

    virtual ~WFG4();
private:
    double* m_fx;
    double* m_x;
};


#endif // WFG4_H
