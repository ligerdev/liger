/****************************************************************************
**
** Copyright (C) 2012-2014 The University of Sheffield (www.sheffield.ac.uk)
** Copyright (C) 2012-2014 Ioannis Giagkiozis
**
** Contact: http://ligeropt.sourceforge.net
**
** This file is part of Liger.
**
** Liger is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version
**
** Liger is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Liger.  If not, see <http://www.gnu.org/licenses/>.
**
** This is a modified version of LZ09_F7.h from jMetalCpp version 1.0.1
** Filename: LZ09_F7.h
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#ifndef __LZ09_F7_H__
#define __LZ09_F7_H__

#include <metal/metal_global.h>
#if defined(Q_OS_WIN)
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <QString>
#include <metal/core/IProblem.h>
#include <metal/encodings/solutionType/BinaryRealSolutionType.h>
#include <metal/encodings/solutionType/RealSolutionType.h>
#include <metal/encodings/solutionType/ArrayRealSolutionType.h>
#include <metal/util/wrapper/XReal.h>
#include <metal/core/Solution.h>
#include <metal/problems/LZ09.h>

class LIGER_METAL_EXPORT LZ09_F7 : public IProblem {
public:
    LZ09_F7(QString solutionType, int ptype=21, int dtype=3, int ltype=21);
    void evaluate(Solution *solution);

    virtual ~LZ09_F7();
private:
    LZ09*   LZ09_;
    double* m_fx;
    double* m_x;
};

#endif /* __LZ09_F7_H__ */
