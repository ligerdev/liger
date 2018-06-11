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
#include <tigon/Operators/Evaluators/WFG/WFG1.h>

#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <tigon/Operators/Evaluators/WFG/wfg_toolkit/ExampleProblems.h>
#include <tigon/Operators/Evaluators/WFG/wfg_toolkit/TransFunctions.h>
#include <tigon/Operators/Evaluators/WFG/wfg_toolkit/Misc.h>

using namespace WFGT::Toolkit;
using namespace WFGT::Toolkit::Examples;
using std::vector;

WFG1::WFG1(QString solutionType, int numberOfVariables, int numberOfObjectives) {
    m_numberOfVariables   = numberOfVariables;
    m_numberOfObjectives  = numberOfObjectives;
    m_numberOfConstraints = 0;
    m_problemName         = "WFG1";

    if(m_numberOfVariables<4 || m_numberOfVariables<m_numberOfObjectives) {
        m_numberOfVariables = (m_numberOfObjectives>4) ? m_numberOfObjectives : 4;
    }

    m_lowerLimit = new double[m_numberOfVariables];
    if (m_lowerLimit == 0) {
        throw MetalException("Error in WFG1::WFG1(QString, int, int, int) : Out of memory.");
    }

    m_upperLimit = new double[m_numberOfVariables];
    if (m_upperLimit == 0) {
        throw MetalException("Error in WFG1::WFG1(QString, int, int, int) : Out of memory.");
    }

    for (int i = 0; i < m_numberOfVariables; i++) {
        m_lowerLimit[i] = 0.0;
        m_upperLimit[i] = 2.0*(i+1);
    }

    if (solutionType.compare("BinaryReal") == 0)
        m_solutionType = new BinaryRealSolutionType(this);
    else if (solutionType.compare("Real") == 0) {
        m_solutionType = new RealSolutionType(this);
    }
    else if (solutionType.compare("ArrayReal") == 0)
        m_solutionType = new ArrayRealSolutionType(this);
    else {
        throw MetalException("Error in WFG1::WFG1(QString, int, int, int) : Unknown SolutionType.");
    }

    m_fx = new double[m_numberOfObjectives];
    m_x  = new double[m_numberOfVariables];
}

WFG1::~WFG1() {
    delete [] m_lowerLimit;
    delete [] m_upperLimit;
    delete m_solutionType;
    delete [] m_fx;
    delete [] m_x;
}

/**
 * Evaluates a decision vector
 * @param solution The decision vector to evaluate
 */
void WFG1::evaluate(Solution *solution) {
    XReal * vars = new XReal(solution);

    int nobj  = m_numberOfObjectives;
    int nbVar = m_numberOfVariables;
    int k     = qRound(nbVar/1.2);
    // Make sure k % (nobj-1) == 0
    int dff = k % (nobj-1);
    k = k - dff;

    vector<double> ind;
    for(int i=0; i<nbVar; i++) {
        ind.push_back(vars->getValue(i));
    }

    // Call the WFG Toolkit.
    const vector<double>& z = ind;
    const vector<double>& f = Problems::WFG1(z, k, nobj);

    for(int i=0; i<nobj; i++)
        m_fx[i] = f[i];

    for (int i = 0; i < m_numberOfObjectives; i++)
        solution->setObjective(i, m_fx[i]);

    delete vars;
} // evaluate
