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
** This is a modified version of ZDT2.cpp from jMetalCpp version 1.0.1
** Filename: ZDT2.cpp
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#include <metal/problems/ZDT2.h>

ZDT2::ZDT2(QString solutionType, int numberOfVariables) {
    m_numberOfVariables   = numberOfVariables;
    m_numberOfObjectives  = 2;
    m_numberOfConstraints = 0;
    m_problemName         = "ZDT2";

    m_lowerLimit = new double[m_numberOfVariables];
    if (m_lowerLimit == 0) {
        throw MetalException("Error in ZDT2::ZDT2(QString, int) : Out of memory.");
    }

    m_upperLimit = new double[m_numberOfVariables];
    if (m_upperLimit == 0) {
        throw MetalException("Error in ZDT2::ZDT2(QString, int) : Out of memory.");
    }

    for (int i = 0; i < m_numberOfVariables; i++) {
        m_lowerLimit[i] = 0.0;
        m_upperLimit[i] = 1.0;
    }

    if (solutionType.compare("BinaryReal") == 0)
        m_solutionType = new BinaryRealSolutionType(this);
    else if (solutionType.compare("Real") == 0) {
        m_solutionType = new RealSolutionType(this);
    }
    else if (solutionType.compare("ArrayReal") == 0)
        m_solutionType = new ArrayRealSolutionType(this);
    else {
        throw MetalException("Error in ZDT2::ZDT2(QString, int) : Unknown SolutionType.");
    }
    m_fx = new double[m_numberOfObjectives];
}


ZDT2::~ZDT2() {
    delete [] m_lowerLimit;
    delete [] m_upperLimit;
    delete m_solutionType;
    delete [] m_fx;
}

/**
 * Evaluates a solution
 * @param solution The solution to evaluate
 */
void ZDT2::evaluate(Solution *solution) {
    XReal * x = new XReal(solution);

    m_fx[0] = x->getValue(0);
    double g = evalG(x);
    double h = evalH(m_fx[0], g);
    m_fx[1] = h * g ;

    solution->setObjective(0,m_fx[0]);
    solution->setObjective(1,m_fx[1]);

    delete x;
}

double ZDT2::evalG(XReal * x) {
    double g = 0.0;
    for (int i = 1; i < x->getNumberOfDecisionVariables(); i++)
        g += x->getValue(i);

    double c = 9.0/(m_numberOfVariables - 1);
    g = c * g;
    g = g + 1.0;
    return g;
}

double ZDT2::evalH(double f, double g) {
    double h = 0.0;
    h = 1.0 - pow(f/g, 2.0);
    return h;
}

