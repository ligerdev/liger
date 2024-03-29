/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
**
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#include <matlabplugin/Utils/IMatlabEngine.h>

namespace Tigon {

IMatlabEngine::IMatlabEngine()
{

}

IMatlabEngine::~IMatlabEngine()
{

}

void IMatlabEngine::setErrorMessage(TString msg)
{
    m_errorBuff = msg;
}

TString IMatlabEngine::errorMessage()
{
    return m_errorBuff;
}

TStringList IMatlabEngine::commandHistory()
{
    return m_commandHist;
}

void IMatlabEngine::addCommand(TString cmd)
{
    m_commandHist.push_back(cmd);
}

void IMatlabEngine::clearCommandHistory()
{
    m_commandHist.clear();
}

} //namespace Tigon
