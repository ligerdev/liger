/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef IMATLABENGINE_H
#define IMATLABENGINE_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {

class LIGER_TIGON_EXPORT IMatlabEngine
{
    friend class MatlabPool;

public:
    IMatlabEngine();
    virtual ~IMatlabEngine();

    TString errorMessage();
    void setErrorMessage(TString msg);

    TStringList commandHistory();
    void addCommand(TString cmd);
    void clearCommandHistory();


    // Purely virtual methods
    virtual bool evaluateString(const TString& command, bool errorCatch = true) = 0;
    virtual void resetEngine() = 0;

    virtual void setInteractive(bool visible) = 0;
    virtual bool Interactive() = 0;

    virtual void placeVariable(const TString &name, int value) = 0;
    virtual void placeVariable(const TString &name, double value) = 0;
    virtual void placeVariable(const TString &name, bool value) = 0;
    virtual void placeVariable(const TString &name, TComplex value) = 0;
    virtual void placeVariable(const TString &name, TString value) = 0;

    // Place a vector into the MATLAB workspace, can be a row or column vector.
    virtual void placeVectorColumn(const TString &name, const TVector<int>& vec) = 0;
    virtual void placeVectorColumn(const TString &name, const TVector<double>& vec) = 0;
    virtual void placeVectorColumn(const TString &name, const TVector<bool>& vec) = 0;
    virtual void placeVectorColumn(const TString &name, const TVector<TComplex>& vec) = 0;

    virtual void placeVectorRow(const TString &name, const TVector<int>& vec) = 0;
    virtual void placeVectorRow(const TString &name, const TVector<double>& vec) = 0;
    virtual void placeVectorRow(const TString &name, const TVector<bool>& vec) = 0;
    virtual void placeVectorRow(const TString &name, const TVector<TComplex>& vec) = 0;

    // Place a Matrix onto the MATLAB workspace
    virtual void placeMatrix(const TString &name, const TVector<TVector<int>>& mat) = 0;
    virtual void placeMatrix(const TString &name, const TVector<TVector<double>>& mat) = 0;
    virtual void placeMatrix(const TString &name, const TVector<TVector<bool>>& mat) = 0;
    virtual void placeMatrix(const TString &name, const TVector< TVector<TComplex>>& mat) = 0;

    // Get Variable overides
    virtual bool getWorkspaceVariable(const TString &name, int& value) = 0;
    virtual bool getWorkspaceVariable(const TString &name, double& value) = 0;
    virtual bool getWorkspaceVariable(const TString &name, bool& value) = 0;
    virtual bool getWorkspaceVariable(const TString &name, TComplex& value) = 0;
    virtual bool getWorkspaceVariable(const TString &name, TString& value) = 0;

    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<int>& vec) = 0;
    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<double>& vec) = 0;
    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<bool>& vec) = 0;
    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<TComplex>& vec) = 0;

    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<int>>& mat) = 0;
    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<double>>& mat) = 0;
    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<bool>>& mat) = 0;
    virtual TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<TComplex> >& mat) = 0;

private:
    virtual void openEngine() = 0;
    virtual void closeEngine() = 0;

    TString m_errorBuff;
    TStringList m_commandHist;
};

} // Tigon

#endif // IMATLABENGINE_H
