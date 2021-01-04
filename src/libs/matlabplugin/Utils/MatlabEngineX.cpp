/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <matlabplugin/Utils/MatlabEngineX.h>

namespace Tigon {
/*!
 *\brief Create a MatlabEngine object.
 *
 * This constructor is private, only MatlabPool is allowed to create new MatlabEngine.
 * Lazy initialization is used so the MATLAB engine is not started until a command
 * is run or a variable placed.
 */
MatlabEngineX::MatlabEngineX()
    : m_engine(nullptr)
{

}

/*!
 * \brief Destructor
 *
 * Destroys the MatlabEngine object and closes the MATLAB engine, stopping the MATLAB process.
 */
MatlabEngineX::~MatlabEngineX()
{
    closeEngine();
}

/*!
 * \brief Starts the MATLAB engine
 *
 * Starts the MATLAB process and sets m_engine to point to the MATLAB engine.
 *
 * This operation is called once when an instance of MATLAB is first required,
 * each MatlabEngine holds a reference to a MATLAB engine which runs until the
 * MatlabEngine object is destroyed.
 */
void MatlabEngineX::openEngine()
{
    if(!m_engine) {
        m_engine = startMATLAB();
    }
}

/*!
 * \brief Stop the MATLAB engine
 *
 * Stops the MATLAB process, closing the engine.
 */
void MatlabEngineX::closeEngine()
{
    if(m_engine) {
        m_engine.release();
        clearCommandHistory();
    }
}

/*!
 * \brief Resets the engine to an initial state.
 *
 * This clears the MATLAB workspace and the list of commands so far issued to
 * the engine. Primarily used by MatlabPool to ensure the pooled engines are in
 * a known initial state.
 */
void MatlabEngineX::resetEngine()
{
    if(m_engine) {
        ///\note "clear all" was used to reset the engine. However, it sometimes
        /// causes crash of Simulink. Changed to clearvars for now.
        //evaluateString("clear all", 0);
        evaluateString("clearvars", false);

        clearCommandHistory();
    }
}

/*!
 * \brief Evaluate a string of commands in MATLAB
 *
 * Evaluate a string of commands in the MATLAB engine.
 * \param command The string to be evaluated by MATLAB
 * \param errorCatch Indicates if errors are to be logged (true) or not (false)
 * \return True if evaluation succeeded, false if an error was thrown
 */
bool MatlabEngineX::evaluateString(const TString &command, bool errorCatch)
{
    if(!m_engine){
        openEngine();
    }

    auto outBuf = std::make_shared<SBuf>();
    auto errBuf = std::make_shared<SBuf>();

    addCommand(command);

    try {
        m_engine->eval(convertUTF8StringToUTF16String(command), outBuf, errBuf);
    } catch (const MATLABExecutionException& e) {
        setErrorMessage(TString(e.what()));
        return false;
    }

    bool error = false;
    if(errorCatch) {
        auto text_ = errBuf->str();
        setErrorMessage(TString(convertUTF16StringToUTF8String(text_)));

        if(errorMessage().length() > 0) {
            error = true;
        }
    }

    return !error;
}

/*!
 * \brief Calls a function in Matlab and retrieves the output,
 *
 * Uses feval from the Matlab API.
 *
 * This is a good alternative to evaluateString to evaluate the problem model,
 * since the inputs can have different types. The current implementation doesn't
 * make use of this (useful) feature since all inputs are treated as doubles.
 * \todo replace "const TVector<double>& in" by "const TVector<IElementSPrt>& in".
 *
 * \param funcName
 * \param nOutputs
 * \param in
 * \param errorCatch
 * \return
 */
TVector<double> MatlabEngineX::evaluateFunction(const TString& funcName,
                                                const size_t nOutputs,
                                                const TVector<double>& in,
                                                bool errorCatch)
{
    if(!m_engine){
        openEngine();
    }

    ArrayFactory factory;
    TVector<Array> args;
    args.reserve(in.size());
    for(auto elem : in) {
        args.push_back(factory.createScalar<double>(elem));
    }

    auto outBuf = std::make_shared<SBuf>();
    auto errBuf = std::make_shared<SBuf>();

    addCommand(funcName);

    TVector<Array> rest;
    try {
        auto name = convertUTF8StringToUTF16String(funcName);
        rest = m_engine->feval(name, nOutputs, args, outBuf, errBuf);
    } catch (const MATLABExecutionException& e) {
        setErrorMessage(TString(e.what()));
        return TVector<double>(nOutputs);
    }

    if(errorCatch) {
        auto text_ = errBuf->str();
        setErrorMessage(TString(convertUTF16StringToUTF8String(text_)));
    }

    // format outputs

//    TString test1 = TString(convertUTF16StringToUTF8String(errBuf->str()));
//    TString test2 = TString(convertUTF16StringToUTF8String(outBuf->str()));

    TVector<double> res;
    for(auto r : rest) {
        for(auto elem : TypedArray<double>(r)) {
            res.push_back(elem);
        }
    }

    return res;
}

/*!
 * A single variable is placed into the MATLAB workspace. The variable name is
 * determined by \p name and its value is determined by \p value, workspace
 * variable has a corresponding MATLAB type to type of \p value.
 * \param name  The name of the workspace variable.
 * \param value The value of the variable.
 */
void MatlabEngineX::placeVariable(const TString &name, int value)
{
    if(!m_engine) {
        openEngine();
    }

    ArrayFactory factory;
    auto val = factory.createScalar<int>(value);
    m_engine->setVariable(convertUTF8StringToUTF16String(name), std::move(val));
}

//! \overload placeVariable(const TString &name, int value)
void MatlabEngineX::placeVariable(const TString &name, double value)
{
    if(!m_engine) {
        openEngine();
    }

    ArrayFactory factory;
    auto val = factory.createScalar<double>(value);
    m_engine->setVariable(convertUTF8StringToUTF16String(name), std::move(val));
}

//! \overload placeVariable(const TString &name, int value)
void MatlabEngineX::placeVariable(const TString &name, bool value)
{
    if(!m_engine) {
        openEngine();
    }

    ArrayFactory factory;
    auto val = factory.createScalar<bool>(value);
    m_engine->setVariable(convertUTF8StringToUTF16String(name), std::move(val));
}

//! \overload placeVariable(const TString &name, int value)
void MatlabEngineX::placeVariable(const TString &name, TComplex value)
{
    T_UNUSED(name);
    T_UNUSED(value);

    /// \todo
}

//! \overload placeVariable(const TString &name, int value)
void MatlabEngineX::placeVariable(const TString &name, TString value)
{
    T_UNUSED(name);
    T_UNUSED(value);

    /// \todo
}


/*!
 * \brief Place a column vector into the MATLAB workspace.
 *
 * A column vector named \p name is placed into the MATLAB workspace. This is
 * the same length as \p vec. The values in the workspace vector are the same
 * order as those in \p vec. The workspace variable has an equivalant MATLAB
 * type to the type of values in \p vec.
 * \param name
 * \param vec
 */
void MatlabEngineX::placeVectorColumn(const TString &name, const TVector<int>& vec)
{
    setMatrix<int>(name, vec, vec.size(), 1);
}

//! \overload placeVectorColumn(const TString &name, TVector<int> vec)
void MatlabEngineX::placeVectorColumn(const TString &name, const TVector<double>& vec)
{
    setMatrix<double>(name, vec, vec.size(), 1);
}

//! \overload placeVectorColumn(const TString &name, TVector<int> vec)
void MatlabEngineX::placeVectorColumn(const TString &name, const TVector<bool>& vec)
{
    setMatrix<bool>(name, vec, vec.size(), 1);
}

//! \overload placeVectorColumn(const TString &name, TVector<int> vec)
void MatlabEngineX::placeVectorColumn(const TString &name, const TVector<TComplex>& vec)
{
    T_UNUSED(name);
    T_UNUSED(vec);

    /// \todo
}

/*!
 * \brief Place a row vector into the MATLAB workspace.
 *
 * A row vector named \p name is placed into the MATLAB workspace. This is the
 * same length as \p vec. The values in the workspace vector are the same order
 * as those in \p vec. The workspace variable has an equivalant MATLAB type to
 * the type of values in \p vec.
 * \param name
 * \param vec
 */
void MatlabEngineX::placeVectorRow(const TString &name, const TVector<int>& vec)
{
    setMatrix<int>(name, vec, 1, vec.size());
}

//! \overload placeVectorRow(const TString &name, TVector<int> vec)
void MatlabEngineX::placeVectorRow(const TString &name, const TVector<double>& vec)
{
    setMatrix<double>(name, vec, 1, vec.size());
}

//! \overload placeVectorRow(const TString &name, TVector<int> vec)
void MatlabEngineX::placeVectorRow(const TString &name, const TVector<bool>& vec)
{
    setMatrix<bool>(name, vec, 1, vec.size());
}

//! \overload placeVectorRow(const TString &name, TVector<int> vec)
void MatlabEngineX::placeVectorRow(const TString &name, const TVector<TComplex>& vec)
{
    T_UNUSED(name);
    T_UNUSED(vec);

    /// \todo
}

/*!
 * \brief Place a 2D matrix into the MATLAB workspace.
 *
 * A 2D matrix \p mat is placed into the MATLAB workspace named \p name. \p mat
 * must be index by row-column order, i.e mat[row][col]. If all rows of \p mat
 * are not of the same length this operator fails silently, it returns without
 * having written any value to the workspace.
 * \param name  Name of the matrix in the workspace.
 * \param mat   A 2D Vector (Vector of Vectors) containing the matrix.
 */
void MatlabEngineX::placeMatrix(const TString &name,
                                const TVector<TVector<int>>& mat)
{
    if(!m_engine) {
        openEngine();
    }

    auto nRows = mat.size();
    auto nCols = mat[0].size();
    auto size = nRows*nCols;

    // copy the elements of the matrix into a vector in column-major order
    TVector<int> cmo;
    cmo.reserve(size);
    for(size_t i=0; i<nCols; i++) {
        for(size_t j=0; j<nRows; j++) {
            cmo.push_back(mat[j][i]);
        }
    }

    ArrayFactory factory;
    TypedArray<int> data = factory.createArray({ mat.size(), mat[0].size() },
                                               cmo.begin(), cmo.end());
    m_engine->setVariable(convertUTF8StringToUTF16String(name), data);
}

//! \overload
void MatlabEngineX::placeMatrix(const TString &name,
                                const TVector<TVector<double>>& mat)
{
    if(!m_engine) {
        openEngine();
    }

    auto nRows = mat.size();
    auto nCols = mat[0].size();
    auto size = nRows*nCols;

    // copy the elements of the matrix into a vector in column-major order
    TVector<double> cmo;
    cmo.reserve(size);
    for(size_t i=0; i<nCols; i++) {
        for(size_t j=0; j<nRows; j++) {
            cmo.push_back(mat[j][i]);
        }
    }

    ArrayFactory factory;
    TypedArray<double> data = factory.createArray({ mat.size(), mat[0].size() },
                                                   cmo.begin(), cmo.end());
    m_engine->setVariable(convertUTF8StringToUTF16String(name), data);
}

//! \overload
void MatlabEngineX::placeMatrix(const TString &name,
                                const TVector<TVector<bool>>& mat)
{
    if(!m_engine) {
        openEngine();
    }

    auto nRows = mat.size();
    auto nCols = mat[0].size();
    auto size = nRows*nCols;

    // copy the elements of the matrix into a vector in column-major order
    TVector<bool> cmo;
    cmo.reserve(size);
    for(size_t i=0; i<nCols; i++) {
        for(size_t j=0; j<nRows; j++) {
            cmo.push_back(mat[j][i]);
        }
    }

    ArrayFactory factory;
    TypedArray<bool> data = factory.createArray({ mat.size(), mat[0].size() },
                                                  cmo.begin(), cmo.end());
    m_engine->setVariable(convertUTF8StringToUTF16String(name), data);
}

//! \overload
void MatlabEngineX::placeMatrix(const TString &name, const TVector<TVector<TComplex>>& mat)
{
    T_UNUSED(name);
    T_UNUSED(mat);

    /// \todo
}

/*!
 * \brief Retreive a variable from the MATLAB workspace.
 *
 * Retreives the MATLAB workspace variable called \p name and places it into \p
 * value. Any existing value of \p value is overwritten. If no workspace variable
 * called \p name is found then \p value is unchanged.
 * \param[in] name      The name of the function to retreive from the workspace.
 * \param[out] value    A reference to where the workspace variable will be placed.
 */
bool MatlabEngineX::getWorkspaceVariable(const TString &name, int& value)
{
    return getValue<int>(name, value);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineX::getWorkspaceVariable(const TString &name, double& value)
{
    return getValue<double>(name, value);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineX::getWorkspaceVariable(const TString &name, bool& value)
{
    return getValue<bool>(name, value);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineX::getWorkspaceVariable(const TString &name, TComplex& value)
{
    T_UNUSED(name);
    T_UNUSED(value);

    /// \todo

    return true;
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineX::getWorkspaceVariable(const TString &name, TString& value)
{
    T_UNUSED(name);
    T_UNUSED(value);

    /// \todo

    return true;
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name,
                                                 TVector<int>& vec)
{
    return getVector<int>(name, vec);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name,
                                                 TVector<double>& vec)
{
    return getVector<double>(name, vec);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name,
                                                 TVector<bool>& vec)
{
    return getVector<bool>(name, vec);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name, TVector<TComplex>& vec)
{
    T_UNUSED(name);
    T_UNUSED(vec);

    /// \todo

    return TVector<int>();
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name,
                                                 TVector<TVector<int>>& mat)
{
    return getMatrix<int>(name, mat);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name,
                                                 TVector<TVector<double>>& mat)
{
    return getMatrix<double>(name, mat);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name,
                                                 TVector<TVector<bool>>& mat)
{
    return getMatrix<bool>(name, mat);
}

//! \overload getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineX::getWorkspaceVariable(const TString &name, TVector<TVector<TComplex>>& mat)
{
    T_UNUSED(name);
    T_UNUSED(mat);

    /// \todo

    return TVector<int>();
}


/*!
 * \brief Deprecated, his engine does not have an interactive state.
 * \param visible
 */
void MatlabEngineX::setInteractive(bool visible)
{
    T_UNUSED(visible);
}

/*!
 * \brief Deprecated, this engine does not have an interactive state.
 * \param visible
 */
bool MatlabEngineX::Interactive()
{
    return true;
}

} //namespace Tigon
