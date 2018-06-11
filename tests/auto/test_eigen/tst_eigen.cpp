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
#include <QtTest>
#include <QDebug>

#include <iostream>

// Eigen specific headers
#include <eigen/Eigen>
#include <eigen/QR>
#include <eigen/bench/BenchTimer.h>

//*************************************//
using namespace Eigen;
using namespace std;

typedef MatrixXd mXd;
typedef VectorXd vXd;
typedef Matrix<double, Dynamic, Dynamic, RowMajor> rmXd;

// Comment out the following line if SVD needs to be tested
#define FAST_EIGEN_TEST

//TESTED_COMPONENT=src/libs/eigen/....
class tst_eigen : public QObject
{
    Q_OBJECT

public:
    void set_eps(double eps)
    {
        m_eps = eps;
    }

private slots:
    void test_initEPS();

    void test_MatrixIndexing();
    void test_SubMatrix();

    void test_VectorIndexing();
    void test_VectorSlicing();

    void test_Matrix2VectorMultiplication();
    void test_Matrix2MatrixMultiplication();

    void test_VectorElementarywiseProduct();

    /*
     * Find the min/max of a vector
     */
    void test_VectorMinMax();

    /*
     * Find min/max of each column of a matrix;
     * find min/max of the whole matrix
     */
    void test_MatrixMinMax();

    /*
     * Gnerate a random matrix and check if it is
     * nonsingular
     */
    void test_RandomMatrixGeneration();

    /*
     * Copy data from a cpp array to Eigen and backwards.
     * Note that the default layout of Eigen data is
     * column major.
     */
    void test_CppArray2EigenData();
    void test_EigenData2CppArray();
    void test_QVector2EigenData();

    /*
     * Eigenvalues module
     */
    void test_EigenvaluesEigenvectors();

    /*
     * LU module
     */
    void test_FullPivLU();
    void test_PartialPivLU();

    /*
     * QR mnodule
     */
    void test_HouseholderQR();
    void test_FullPivHouseholderQR();
    void test_ColPivHouseholderQR();

    /*
     * Cholesky module
     */
    void test_LLT();  //  matrix needs to be positive definite
    void test_LDLT(); //  matrix needs to be positive or negative semidefinite

    /*
     * SVD module
     */
    void test_SVD();  //  slow

    /*
     * Test FullPivLu, HouseholderPivLu, ColPivHouseholderQR,
     * FullPivHouseholderQR on large test problems,
     * in the aim of comparing accuracy
     */
    void test_MatrixFactorisationAccuracyLargeNonsingular();
    void test_MatrixFactorisationAccuracyLargeSingular();

private:
    double m_eps;

    // Generate a fixed small 5*5 test problem; not random
    void smallExample5x5(mXd &X, vXd &b, vXd &sol);

    // Generate a random test problem
    void randomExample(mXd &X, vXd &b, int m, int n);

    // return true if |num1 - num2| < m_eps; otherwise false
    bool verifyApprox(double num1, double num2) const;

    // return true if the absolute value of every element of (v1 - v2) is less than m_eps; otherwise false
    bool verifyVecApprox(vXd v1, vXd v2) const;

    // return true if the absolute value of (M1 - M2) is less than m_eps; otherwise false
    bool verifyMatApprox(mXd M1, mXd M2) const;

    // return true if the 2-norm of (v1 - v2) is less than m_eps; otherwise false
    bool verifyVecApproxLarge(vXd v1, vXd v2, double &residual) const;

    // calculate the resiudal of a least-squares solution
    double leastSquaresResidual(mXd A, vXd b, vXd sol) const;
};

void tst_eigen::test_initEPS()
{
    set_eps(1e-12);
}

void tst_eigen::test_MatrixIndexing()
{
    mXd X(3,2);
    X <<    1, 2.0,
            3, 4,
            5, 6.0;

    QCOMPARE(verifyApprox(X(1,1), 4), true);
}

