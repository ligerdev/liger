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
#ifndef MATLABENGINEX_H
#define MATLABENGINEX_H

#include <matlabplugin/Utils/IMatlabEngine.h>

#include "MatlabEngine.hpp"
#include "MatlabDataArray.hpp"

namespace Tigon {

using namespace matlab::engine;
using namespace matlab::data;
using SBuf = std::basic_stringbuf<char16_t>;

class LIGER_TIGON_EXPORT MatlabEngineX : public IMatlabEngine
{
    friend class MatlabPool;

public:
    bool evaluateString(const TString& command, bool errorCatch = true);
    TVector<double> evaluateFunction(const TString& funcName,
                                     const size_t nOutputs,
                                     const TVector<double>& in,
                                     bool errorCatch = true);
    void resetEngine();

    void setInteractive(bool visible);
    bool Interactive();

    void placeVariable(const TString &name, int value);
    void placeVariable(const TString &name, double value);
    void placeVariable(const TString &name, bool value);
    void placeVariable(const TString &name, TComplex value);
    void placeVariable(const TString &name, TString value);

    void placeVectorColumn(const TString &name, const TVector<int>& vec);
    void placeVectorColumn(const TString &name, const TVector<double>& vec);
    void placeVectorColumn(const TString &name, const TVector<bool>& vec);
    void placeVectorColumn(const TString &name, const TVector<TComplex>& vec);

    void placeVectorRow(const TString &name, const TVector<int>& vec);
    void placeVectorRow(const TString &name, const TVector<double>& vec);
    void placeVectorRow(const TString &name, const TVector<bool>& vec);
    void placeVectorRow(const TString &name, const TVector<TComplex>& vec);

    void placeMatrix(const TString &name, const TVector<TVector<int>>& mat);
    void placeMatrix(const TString &name, const TVector<TVector<double>>& mat);
    void placeMatrix(const TString &name, const TVector<TVector<bool>>& mat);
    void placeMatrix(const TString &name, const TVector<TVector<TComplex>>& mat);


    bool getWorkspaceVariable(const TString &name, int& value);
    bool getWorkspaceVariable(const TString &name, double& value);
    bool getWorkspaceVariable(const TString &name, bool& value);
    bool getWorkspaceVariable(const TString &name, TComplex& value);
    bool getWorkspaceVariable(const TString &name, TString& value);

    TVector<int> getWorkspaceVariable(const TString &name, TVector<int>& vec);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<double>& vec);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<bool>& vec);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TComplex>& vec);

    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<int>>& mat);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<double>>& mat);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<bool>>& mat);
    TVector<int> getWorkspaceVariable(const TString &name, TVector<TVector<TComplex>>& mat);

    ~MatlabEngineX();

private:
    MatlabEngineX();
    void openEngine();
    void closeEngine();

    template<typename T>
    bool getValue(const TString &name, T& val);
    template<typename T>
    TVector<int> getVector(const TString &name, TVector<T>& vec);
    template<typename T>
    TVector<int> getMatrix(const TString &name, TVector<TVector<T>>& mat);

    template<typename T>
    void setMatrix(const TString &name, const TVector<T>& source, size_t rows, size_t cols);
    template<typename T>
    void setMatrix(const TString &name, const TVector<TVector<T>>& source, size_t rows, size_t cols);
//    template<typename T>
//    void setMatrix(const TString &name, TVector<TComplex>& source, int rows, int cols);
//    template<typename T>
//    void setMatrix(const TString &name, TVector<TVector<TComplex>>& source, int rows, int cols);

    std::unique_ptr<MATLABEngine> m_engine;
};

/*!
 * \brief Place a 1D vector into the MATLAB workspace.
 *
 * Places a 1D columnwise matrix or vector into the MATLAB workspace, values
 * in\p source are unchanged.
 * \param name      Name of the matrix.
 * \param source    The matrix to be copied to the workspace.
 * \param rows      Number of rows in the matrix.
 * \param cols      Number of columns in the matrix.
 */
