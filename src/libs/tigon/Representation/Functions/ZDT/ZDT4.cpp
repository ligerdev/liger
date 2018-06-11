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
** This is a modified version of ZDT4.cpp from jMetalCpp version 1.0.1
** Filename: ZDT4.cpp
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#include <metal/problems/ZDT4.h>

ZDT4::ZDT4(QString solutionType, int numberOfVariables) {
    m_numberOfVariables   = numberOfVariables;
    m_numberOfObjectives  = 2;
    m_numberOfConstraints = 0;
    m_problemName         = "ZDT4";

    m_lowerLimit = new double[m_numberOfVariables];
    if (m_lowerLimit == 0) {
        throw MetalException("Error in ZDT4::ZDT4(QString, int) : Out of memory.");
    }

    m_upperLimit = new double[m_numberOfVariables];
    if (m_upperLimit == 0) {
        throw MetalException("Error in ZDT4::ZDT4(QString, int) : Out of memory.");
    }

    m_lowerLimit[0] = 0.0;
    m_upperLimit[0] = 1.0;
    for (int i = 1; i < m_numberOfVariables; i++) {
        m_lowerLimit[i] = -5.0;
        m_upperLimit[i] = 5.0;
    }

    if (solutionType.compare("BinaryReal") == 0)
        m_solutionType = new BinaryRealSolutionType(this);
    else if (solutionType.compare("Real") == 0) {
        m_solutionType = new RealSolutionType(this);
    }
    else if (solutionType.compare("ArrayReal") == 0)
        m_solutionType = new ArrayRealSolutionType(this);
    else {
        throw MetalException("Error in ZDT4::ZDT4(QString, int) : Unknown SolutionType.");
    }
    m_fx = new double[m_numberOfObjectives];
}


ZDT4::~ZDT4() {
    delete [] m_lowerLimit;
    delete [] m_upperLimit;
    delete m_solutionType;
    delete [] m_fx;
}

/**
 * Evaluates a solution
 * @param solution The solution to evaluate
 */
void ZDT4::evaluate(Solution *solution) {
    XReal * x = new XReal(solution);
    double * fx = new double[m_numberOfObjectives];

    m_fx[0] = x->getValue(0);
    double g = evalG(x);
    double h = evalH(m_fx[0], g);
    m_fx[1] = h * g;

    solution->setObjective(0,m_fx[0]);
    solution->setObjective(1,m_fx[1]);

    delete [] fx;
    delete x;
}

double ZDT4::evalG(XReal * x) {
    double g = 0.0 ;
    for (int i = 1; i < x->getNumberOfDecisionVariables(); i++)
        g += pow(x->getValue(i), 2.0) -
                10.0 * cos(4.0 * M_PI * x->getValue(i));

    double c = 1.0 + 10.0 * (m_numberOfVariables - 1);
    return g + c;
}

double ZDT4::evalH(double f, double g) {
    return 1.0 - sqrt(f/g);
}
