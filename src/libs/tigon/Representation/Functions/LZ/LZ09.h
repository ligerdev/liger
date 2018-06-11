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
** This is a modified version of LZ09.h from jMetalCpp version 1.0.1
** Filename: LZ09.h
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#ifndef __LZ09_H__
#define __LZ09_H__

#include <metal/metal_global.h>
//#if defined(Q_OS_WIN)
//#define _USE_MATH_DEFINES
//#endif
//#include <math.h>
#include <QtMath>
#include <cstdlib>
#include <vector>

using namespace std;

class LIGER_METAL_EXPORT LZ09 {
private:
    int nvar_;
    int nobj_;
    int lm_type;
    int dm_type;
    int pm_type;

public:
    LZ09 (int nvar, int nobj, int ptype, int dtype, int ltype) ;
    void alphaFunction(double *alpha, vector<double> *x, int dim, int type) ;
    double betaFunction(vector<double> * x, int type) ;
    double psfunc2(double x, double t1, int dim, int type, int css) ;
    double psfunc3(double x, double t1, double t2, int dim, int type) ;
    void objective(vector<double> * x_var, vector <double> * y_obj) ;

};

#endif /* __LZ09_H__ */