template<typename T>
void MatlabEngineX::setMatrix(const TString &name, const TVector<T>& source,
                              size_t rows, size_t cols)
{
    if(!m_engine) {
        openEngine();
    }

    ArrayFactory factory;
    TypedArray<T> data = factory.createArray({ rows, cols }, source.begin(), source.end());
    m_engine->setVariable(convertUTF8StringToUTF16String(name), std::move(data));
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
 */
template<typename T>
void MatlabEngineX::setMatrix(const TString &name, const TVector<TVector<T>>& source,
                              size_t rows, size_t cols)
{
    TVector<T> tmp;
    for(size_t c = 0; c < cols; c++) {
        for(size_t r = 0; r < rows; r++) {
            tmp.push_back(source[r][c]);
        }
    }

    setMatrix(name, tmp, rows, cols);
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
bool MatlabEngineX::getValue(const TString &name, T& val)
{
    if(!m_engine) {
        openEngine();
    }

    auto A = m_engine->getVariable(convertUTF8StringToUTF16String(name));
    auto d = A.getDimensions();

    if(d[0]>0) {
        val = static_cast<T>(A[0]);
        return true;
    }

    return false;
}

/*!
 * \overload getWorkspaceVariable(const TString &name, int& value)
 * \brief Retreives a 1D vector from the MATLAB workspace.
 *
 * Retrieves a 1D vector from the MATLAB workspace. Any existing value of \p vec is overwritten.
 * \param[in]   name    Name of the matrix in the workspace
 * \parma[out]  vec     1D TVector where matrix will be placed
 * \return      Dimensions of the retireved matrix (rows, cols)
 */
template<typename T>
TVector<int> MatlabEngineX::getVector(const TString &name, TVector<T>& vec)
{
    if(!m_engine) {
        openEngine();
    }

    auto A = m_engine->getVariable(convertUTF8StringToUTF16String(name));
    auto d = A.getDimensions();
    auto c = A.getType();

    TVector<int> dimensions;
    for(auto e : d) {
        dimensions.push_back(static_cast<int>(e));
    }

    switch(c) {
        case ArrayType::INT8:
        case ArrayType::UINT8:
        case ArrayType::INT16:
        case ArrayType::UINT16:
        case ArrayType::INT32:
        case ArrayType::UINT32:
        case ArrayType::INT64:
        case ArrayType::UINT64:
        {
            auto B = TypedArray<int>(A);

            vec.clear();
            vec.reserve(A.getNumberOfElements());
            for(auto e : B) {
                vec.push_back( static_cast<T>(e) );
            }
            break;
        }
        case ArrayType::DOUBLE:
        {
            auto B = TypedArray<double>(A);

            vec.clear();
            vec.reserve(A.getNumberOfElements());
            for(auto e : B) {
                vec.push_back( static_cast<T>(e) );
            }
            break;
        }
        case ArrayType::LOGICAL:
        {
            auto B = TypedArray<bool>(A);

            vec.clear();
            vec.reserve(A.getNumberOfElements());
            for(auto e : B) {
                vec.push_back( static_cast<T>(e) );
            }
            break;
        }
    }

    return dimensions;
}

/*!
 * \overload getWorkspaceVariable(const TString &name, int& value)
 * \brief Retreives a 2D matrix from the MATLAB workspace.
 *
 * Retrieves a 2D matrix from the MATLAB workspace. Any existing value of \p mat
 * is overwritten.
 * \param[in]   name    Name of the matrix in the workspace
 * \parma[out]  vec     2D TVector where matrix will be placed
 * \return      Dimensions of the retireved matrix (rows, cols)
 */
template<typename T>
TVector<int> MatlabEngineX::getMatrix(const TString &name, TVector<TVector<T>>& mat)
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


} // Tigon

#endif // MATLABENGINEX_H
