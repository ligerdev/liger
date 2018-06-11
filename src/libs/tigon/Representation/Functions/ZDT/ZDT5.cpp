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
** This is a modified version of ZDT5.cpp from jMetalCpp version 1.0.1
** Filename: ZDT5.cpp
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#include <metal/problems/ZDT5.h>

ZDT5::ZDT5(QString solutionType, int numberOfVariables) {
    m_numberOfVariables   = numberOfVariables;
    m_numberOfObjectives  = 2;
    m_numberOfConstraints = 0;
    m_problemName         = "ZDT5";

    m_length = new int[m_numberOfVariables];
    m_length[0] = 30;
    for (int var = 1; var < m_numberOfVariables; var++) {
        m_length[var] = 5;
    }

    if (solutionType.compare("Binary") == 0)
        m_solutionType = new BinarySolutionType(this);
    else {
        throw MetalException("Error in ZDT5::ZDT5(QString, int) : Unknown SolutionType.");
    }
    m_fx = new double[m_numberOfObjectives];
}

ZDT5::~ZDT5() {
    delete [] m_length;
    delete [] m_fx;
}

/**
 * Evaluates a solution
 * @param solution The solution to evaluate
 */
void ZDT5::evaluate(Solution *solution) {
    Binary * variable;
    int    counter;

    variable = (Binary *)(solution->getDecisionVariables()[0]);

    m_fx[0]  = 1 + variable->cardinality();

    double g = evalG(solution);
    double h = evalH(m_fx[0],g);
    m_fx[1]  = h * g;

    solution->setObjective(0,m_fx[0]);
    solution->setObjective(1,m_fx[1]);


}

double ZDT5::evalG(Solution * solution) {
    double res = 0.0;
    Binary * variable;

    for (int i = 1; i < m_numberOfVariables; i++) {
        variable = (Binary *)(solution->getDecisionVariables()[i]);
        res += evalV(variable->cardinality());
    }

    return res;
}

double ZDT5::evalH(double f, double g) {
    return 1.0/f;
}

double ZDT5::evalV(double value) {
    if (value < 5.0) {
        return 2.0 + value;
    } else {
        return 1.0;
    }
}