void tst_eigen::test_SubMatrix()
{
    int rowBegin = 0;
    int colBegin = 2;
    int m = 2;     // number of rows in the block (submatrix)
    int n = 1;     // number of cols in the block (submatrix)

    mXd X(4,4);
    X <<    1, 2, 3, 4,
            5, 6, 7, 8,
            9, 10,11,12,
            13,14,15,16;

    mXd subXTrue(m,n);

    for (int i=rowBegin; i<m+rowBegin; i++)
        for (int j=colBegin; j<n+colBegin; j++)
            subXTrue(i-rowBegin, j-colBegin) = X(i,j);

    // create the submatrix
    // matrix.block(i,j,m,n) : Block of size (m,n), starting at (i,j)
    mXd subX = X.block(rowBegin, colBegin, m, n);

    QCOMPARE(verifyMatApprox(subX, subXTrue), true);

    // Test: join two submatrix vertically
    mXd xTop = X.block(0, 0, 1, 4);      // the first row
    mXd xBottom = X.block(1, 0, 3, 4);   // row 2 to row 4
    mXd joinVertically(4,4);
    joinVertically << xTop,
                      xBottom;

    QCOMPARE(verifyMatApprox(joinVertically, X), true);

    // Test: join two submatrix horizontally
    mXd xLeft = X.block(0,  0, 4, 3);    // the first 3 cols
    mXd xRight = X.block(0, 3, 4, 1);    // the fourth col
    mXd joinHorizontally(4,4);
    joinHorizontally << xLeft, xRight;

    QCOMPARE(verifyMatApprox(joinHorizontally, X), true);
}

void tst_eigen::test_VectorIndexing()
{
    vXd v(3);
    v << 1,2,3;
    QCOMPARE(verifyApprox(v(2), 3), true);
}

void tst_eigen::test_VectorSlicing()
{
    int start = 2;    // starting index
    int n = 3;        // length of the subvector

    vXd v(8);
    v << 1, 2, 3, 4, 5, 6, 7, 8;

    vXd subVecTrue(n);

    for (int i =0; i<n; i++)
        subVecTrue(i) = v(i+start);

    vXd subVec = v.segment(start,n);

    QCOMPARE(verifyVecApprox(subVecTrue, subVec), true);
}

void tst_eigen::test_Matrix2VectorMultiplication()
{
    vXd v(5);
    mXd X(3,5);

    X <<    1, 2, 3, 4, 5,
            6, 7, 8, 9, 10,
            11, 12, 13, 14, 15;

    v << 2, 7, 3, 7, 6;

    vXd sol(3);
    sol<<83, 208, 333;

    QCOMPARE(verifyVecApprox(X*v, sol), true);
}

void tst_eigen::test_Matrix2MatrixMultiplication()
{
    int m = 3;
    int n = m;

    mXd M1(m,n);
    mXd M2(m,n);

    M1<<    0,13,6,
            9, 0, 6,
            5, 6,11;

    M2<<    9, 1,13,
            22, 5, 4,
            13, 20, 9;

    mXd sol(m,n);
    sol<<   364,   185,   106,
            159,   129,   171,
            320,   255,   188;

    QCOMPARE(verifyMatApprox(M1*M2, sol),true);
}

void tst_eigen::test_VectorElementarywiseProduct()
{
    vXd v1(3);
    vXd v2(3);
    vXd sol(3);

    v1  << 1,   2,  3;
    v2  << 4, 5.0,  6;
    sol << 4,  10, 18;

    QCOMPARE(verifyVecApprox(v1.cwiseProduct(v2), sol), true);
}

void tst_eigen::test_VectorMinMax()
{
    vXd v(10);
    v << 1, 2, 9, 4, 5, 6, 7, 8, 3, 0;

    // check the min of the vector
    QCOMPARE(verifyApprox(v.minCoeff(), 0), true);

    // check the max of the vector
    QCOMPARE(verifyApprox(v.maxCoeff(), 9), true);

    int i;
    double s;

    s = v.minCoeff(&i);
    //check the index of min
    QCOMPARE(i == 9, true);

    s = v.maxCoeff(&i);
    // check the index of max
    QCOMPARE(i == 2, true);
}

void tst_eigen::test_MatrixMinMax()
{
    mXd X(3,3);
    X <<    9, 1,13,
            22, 5, 4,
            13, 20, 9;

    // actual min/max of each col/row
    vXd minColVecTrue(3);
    vXd maxColVecTrue(3);
    vXd minRowVecTrue(3);
    vXd maxRowVecTrue(3);

    minColVecTrue << 9, 1, 4;
    maxColVecTrue << 22, 20, 13;
    minRowVecTrue << 1, 4, 9;
    maxRowVecTrue << 13, 22, 20;

    vXd minColVec = X.colwise().minCoeff();
    vXd maxColVec = X.colwise().maxCoeff();
    vXd minRowVec = X.rowwise().minCoeff();
    vXd maxRowVec = X.rowwise().maxCoeff();

    cout<< "Matrix X = " << endl << X << endl;
    cout<< "Min of each column :" << endl << minColVec.transpose() << endl << endl;
    QCOMPARE(verifyVecApprox(minColVec, minColVecTrue), true);

    cout<< "Max of each column :" << endl << maxColVec.transpose() << endl << endl;
    QCOMPARE(verifyVecApprox(maxColVec, maxColVecTrue), true);

    cout<< "Min of each row    :" << endl << minRowVec             << endl << endl;
    QCOMPARE(verifyVecApprox(minRowVec, minRowVecTrue), true);

    cout<< "Max of each row    :" << endl << maxRowVec             << endl << endl;
    QCOMPARE(verifyVecApprox(maxRowVec, maxRowVecTrue), true);
}

