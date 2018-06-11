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
** This is a modified version of DTLZ7.cpp from jMetalCpp version 1.0.1
** Filename: DTLZ7.cpp
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#include <metal/problems/DTLZ7.h>

DTLZ7::DTLZ7(QString solutionType, int numberOfVariables, int numberOfObjectives) {
    m_numberOfVariables   = numberOfVariables;
    m_numberOfObjectives  = numberOfObjectives;
    m_numberOfConstraints = 0;
    m_problemName         = "DTLZ7";

    m_lowerLimit = new double[m_numberOfVariables];
    if (m_lowerLimit == 0) {
        throw MetalException("Error in DTLZ7::DTLZ7(QString, int, int) : Out of memory.");
    }

    m_upperLimit = new double[m_numberOfVariables];
    if (m_upperLimit == 0) {
        throw MetalException("Error in DTLZ7::DTLZ7(QString, int, int) : Out of memory.");
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
        throw MetalException("Error in DTLZ7::DTLZ7(QString, int, int) : Unknown SolutionType.");
    }

    m_fx = new double[m_numberOfObjectives];
    m_x = new double[m_numberOfVariables];
}

DTLZ7::~DTLZ7() {
    delete [] m_lowerLimit;
    delete [] m_upperLimit;
    delete m_solutionType;
}

/**
 * Evaluates a solution
 * @param solution The solution to evaluate
 */
void DTLZ7::evaluate(Solution *solution) {
    XReal * vars = new XReal(solution);
    double g = 0.0;

    int k = m_numberOfVariables - m_numberOfObjectives + 1;
    double alpha = 100.0;

    for (int i = 0; i < m_numberOfVariables; i++)
        m_x[i] = vars->getValue(i);

    for (int i = m_numberOfVariables - k; i < m_numberOfVariables; i++)
        g += m_x[i] ;

    g = 1 + (9.0 * g)/k;

    for (int i = 0; i < m_numberOfObjectives - 1; i++)
        m_fx[i] = m_x[i];

    double h = 0.0;
    for (int i = 0; i < m_numberOfObjectives - 1; i++){
        h+=(m_fx[i]/(1.0+g))*(1 + sin(3.0*M_PI*m_fx[i]));
    }

    h = m_numberOfObjectives - h;

    m_fx[m_numberOfObjectives - 1] = (1+g)*h;

    for (int i = 0; i < m_numberOfObjectives; i++)
        solution->setObjective(i, m_fx[i]);

    delete vars;
}
