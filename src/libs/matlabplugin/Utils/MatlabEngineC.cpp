/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <matlabplugin/Utils/MatlabEngineC.h>

namespace Tigon {
/*!
 *\brief Create a MatlabEngine object.
 *
 * This constructor is private, only MatlabPool is allowed to create new MatlabEngine.
 * Lazy initialization is used so the MATLAB engine is not started until a command
 * is run or a variable placed.
 */
MatlabEngineC::MatlabEngineC()
    : m_engine(nullptr)
{
    // Lazy initialisation of the MATLAB Engine
}

/*!
 * \brief Destructor
 *
 * Destroys the MatlabEngine object and closes the MATLAB engine, stopping the MATLAB process.
 */
MatlabEngineC::~MatlabEngineC()
{
    closeEngine();
}

/*!
 * \brief Evaluate a string of commands in MATLAB
 *
 * Evaluate a string of commands in the MATLAB engine.
 * \param[in] command The string to be evaluated by MATLAB
 * \return True if evaluation succeeded, false if an error was thrown
 */
bool MatlabEngineC::evaluateString(const TString &command, bool errorCatch)
{
    if(!m_engine){
        openEngine();
    }

    TString com;
    if(errorCatch == true) {
        placeVariable("TigonError", 0);
        com += "try\n";
        com += command;
        com += "\ncatch ex\nTigonError=1;\nTigonErrorString = ex.message;\nend";
    } else {
        com += command;
    }

    addCommand(command);
    //std::cout <<"Matlab command: "<< command << std::endl;

    int errCode = engEvalString(m_engine, com.c_str());

    bool error = false;
    if (errCode == 0 ) { // errCode is 1 if the session is dead.
        if(errorCatch == true) {
            if(getWorkspaceVariable("TigonError", error)) {
                if(error) {
                    TString error_tmp;
                    getWorkspaceVariable("TigonErrorString", error_tmp);
                    setErrorMessage(error_tmp);
                    // qDebug() << TString("Error evaluating "
                    //                    + command + ": "
                    //                    + m_errorBuff);
                }
            } else {
                // TigonError has been cleared by the command do nothing
            }
        }

    } else {
        // MATLAB session has died, restart session and indicate error.
        closeEngine();
        openEngine();
        error = true;
    }

    return !error;
}

/*!
 * \brief Start the MATLAB engine
 *
 * Starts the MATLAB process and sets m_engine to point to the MATLAB engine. By default the engine is
 * invisible or not interactive, this can be changed by calling Interactive(bool visibile). This operation
 * is called once when an instance of MATLAB is first required, each MatlabEngine holds a reference to a
 * MATLAB engine which runs until the MatlabEngine object is destroyed.
 *
 * Parallelisable single instance is only available on Windows system
 */
void MatlabEngineC::openEngine()
{
    if(!m_engine) {
        m_engine = engOpen("");
        engSetVisible(m_engine, false);
    }
}

/*!
 * \brief Stop the MATLAB engine
 *
 * Stops the MATLAB process, closing the engine. \ref m_engine is reset to NULL.
 */
void MatlabEngineC::closeEngine()
{
    if(m_engine) {
        engClose(m_engine);
        m_engine = nullptr;
        clearCommandHistory();
    }
}

/*!
 * \brief Resets the engine to an initial state.
 *
 * This clears the MATLAB workspace and resets the visibility of the process to invisible. This operation is
 * primarily used by MatlabPool to ensure the pooled engines are in a known initial state.
 */
void MatlabEngineC::resetEngine()
{
    if(m_engine) {
        ///\note "clear all" was used to reset the engine. However, it sometimes
        /// causes crash of Simulink. Changed to clearvars for now.
        //evaluateString("clear all", 0);
        evaluateString("clearvars", 0);

        setInteractive(false);
        clearCommandHistory();
    }
}

/*!
 * \brief Set visibility of MATLAB engine.
 *
 * Allows the user of the class to determine if the MATLAB command windows and
 * any extra created windows should be visible or not.
 * \param[in] visible Boolean for whether the engine should be visible or not.
 */
void MatlabEngineC::setInteractive(bool visible)
{
    if(!m_engine) {
        openEngine();
    }

    engSetVisible(m_engine, visible);
}

/*!
 * \brief The visibility of the MaTLAB engine
 *
 * Allows the user of the class to check whether the MATLAB engine is currently visible.
 * If no engine is running then the default return value is false.
 * \return MATLAB engine visbility
 */
bool MatlabEngineC::Interactive()
{
    bool status;
    if(m_engine){
        engGetVisible(m_engine, &status);
    } else {
        status = false;
    }
    return status;
}

/*!
 * A single variable is placed into the MATLAB workspace. The variable name is determined by \p name and its value is determined by \p value, workspace variable
 * has a corresponding MATLAB type to type of \p value.
 * \param[in] name  The name of the workspace variable.
 * \param[in] value The value of the variable.
 */
void MatlabEngineC::placeVariable(const TString &name, int value)
{
    mxArray* tmp = mxCreateNumericMatrix(1,1,mxINT32_CLASS, mxREAL); //create a 1x1 matrix of integers
    *(int*)mxGetData(tmp) = value;

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

//! \overload MatlabEngine::placeVariable(const TString &name, int value)
void MatlabEngineC::placeVariable(const TString &name, double value)
{
    mxArray* tmp = mxCreateDoubleScalar(value);

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

//! \overload MatlabEngine::placeVariable(const TString &name, int value)
void MatlabEngineC::placeVariable(const TString &name, bool value)
{
    mxArray* tmp = mxCreateLogicalScalar(value);

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

//! \overload MatlabEngine::placeVariable(const TString &name, int value)
void MatlabEngineC::placeVariable(const TString &name, TComplex value)
{
    mxArray* tmp = mxCreateNumericMatrix_730(1, 1, mxDOUBLE_CLASS, mxCOMPLEX);
    double* pr =  mxGetPr(tmp) ;
    *pr = value.real();
    double* pi =  mxGetPi(tmp);
    *pi = value.imag();

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

//! \overload MatlabEngine::placeVariable(const TString &name, int value)
void MatlabEngineC::placeVariable(const TString &name, TString value)
{
    mxArray* tmp = mxCreateString(value.c_str());

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

/*!
 * \overload MatlabEngine::placeVariable(const TString &name, int value)
 * \brief Put a pre-created mxArray into the MATLAB workspace
 *
 * This provides direct access to place a MATLAB mxArray into the MATLAB workspace. mxArray is defined as part of the MATLAB API, it is
 * the data type utilised by all MATLAB MEX functions to interface with MATLAB. An mxArray is allocated in memory, as this function does
 * not create the array it does not destory the mxArray. The creator of the array should call the MATLAB API function mxDestroyArray().
 * \param[in] name  The name of the variable
 * \param[in] value The MATLAB mxArray to be placed into the workspace
 */
void MatlabEngineC::placeVariable(const TString &name, mxArray* value)
{
    if(!m_engine) {
        openEngine();
    }

    if(value != nullptr) {
        engPutVariable(m_engine, name.c_str(), value);
    }
}

/*!
 * \brief Place a column vector into the MATLAB workspace.
 *
 * A column vector named \p name is placed into the MATLAB workspace. This is the same length as \p vec. The values in
 * the workspace vector are the same order as those in \p vec. The workspace variable has an equivalant MATALAB type to the type of values in
 * \p vec.
 * \param[in] name
 * \param[in] vec
 */
void MatlabEngineC::placeVectorColumn(const TString &name, const TVector<int>& vec)
{
    setMatrix<int>(name, vec, vec.size(), 1, mxINT32_CLASS);
}

/*!
 * \brief Place a row vector into the MATLAB workspace.
 *
 * A row vector named \p name is placed into the MATLAB workspace. This is the same length as \p vec. The values in
 * the workspace vector are the same order as those in \p vec. The workspace variable has an equivalant MATALAB type to the type of values in
 * \p vec.
 * \param[in] name
 * \param[in] vec
 */
void MatlabEngineC::placeVectorRow(const TString &name, const TVector<int>& vec)
{
    setMatrix<int>(name, vec, 1, vec.size(), mxINT32_CLASS);
}

//! \overload MatlabEngine::placeVectorColumn(const TString &name, TVector<int> vec)
void MatlabEngineC::placeVectorColumn(const TString &name, const TVector<double>& vec)
{
    setMatrix<double>(name, vec, vec.size(), 1, mxDOUBLE_CLASS);
}

//! \overload MatlabEngine::placeVectorRow(const TString &name, TVector<int> vec)
void MatlabEngineC::placeVectorRow(const TString &name, const TVector<double>& vec)
{
    setMatrix<double>(name, vec, 1, vec.size(), mxDOUBLE_CLASS);
}

//! \overload MatlabEngine::placeVectorColumn(const TString &name, TVector<int> vec)
void MatlabEngineC::placeVectorColumn(const TString &name, const TVector<bool>& vec)
{
    setMatrix<bool>(name, vec, vec.size(), 1, mxLOGICAL_CLASS);
}

//! \overload MatlabEngine::placeVectorRow(const TString &name, TVector<int> vec)
void MatlabEngineC::placeVectorRow(const TString &name, const TVector<bool>& vec)
{
    setMatrix<bool>(name, vec, 1, vec.size(), mxLOGICAL_CLASS);
}

//! \overload MatlabEngine::placeVectorColumn(const TString &name, TVector<int> vec)
void MatlabEngineC::placeVectorColumn(const TString &name, const TVector<TComplex>& vec)
{
    setMatrix(name, vec, vec.size(), 1, mxDOUBLE_CLASS);
}

//! \overload MatlabEngine::placeVectorRow(const TString &name, TVector<int> vec)
void MatlabEngineC::placeVectorRow(const TString &name, const TVector<TComplex>& vec)
{
    setMatrix(name, vec, 1, vec.size(), mxDOUBLE_CLASS);
}

/*!
 * \overload MatlabEngine::setMatrix
 *
 * This provides an override of the \ref setMatrix operator for complex numbers.
 */
void MatlabEngineC::setMatrix(const TString &name, const TVector<TComplex>& source, size_t rows, size_t cols, mxClassID type)
{
    mxArray* tmp = mxCreateNumericMatrix(rows, cols, type, mxCOMPLEX);

    double* pr = mxGetPr(tmp);
    double* pi = mxGetPi(tmp);

    for(size_t i =0; i < source.size(); i++) {
        pr[i] = source[i].real();
        pi[i] = source[i].imag();
    }

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

/*!
 * \overload MatlabEngine::setMatrix
 *
 *
 * This provides an override of the \ref MatlabEngine::setMatrix operator for complex numbers.
 */
void MatlabEngineC::setMatrix(const TString &name, const TVector<TVector<TComplex>>& source, size_t rows, size_t cols, mxClassID type)
{
    mxArray* tmp = mxCreateNumericMatrix(rows, cols, type, mxCOMPLEX);

    double* pr = mxGetPr(tmp);
    double* pi = mxGetPi(tmp);

    for(size_t c = 0; c < cols; c++) {
        for(size_t r = 0; r < rows; r++) {
            *pr = source[r][c].real();
            *pi = source[r][c].imag();

            pr++;
            pi++;
        }
    }

    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

/*!
 * \overload template < T > TVector< int > Tigon::MatlabEngine::getWorkspaceVariable(const TString &name, TVector< T > &vec)
 *
 * This provides a specialization of the \ref getWorkspaceVariable operator for complex numbers.
 */
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<TComplex>& vec)
{
    mxArray* tmp = getVariable(name);
    TVector<int> dimensions(2,0);

    if(tmp != nullptr) {
        mwSize rows = mxGetM(tmp);
        mwSize cols = mxGetN(tmp);
        dimensions[0] = static_cast<int>(rows);
        dimensions[1] = static_cast<int>(cols);

        mwSize nElem = rows * cols;
        vec.resize((int)nElem);

        if(mxIsComplex(tmp)) {
            // tmp is a complex matrix
            double* pr = mxGetPr(tmp);
            double* pi = mxGetPi(tmp);

            for(int i = 0;i< nElem; i++) {
                vec[i].real(*pr);
                vec[i].imag(*pi);
                pr++;
                pi++;
            }
        } else {
            // tmp is not a complex matrix
            double* pr = mxGetPr(tmp);

            for(int i=0; i<nElem; i++) {
                vec[i].real(*pr);
                pr++;
            }
        }
        mxDestroyArray(tmp);
    } else {
        // // qDebug() << "Error: no variable " << name << "found.";
        mxDestroyArray(tmp);
    }
    return dimensions;
}

/*!
 * \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
 *
 * \brief Retreive a 2D matrix from the workspace
 *
 * The matrix fetch function calls \ref MatlabEngine::getMatrix. These functions hide the template
 * functions so that the handled types are controlled.
 */
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<TVector<int>>& mat)
{
    return getMatrix<int>(name, mat);
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<TVector<double>>& mat)
{
    return getMatrix<double>(name, mat);
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<TVector<bool>>& mat)
{
    return getMatrix<bool>(name, mat);
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<TVector<TComplex>>& mat)
{
    mxArray* tmp = getVariable(name);
    TVector<int> dimensions(2, 0);
    mat.clear();

    if(tmp != nullptr) {
        mwSize rows = mxGetM(tmp);
        mwSize cols = mxGetN(tmp);
        dimensions[0] = static_cast<int>(rows);
        dimensions[1] = static_cast<int>(cols);

        mat.resize((int)rows);

        if(mxIsComplex(tmp)) {
            // tmp is a complex matrix
            double* pr = mxGetPr(tmp);
            double* pi = mxGetPi(tmp);

            for (int r = 0; r < rows; r++) {
                mat[r].resize((int)cols);
                for (int c = 0; c < cols; c++) {
                    mat[r][c].real(pr[(c*rows) + r]);
                    mat[r][c].imag(pi[(c*rows) + r]);
                }
            }
        } else {
            // tmp is not a complex matrix
            double* pr = mxGetPr(tmp);

            for (int r = 0; r < rows; r++) {
                mat[r].resize((int)cols);
                for (int c = 0; c < cols; c++) {
                    mat[r][c].real(pr[(c*rows) + r]);
                }
            }
        }
        mxDestroyArray(tmp);
    } else {
        // // qDebug() << "Error: no variable " << name << "found.";
        mxDestroyArray(tmp);
    }
    return dimensions;
}

/*!
 * \brief Place a 2D matrix into the MATLAB workspace.
 *
 * A 2D matrix \p mat is placed into the MATLAB workspace named \p name. \p mat must be index by row-column order, i.e mat[row][col].
 * If all rows of \p mat are not of the same length this operator fails silently, it returns without having written any value to the workspace.
 * \param name  Name of the matrix in the workspace.
 * \param mat   A 2D Vector (Vector of Vectors) containing the matrix.
 */
void MatlabEngineC::placeMatrix(const TString &name, const TVector<TVector<int>>& mat)
{
    auto rows = mat.size();
    if(rows > 0) {
        auto cols = mat[0].size();

        // Check that all columns are the same length
        for(size_t r = 1; r < rows; r++) {
            if(mat[r].size() != cols){
                // // qDebug() << "MatlabEngine::placeMatrix(): unequal coloumn length";
                return;
            }
        }

        setMatrix<int>(name, mat, rows, cols, mxINT32_CLASS);
    }
}

//! \overload
void MatlabEngineC::placeMatrix(const TString &name, const TVector<TVector<double>>& mat)
{
    auto rows = mat.size();
    if(rows > 0) {
        auto cols = mat[0].size();

        // Check that all columns are the same length
        for(size_t r = 1; r < rows; r++) {
            if(mat[r].size() != cols){
                // qDebug() << "MatlabEngine::placeMatrix(): unequal coloumn length";
                return;
            }
        }

        setMatrix<double>(name, mat, rows, cols, mxDOUBLE_CLASS);
    }
}

//! \overload
void MatlabEngineC::placeMatrix(const TString &name, const TVector<TVector<bool>>& mat)
{
    auto rows = mat.size();
    if(rows > 0) {
        auto cols = mat[0].size();

        // Check that all columns are the same length
        for(size_t r = 1; r < rows; r++) {
            if(mat[r].size() != cols){
                // qDebug() << "MatlabEngine::placeMatrix(): unequal coloumn length";
                return;
            }
        }

        setMatrix<bool>(name, mat, rows, cols, mxLOGICAL_CLASS);
    }
}

//! \overload
void MatlabEngineC::placeMatrix(const TString &name, const TVector<TVector<TComplex>>& mat)
{
    auto rows = mat.size();
    if(rows > 0) {
        auto cols = mat[0].size();

        // Check that all columns are the same length
        for(size_t r = 1; r < rows; r++) {
            if(mat[r].size() != cols){
                // qDebug()"MatlabEngine::placeMatrix(): unequal coloumn length";
                return;
            }
        }

        setMatrix(name, mat, rows, cols, mxDOUBLE_CLASS);
    }
}

/*!
 * \brief Retreive a variable from the MATLAB workspace.
 *
 * Retreives the MATLAB workspace variable called \p name and places it into \p value. Any existing value of \p value is
 * overwritten. If no workspace variable called \p name is found then \p value is unchanged.
 * \param[in] name      The name of the function to retreive from the workspace.
 * \param[out] value    A reference to where the workspace variable will be placed.
 */
bool MatlabEngineC::getWorkspaceVariable(const TString &name, int& value)
{
    return getValue<int>(name, value);
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineC::getWorkspaceVariable(const TString &name, double& value)
{
    return getValue<double>(name, value);
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineC::getWorkspaceVariable(const TString &name, bool& value)
{
    return getValue<bool>(name, value);
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineC::getWorkspaceVariable(const TString &name, TComplex& value)
{
    mxArray* tmp = getVariable(name);
    if(tmp != nullptr) {
        if(mxGetNumberOfElements(tmp) == 1) {
            if(mxIsComplex(tmp)) {

                value.real(*mxGetPr(tmp));
                value.imag(*mxGetPi(tmp));
                mxDestroyArray(tmp);
            } else {
                // ERROR tmp is not complex, fetch real part
                mxDestroyArray(tmp);
                double pr;
                getValue<double>(name, pr);
                value.real(pr);
            }
        } else {
            // qDebug() << name << " is not a single value. Use MatlabEngine::getVector instead.";
            mxDestroyArray(tmp);
            return false;
        }
    } else {
        // qDebug() <<  "Error no variable of that name: " << name;
        mxDestroyArray(tmp); // for safety
        return false;
    }
    return true;
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineC::getWorkspaceVariable(const TString &name, TString& value)
{
    mxArray* tmp = getVariable(name);

    if(tmp != nullptr) {
        if(mxIsChar(tmp)) {
            value = TString(mxArrayToString(tmp));
            mxDestroyArray(tmp);
        } else {
            // ERROR tmp is not a logical
            mxDestroyArray(tmp);
            return false;
        }
    } else {
        // ERROR no variable of that name
        mxDestroyArray(tmp); // for safety
        return false;
    }
    return true;
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
bool MatlabEngineC::getWorkspaceVariable(const TString &name, mxArray*& value)
{
    value = getVariable(name);
    if(value) {
        return true;
    } else {
        return false;
    }
}

//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<int>& vec)
{
    return getVector<int>(name, vec);
}
//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<double>& vec)
{
    return getVector<double>(name, vec);
}
//! \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
TVector<int> MatlabEngineC::getWorkspaceVariable(const TString &name, TVector<bool>& vec)
{
    return getVector<bool>(name, vec);
}

/*!
 * \brief Retreive a variable from the MATLAB workspace
 *
 * Returns an mxArray pointer to the variable in the MATLAB workspace. Returns NULL on failure.
 *
 * \param name  The name of the variable in the MATLAB workspace
 * \return      A pointer to \p name
 */
mxArray* MatlabEngineC::getVariable(const TString &name)
{
    if(!m_engine) {
        openEngine();
    }

    if(m_engine) {
        mxArray* tmp = engGetVariable(m_engine, name.c_str());

        return tmp;
    } else {
        //ERROR - engine failed to open
        return nullptr;
    }
}

} //namespace Tigon