void tst_eigen::test_RandomMatrixGeneration()
{
    int m = 100;
    int n = m;

    mXd X = mXd::Random(m,n);
    FullPivLU<mXd> lu(X);

    // Check is the matrix X is invertable using LU factorization.
    // A randomly generated square should be nonsingular.
    QCOMPARE(lu.isInvertible(), true);

}

void tst_eigen::test_CppArray2EigenData()
{
    /* Copy data from a cpp array
     * to Eigen. Note that the default layout
     * of Eigen data is column major
     */

    int m = 3;
    int n = 5;

    int l = m*n;
    double array[15];

    cout<<"\nCpp array = ";
    for (int i=0; i<l; i++)
    {
        array[i] = i+1;
        cout<<array[i]<<" ";
    }
    cout<<endl;


    vXd v(l);
    for (int i=0; i<l; i++)
        v(i) = i+1;

    mXd matrixRowMajor(m,n), matrixColMajor(m,n);

    for (int i=0; i<l;i++)
    {
        matrixRowMajor(i/n, i%n) = array[i];
        matrixColMajor(i%m, i/m) = array[i];
    }

    // eigen's default layout is column major
    Map<mXd>  mapColMajor(array, m, n);
    Map<rmXd> mapRowMajor(array, m, n);
    Map<vXd>  mapV(array, m*n);

    cout<<"mapRowMajor: \n"<<mapRowMajor<<endl;
    cout<<"mapColMajor: \n"<<mapColMajor<<endl;
    cout<<"mapV:        \n"<<mapV<<endl;

    QCOMPARE(verifyMatApprox(mapColMajor, matrixColMajor), true);
    QCOMPARE(verifyMatApprox(mapRowMajor, matrixRowMajor), true);
    QCOMPARE(verifyVecApprox(mapV, v), true);
}

void tst_eigen::test_EigenData2CppArray()
{
    int m = 3;
    int n = 5;
    int l = m*n;

    rmXd matrixRowMajor(m,n);
    for (int i=0; i<m;i++)
        for (int j=0; j<n;j++)
            matrixRowMajor(i,j) = i+j;

    cout<<"\nmatrixRowMajor: \n"<<matrixRowMajor<<endl;

    double* arrayFromRowMajor = matrixRowMajor.data();
    // member function data() will return a pointer to
    // where the raw data is being stored

    cout<< "C++ array: ";
    for (int i=0; i<l; i++)
        cout<<arrayFromRowMajor[i]<<" ";
    cout<<endl;

    mXd matrixColMajor(m,n);
    for (int i=0; i<m; i++)
        for (int j=0; j<n;j++)
            matrixColMajor(i,j) = i + j;
    cout<<"\nmatrixColMajor: \n"<< matrixColMajor <<endl;

    double* arrayFromColMajor = matrixColMajor.data();
    cout<< "C++ array: ";
    for (int i=0; i<l; i++)
        cout<<arrayFromColMajor[i]<<" ";
    cout<<endl;

    bool flag = true;

    for (int i=0; i < l; i++)
    {
        if (!verifyApprox(arrayFromRowMajor[i], matrixRowMajor(i / n, i % n))
                || !verifyApprox(arrayFromColMajor[i], matrixColMajor(i % m, i / m)))
        {
            flag = false;
            break;
        }
    }

    QCOMPARE(flag, true);
}

void tst_eigen::test_QVector2EigenData()
{
    using namespace std;

    int m = 2;
    int n = 3;
    QVector<qreal> M(m*n);
    for(int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++) {
            M[m*j+i] = m*j+i;
        }
    }
    for(int i = 0; i < M.length(); i++) {
        cout << M[i] << endl;
    }

    Map<MatrixXd> eigenM(M.data(),m,n);
    cout << "eigenM: " << endl;
    cout << eigenM << endl;

    eigenM(0,0) = 100;

    cout << "eigenM: " << endl;
    cout << eigenM << endl;

    for(int i = 0; i < M.length(); i++) {
        cout << M[i] << endl;
    }
}

