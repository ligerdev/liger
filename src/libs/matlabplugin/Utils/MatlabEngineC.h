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
#pragma once

//#include <tigon/tigon_global.h>
//#include <tigon/tigonconstants.h>

#include <matlabplugin/Utils/IMatlabEngine.h>

#include "engine.h"
#include "matrix.h"

// Disable the following warning
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wsign-compare" // -Wsign-compare
#endif

namespace Tigon {

/*!
 * \brief This class encapsulates access to the MATLAB engine.
 *
 * The MatlabEngine class provides access to an instance of MATLAB running on
 * the local machine. Multiple MatlabEngines are allowed and each MatlabEngine
 * object manages access to its own MATLAB process, a new process is created
 * for each new MatlabEngine.
 *
 * When using MatlabEngine for the first time it is important to set up the
 * MATLAB environment to work with API calls.
 *
 * ### Windows:
 * - Add to the PATH environment variable: \a matlabroot\\bin\\win64
 * - Register MATLAB as a com server
 *      + Start MATLAB as a user with administrator privilidges and run:
 *        ```!matlab -regserver```
 *
 * ### Mac:
 * Add MATLAB application to the PATH environment variable with:
 * - sudo launchctl config user path \a $PATH:matlabroot/bin
 * where matlabroot is the path to the root application folder of MATLAB,
 * usually /Applications/MATLAB_R20XX_X.app/.
 *
 * ### Linux:
 * - Add Matlab binary to PATH variable.
 * Optional (rpath properties should take care of this):
 * - setenv LD_LIBRARY_PATH \a matlabroot/bin/glnxa64:\a matlabroot/sys/os/glnxa64
 *
 * for more information see http://uk.mathworks.com/help/matlab/matlab_external/building-and-running-engine-applications-on-unix-operating-systems.html
 */
class LIGER_TIGON_EXPORT MatlabEngineC : public IMatlabEngine
{
    friend class MatlabPool;

public:
    bool evaluateString(const TString& command, bool errorCatch = true);
    void resetEngine();
    void setInteractive(bool visible);
    bool Interactive();

    void placeVariable(const TString &name, int value);
    void placeVariable(const TString &name, double value);
    void placeVariable(const TString &name, bool value);
    void placeVariable(const TString &name, TComplex value);
    void placeVariable(const TString &name, TString value);
    void placeVariable(const TString &name, mxArray* value);

    // Place a vector into the MATLAB workspace, can be a row or column vector.
    void placeVectorColumn(const TString &name, const TVector<int>& vec);
    void placeVectorColumn(const TString &name, const TVector<double>& vec);
    void placeVectorColumn(const TString &name, const TVector<bool>& vec);
    void placeVectorColumn(const TString &name, const TVector<TComplex>& vec);

    void placeVectorRow(const TString &name, const TVector<int>& vec);
    void placeVectorRow(const TString &name, const TVector<double>& vec);
    void placeVectorRow(const TString &name, const TVector<bool>& vec);
    void placeVectorRow(const TString &name, const TVector<TComplex>& vec);

    // Place a Matrix onto the MATLAB workspace
    void placeMatrix(const TString &name, const TVector<TVector<int>>& mat);
    void placeMatrix(const TString &name, const TVector<TVector<double>>& mat);
    void placeMatrix(const TString &name, const TVector<TVector<bool>>& mat);
    void placeMatrix(const TString &name, const TVector<TVector<TComplex>>& mat);

    // Get Variable overides
    bool getWorkspaceVariable(const TString &name, int& value);
    bool getWorkspaceVariable(const TString &name, double& value);
    bool getWorkspaceVariable(const TString &name, bool& value);
    bool getWorkspaceVariable(const TString &name, TComplex& value);
    bool getWorkspaceVariable(const TString &name, TString& value);
    bool getWorkspaceVariable(const TString &name, mxArray*& value);

    TVector<int> getWorkspaceVariable(const TString &name, TVector<int>& vec);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<double>& vec);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<bool>& vec);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TComplex>& vec);

    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<int>>& mat);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<double>>& mat);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<bool>>& mat);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<TComplex> >& mat);

    ~MatlabEngineC();

