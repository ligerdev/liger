/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#ifndef QTIGONCONSTANTS_H
#define QTIGONCONSTANTS_H

namespace QTigon {
namespace Constants {

const char * const ACTION_ID = "QTigon.Action";
const char * const MENU_ID   = "QTigon.Menu";


// Scope
const char VARIABLE[]   = "Variable";
const char PARAMETER[]  = "Parameter";
const char OBJECTIVE[]  = "Objective";
const char CONSTRAINT[] = "Constraint";
const char UNUSED[]     = "Unused";

// Opt Type
const char MIN[]  = "Minimisation";
const char MAX[]  = "Maximisation";

// Problem types enumeration
enum ProblemType {WFG_PROBLEM,
                  DTLZ_PROBLEM,
                  MATLAB_PROBLEM,
                  CPP_PROBLEM,
                  INVALID_PROBLEM,
                  ABSTRACT_PROBLEM};

} // namespace QTigon
} // namespace Constants

#endif // QTIGONCONSTANTS_H

