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
** This is a modified version of LZ09_F4.cpp from jMetalCpp version 1.0.1
** Filename: LZ09_F4.cpp
** Path:     jMetalCpp/problems/
** Date:     12/03/2013
** Original Version Author/s:  Antonio J. Nebro, Juan J. Durillo
****************************************************************************/
#include <metal/problems/LZ09_F4.h>

/**
 * Constructor
 */
LZ09_F4::LZ09_F4(QString solutionType, int ptype, int dtype, int ltype) {
    m_numberOfVariables  = 30;
    m_numberOfObjectives = 2;
    m_numberOfConstraints= 0;
    m_problemName        = "LZ09_F4";

    LZ09_  = new LZ09(m_numberOfVariables,
                      m_numberOfObjectives,
                      ptype,
                      dtype,
                      ltype);

    m_lowerLimit = new double[m_numberOfVariables];
    if (m_lowerLimit == 0) {
        throw MetalException("Error in LZ09_F4::LZ09_F4(QString, int, int, int) : Out of memory.");
    }

    m_upperLimit = new double[m_numberOfVariables];
    if (m_upperLimit == 0) {
        throw MetalException("Error in LZ09_F4::LZ09_F4(QString, int, int, int) : Out of memory.");
    }

    m_lowerLimit[0] = 0.0;
    m_upperLimit[0] = 1.0;
    for (int i = 1; i < m_numberOfVariables; i++) {
        m_lowerLimit[i] = -1.0;
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
        throw MetalException("Error in LZ09_F4::LZ09_F4(QString, int, int, int) : Unknown SolutionType.");
    }

    m_fx = new double[m_numberOfObjectives];
    m_x = new double[m_numberOfVariables];
}

/**
 * Destructor
 */
LZ09_F4::~LZ09_F4 () {
    delete [] m_lowerLimit;
    delete [] m_upperLimit;
    delete m_solutionType;
    delete LZ09_;
}

void LZ09_F4::evaluate(Solution * solution) {
    XReal * vars = new XReal(solution);

    vector<double> * x = new vector<double>(m_numberOfVariables);
    vector<double> * y = new vector<double>(m_numberOfObjectives);

    for (int i = 0; i < m_numberOfVariables; i++) {
        x->at(i) = vars->getValue(i);
        y->at(i) = 0.0;
    } // for

    LZ09_->objective(x, y);

    for (int i = 0; i < m_numberOfObjectives; i++)
        solution->setObjective(i, y->at(i));
}