private:
    MatlabEngineC();
    void openEngine();
    void closeEngine();

    mxArray* getVariable(const TString &name);
    template<typename T>
    TVector<int> getVector(const TString &name, TVector<T>& vec);
    template<typename T>
    TVector<int> getMatrix(const TString &name, TVector<TVector<T> >& mat);
    template<typename T>
    bool getValue(const TString &name, T& val);

    template<typename T>
    void setMatrix(const TString &name, const TVector<T>& source, size_t rows, size_t cols, mxClassID type); // Primarily for vectors but can be used for flattened matrices
    // Override
    void setMatrix(const TString &name, const TVector<TComplex>& source, size_t rows, size_t cols, mxClassID type);
    template<typename T>
    void setMatrix(const TString &name, const TVector<TVector<T> >& source, size_t rows, size_t cols, mxClassID type);
    // Override
    void setMatrix(const TString &name, const TVector<TVector<TComplex> >& source, size_t rows, size_t cols, mxClassID type);

    Engine* m_engine; //!< Pointer to the MATLAB engine
};

/*!
 *\brief Place a 1D vector into the MATLAB workspace.
 *
 * Places a 1D columnwise matrix or vector into the MATLAB workspace, values in\p source are unchanged.
 * \param name      Name of the matrix.
 * \param source    The matrix to be copied to the workspace.
 * \param rows      Number of rows in the matrix.
 * \param cols      Number of columns in the matrix.
 * \param type      The MATLAB data type of the created matrix in the workspace.
 */