void tst_eigen::test_EigenvaluesEigenvectors()
{
    // Note that Eigen gives complex eigenvalues,
    // Namely, we should use complex vectors/matrices
    // to get eigenvalues/vectors
    int m = 5;
    int n = m;

    mXd X = mXd::Random(m, n);

    VectorXcd eigenValues;

    EigenSolver<MatrixXd> es(X);
    eigenValues = es.eigenvalues();
    cout << "The eigenvalues of X are:"<< endl << eigenValues << endl << endl;

    MatrixXcd D = es.eigenvalues().asDiagonal(); // a diagonal matrix D of eigenvalues
    MatrixXcd V = es.eigenvectors();             // full matrix V whose columns are the corresponding eigenvectors
    cout<< "Diagnal matrix of eigenvalues D = " << endl << D << endl << endl;
    cout<< "Full matrix of eigenvectors   V = " << endl << V << endl << endl;

    MatrixXcd resultMatrix = V * D * V.inverse();   // This will be a complex matrix
    cout << "V * D * V^(-1) = " << endl << resultMatrix << endl;

    // The real part of V * D * V^(-1) should be the same as X
    // The complex part of it should be approximately zero matrix
    mXd zeroMatrix = mXd::Zero(m, n);
    QCOMPARE(verifyMatApprox(resultMatrix.real(), X), true);
    QCOMPARE(verifyMatApprox(resultMatrix.imag(), zeroMatrix), true);

}

void tst_eigen::test_FullPivLU()
{
    // no requirement on matrix

    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    QCOMPARE( verifyVecApprox(X.fullPivLu().solve(b), sol), true );
}

void tst_eigen::test_PartialPivLU()
{
    // require matrix to be invertable

    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    QCOMPARE( verifyVecApprox(X.partialPivLu().solve(b), sol), true );
}

void tst_eigen::test_HouseholderQR()
{
    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    QCOMPARE( verifyVecApprox(X.householderQr().solve(b), sol), true );
}

void tst_eigen::test_FullPivHouseholderQR()
{
    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    QCOMPARE( verifyVecApprox(X.fullPivHouseholderQr().solve(b), sol), true );
}

void tst_eigen::test_ColPivHouseholderQR()
{
    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    QCOMPARE( verifyVecApprox(X.colPivHouseholderQr().solve(b), sol), true );
}

void tst_eigen::test_LLT()
{
    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    // LLT is fast but solution is not very accurate
    QEXPECT_FAIL("", "LLT is inaccurate", Continue);
    QCOMPARE( verifyVecApprox(X.llt().solve(b), sol), true );
}

void tst_eigen::test_LDLT()
{
    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    // LDLT is fast but solution is not very accurate
    QEXPECT_FAIL("", "LDLT is inaccurate", Continue);
    QCOMPARE( verifyVecApprox(X.ldlt().solve(b), sol), true );
}

void tst_eigen::test_SVD()
{

    mXd X(5,5);
    vXd b(5);

    vXd sol(5);

    smallExample5x5(X, b, sol);

    QCOMPARE( verifyVecApprox(X.jacobiSvd(ComputeThinU | ComputeThinV).solve(b), sol), true );
}

void tst_eigen::test_MatrixFactorisationAccuracyLargeNonsingular()
{
    int m = 1000;
    int n = 1000;

    mXd A;
    vXd b;

    randomExample(A, b, m, n);

    double fullPivLuResidual;
#ifndef FAST_EIGEN_TEST
    double jacobiSvdResidual;
#endif
    double householderQrResidual, fullPivHouseholderQrResidual, colPivHouseholderQrResidual;

    QCOMPARE( verifyVecApproxLarge(A * (A.fullPivLu().solve(b)), b, fullPivLuResidual), true);
    QCOMPARE( verifyVecApproxLarge(A * (A.householderQr().solve(b)), b, householderQrResidual), true);
    QCOMPARE( verifyVecApproxLarge(A * (A.fullPivHouseholderQr().solve(b)), b, fullPivHouseholderQrResidual), true);
    QCOMPARE( verifyVecApproxLarge(A * (A.colPivHouseholderQr().solve(b)), b, colPivHouseholderQrResidual), true);
#ifndef FAST_EIGEN_TEST
    QCOMPARE( verifyVecApproxLarge(A * (A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b)), b, jacobiSvdResidual), true);
#endif
}

