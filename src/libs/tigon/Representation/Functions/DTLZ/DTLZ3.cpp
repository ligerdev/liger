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
** This is a modified version of DTLZ3.cpp from jMetalCpp version 1.0.1
** Filename: DTLZ3.cpp
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#include <metal/problems/DTLZ3.h>

DTLZ3::DTLZ3(QString solutionType, int numberOfVariables, int numberOfObjectives) {
    m_numberOfVariables   = numberOfVariables;
    m_numberOfObjectives  = numberOfObjectives;
    m_numberOfConstraints = 0;
    m_problemName         = "DTLZ3";

    m_lowerLimit = new double[m_numberOfVariables];
    if (m_lowerLimit == 0) {
        throw MetalException("Error in DTLZ3::DTLZ3(QString, int, int) : Out of memory.");
    }

    m_upperLimit = new double[m_numberOfVariables];
    if (m_upperLimit == 0) {
        throw MetalException("Error in DTLZ3::DTLZ3(QString, int, int) : Out of memory.");
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
        throw MetalException("Error in DTLZ2::DTLZ2(QString, int, int) : Unknown SolutionType.");
    }
    m_fx = new double[m_numberOfObjectives];
    m_x = new double[m_numberOfVariables];
}

DTLZ3::~DTLZ3() {
    delete [] m_lowerLimit;
    delete [] m_upperLimit;
    delete m_solutionType;
}

/**
 * Evaluates a solution
 * @param solution The solution to evaluate
 */
void DTLZ3::evaluate(Solution *solution) {
    XReal * vars = new XReal(solution);

    int k = m_numberOfVariables - m_numberOfObjectives + 1;

    for (int i = 0; i < m_numberOfVariables; i++)
        m_x[i] = vars->getValue(i);

    double g = 0.0;
    for (int i = m_numberOfVariables - k; i < m_numberOfVariables; i++)
        g += (m_x[i] - 0.5)*(m_x[i] - 0.5) - cos(20.0 * M_PI * (m_x[i] - 0.5));

    g = 100.0 * (k + g);
    for (int i = 0; i < m_numberOfObjectives; i++)
        m_fx[i] = 1.0 + g;

    for (int i = 0; i < m_numberOfObjectives; i++){
        for (int j = 0; j < m_numberOfObjectives - (i + 1); j++)
            m_fx[i] *= cos(m_x[j]*0.5*M_PI);
        if (i != 0){
            int aux = m_numberOfObjectives - (i + 1);
            m_fx[i] *= sin(m_x[aux]*0.5*M_PI);
        }
    }

    delete vars;
}