template<typename T>
void MatlabEngineC::setMatrix(const TString &name, const TVector<T>& source, size_t rows, size_t cols, mxClassID type)
{
    // For vectors or a 1D, column wise TVector source matrix.
    mxArray* tmp = mxCreateNumericMatrix(rows, cols, type, mxREAL);

    T* data = (T*)mxGetData(tmp);
    for(size_t i =0; i < source.size(); i++) {
        data[i] = source[i];
    }
    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

/*!
 * \overload
 * \brief Place a 2D vector into the MATLAB workspace.
 *
 * Places a 2D matrix into the MATLAB workspace, values in \p source are unchanged.
 * \param name      Name of the matrix.
 * \param source    The matrix to be copied to the workspace.
 * \param rows      Number of rows in the matrix.
 * \param cols      Number of columns in the matrix.
 * \param type      The MATLAB data type of the created matrix in the workspace.
 */
template<typename T>
void MatlabEngineC::setMatrix(const TString &name, const TVector<TVector<T>>& source, size_t rows, size_t cols, mxClassID type)
{
    // For 2D TVector Matrix indexed by [row][column]
    mxArray* tmp = mxCreateNumericMatrix(rows, cols, type, mxREAL);

    T* data = static_cast<T*>(mxGetData(tmp));

    for(int c = 0; c < cols; c++) {
        for(int r = 0; r < rows; r++) {
            *data = source[r][c];
            data++;
        }
    }
    placeVariable(name, tmp);
    mxDestroyArray(tmp);
}

/*!
 * \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
 * \brief Retrieves a vector from the workspace.
 *
 * Retrieves a 1D vector or matrix from the MATLAB workspace. Any existing value of \p vec is overwritten.
 * \param[in]   name    Name of the matrix in the workspace
 * \parma[out]  vec     1D TVector where matrix will be placed
 * \return      Dimensions of the retireved matrix (rows, cols)
 */
template<typename T>
TVector<int> MatlabEngineC::getVector(const TString &name, TVector<T>& vec)
{
    mxArray* tmp = getVariable(name);
    TVector<int> dimensions(2, 0);
    vec.clear();

    if(tmp != nullptr) {

        mwSize rows = mxGetM(tmp);
        mwSize cols = mxGetN(tmp);
        dimensions[0] = static_cast<int>(rows);
        dimensions[1] = static_cast<int>(cols);

        mwSize nElem = rows * cols;
        vec.resize(static_cast<int>(nElem));

        switch(mxGetClassID(tmp))
        {
        case mxINT32_CLASS:
        {
            int* intData = (int*)mxGetData(tmp);

            for(int i = 0; i < nElem; i++) {
                vec[i] = static_cast<T>(*intData);
                intData++;
            }
            break;
        }
        case mxINT16_CLASS:
        {
            short* shortData = (short*)mxGetData(tmp);

            for(int i = 0; i < nElem; i++) {
                vec[i] = static_cast<T>(*shortData);
                shortData++;
            }
            break;
        }
        case mxINT8_CLASS:
        {
            char* charData = (char*)mxGetData(tmp);

            for(int i = 0; i < nElem; i++) {
                vec[i] = static_cast<T>(*charData);
                charData++;
            }
            break;
        }
        case mxDOUBLE_CLASS:
        {
            double* dbData = mxGetPr(tmp);

            for(int i = 0; i < nElem; i++) {
                vec[i] = static_cast<T>(*dbData);
                dbData++;
            }
            break;
        }
        case mxSINGLE_CLASS:
        {
            float* fData = (float*)mxGetData(tmp);
            for(int i = 0; i < nElem; i++) {
                vec[i] = static_cast<T>(*fData);
                fData++;
            }
            break;
        }
        case mxLOGICAL_CLASS:
        {
            mxLogical* boolData = mxGetLogicals(tmp);

            for(int i = 0; i< nElem; i++) {
                vec[i] = static_cast<T>(*boolData);
                boolData++;
            }
            break;
        }
        default:
            // qDebug() << "Unrecognised data type";
            break;
        }

        mxDestroyArray(tmp);
    } else {
        // qDebug() << "Error: no variable " << name << " found.";
        mxDestroyArray(tmp);
    }
    return dimensions;
}

/*!
 * \overload MatlabEngine::getWorkspaceVariable(const TString &name, int& value)
 * \brief Retreives a 2D matrix from the MATLAB workspace.
 *
 * Retrieves a 2D matrix from the MATLAB workspace. Any existing value of \p mat
 * is overwritten.
 * \param[in]   name    Name of the matrix in the workspace
 * \parma[out]  vec     2D TVector where matrix will be placed
 * \return      Dimensions of the retireved matrix (rows, cols)
 */
template<typename T>
TVector<int> MatlabEngineC::getMatrix(const TString &name, TVector<TVector<T>>& mat)
{
    TVector<T> flatMat;
    // Get the matrix as a 1D columnwise TVector and convert to a 2D TVector
    TVector<int> dimensions = getWorkspaceVariable(name, flatMat);

    int rows = dimensions[0];
    int cols = dimensions[1];

    mat.resize(rows);

    for (int r = 0; r < rows; r++) {
        mat[r].resize(cols);
        for (int c = 0; c < cols; c++) {
            mat[r][c] = flatMat[(c*rows) + r];
        }
    }

    return dimensions;
}

/*!
 * \overload getWorkspaceVariable(const TString &name, int& value)
 * \brief Retrieves a value from the workspace.
 *
 * Retrieves a value from the MATLAB workspace. Any existing value of \p val is
 * overwritten.
 * \param[in]   name    Name of the variable in the workspace
 * \parma[out]  val     Where the retrieved value is placed
 * \return      True if the value could be retrieved successfully, or false otherwise.
 */
template<typename T>
bool MatlabEngineC::getValue(const TString &name, T& val)
{
    mxArray* tmp = getVariable(name);

    if(tmp!= nullptr) {
        if(mxGetNumberOfElements(tmp) == 1) {

            switch(mxGetClassID(tmp))
            {
            case mxINT32_CLASS:
            {
                int* intData = (int*)mxGetData(tmp);
                val = static_cast<T>(*intData);
                break;
            }
            case mxINT16_CLASS:
            {
                short* shortData = (short*)mxGetData(tmp);
                val = static_cast<T>(*shortData);
                break;
            }
            case mxINT8_CLASS:
            {
                char* charData = (char*)mxGetData(tmp);
                val = static_cast<T>(*charData);
                break;
            }
            case mxDOUBLE_CLASS:
            {
                double* dbData = mxGetPr(tmp);
                val = static_cast<T>(*dbData);
                break;
            }
            case mxSINGLE_CLASS:
            {
                float* fData = (float*)mxGetData(tmp);
                val = static_cast<T>(*fData);
                break;
            }
            case mxLOGICAL_CLASS:
            {
                mxLogical* boolData = mxGetLogicals(tmp);
                val = static_cast<T>(*boolData);
                break;
            }
            default:
                break;
            }

            mxDestroyArray(tmp);
        } else {
            mxDestroyArray(tmp);
            return false;
        }
    } else {
        mxDestroyArray(tmp);
        return false;
    }
    return true;
}

} // Tigon