void tst_eigen::test_MatrixFactorisationAccuracyLargeSingular()
{

    int m = 1000;
    int n = 1000;

    mXd A;
    vXd b;

    randomExample(A,b,m,n);
    // make  the test problem singular by
    // setting one col of A to be zero.
    for (int i=0; i<m; i++)
        A(i,n-1) = 0.0;
    b(m-1) = 0;

    double fullPivLuResidual;
#ifndef FAST_EIGEN_TEST
    double jacobiSvdResidual;
#endif
    double householderQrResidual, fullPivHouseholderQrResidual, colPivHouseholderQrResidual;

    /*
     * The following tests are expected to fail because the matrix is singular and
     * the solvers can only find a least-squres solution, namely, resiudal = (Ax-b)/(1 + norm(b)) will not be zeros.
     * But the smaller the residual is, the more accurate the solver is.
     *
     * Note that, you may see the residual for householderQr is "nan" (not a number), which implies
     * that householderQr fails to find even a least-squares solution (actually the factorisation rotine fails)
     */
    QEXPECT_FAIL("", "fullPivLu: least-squares solution", Continue);
    QCOMPARE( verifyVecApproxLarge(A * (A.fullPivLu().solve(b)), b, fullPivLuResidual), true);

    QEXPECT_FAIL("", "householderQr: least-squares solution", Continue);
    QCOMPARE( verifyVecApproxLarge(A * (A.householderQr().solve(b)), b, householderQrResidual), true);


    QEXPECT_FAIL("", "fullPivHouseholderQr: least-squares solution", Continue);
    QCOMPARE( verifyVecApproxLarge(A * (A.fullPivHouseholderQr().solve(b)), b, fullPivHouseholderQrResidual), true);


    QEXPECT_FAIL("", "colPivHouseholderQr: least-squares solution", Continue);
    QCOMPARE( verifyVecApproxLarge(A * (A.colPivHouseholderQr().solve(b)), b, colPivHouseholderQrResidual), true);

#ifndef FAST_EIGEN_TEST
    QEXPECT_FAIL("", "jacobiSvd: least-squares solution", Continue);
    QCOMPARE( verifyVecApproxLarge(A * (A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b)), b, jacobiSvdResidual), true);
#endif

    cout<<"\nResidual for least-squares solutions"<<endl;
    cout<<"\t* fullPivLuResidual: "               <<fullPivLuResidual<<endl;
    cout<<"\t* householderQrResidual: "           <<householderQrResidual<<endl;
    cout<<"\t* fullPivHouseholderQrResidual: "    <<fullPivHouseholderQrResidual<<endl;
    cout<<"\t* colPivHouseholderQrResidual: "     <<colPivHouseholderQrResidual<<endl;
#ifndef FAST_EIGEN_TEST
    cout<<"\t* jacobiSvdResidual: "               <<jacobiSvdResidual<<endl;
#endif
}

/*
 * Private functions
 */
void tst_eigen::smallExample5x5(mXd &X, vXd &b, vXd &sol)
{
    X <<    14, 16, 10,  9, 13,
            15,  5, 15, 19, 12,
            14,  2, 17, 19, 20,
            21, 13,  8,  5,  6,
            4, 10, 15, 14,  7;

    b << 8, 10, 9, 11, 9;

    sol<<0.313431819729049, 0.055892164204507, 0.764449588127170, -0.177683981210429, -0.255973857988972;
}

void tst_eigen::randomExample(mXd &X, vXd &b, int m, int n)
{
    X = mXd::Random(m,n);
    b = vXd::Random(m);
}

bool tst_eigen::verifyApprox(double num1, double num2) const
{
    return fabs(num1 - num2) < m_eps;
}

bool tst_eigen::verifyVecApprox(vXd v1, vXd v2) const
{
    if (v1.size() != v2.size())
        return false;

    for (int i=0; i<v1.size(); i++)
    {
        if (!verifyApprox(v1(i),v2(i)))
        {
            return false;
        }
    }

    return true;
}

bool tst_eigen::verifyMatApprox(mXd M1, mXd M2) const
{
    if (M1.rows() != M2.rows() || M1.cols() != M2.cols())
        return false;

    for (int i=0; i<M1.rows(); i++)
    {
        for (int j=0; j<M1.cols(); j++)
            if (!verifyApprox(M1(i,j), M2(i,j)))
                return false;
    }

    return true;
}

bool tst_eigen::verifyVecApproxLarge(vXd v1, vXd v2, double &residual) const
{
    if (v1.rows() != v2.rows() || v1.cols() != v2.cols())
        return false;

    residual = (v1-v2).norm() / (1+std::min(v1.norm(), v2.norm()));
    return residual < 1e-06;
}

double tst_eigen::leastSquaresResidual(mXd A, vXd b, vXd sol) const
{
    // normalised
    return (A*sol - b).norm() / (1+b.norm());
}

QTEST_APPLESS_MAIN(tst_eigen)

#include "tst_eigen.moc"
