/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#include <iostream>
#include <eigen/Eigen>
#include <eigen/bench/BenchTimer.h>

using namespace Eigen;
using namespace std;

typedef MatrixXd mXd;
typedef VectorXd vXd;
typedef Matrix<double, Dynamic, Dynamic, RowMajor> rmXd;

inline void randomExample(mXd &X, vXd &b, int m, int n)
{
    X = mXd::Random(m,n);
    b = vXd::Random(m);
}

inline double leastSquaresResidual(mXd A, vXd b, vXd sol)
{
    // normalised
    return (A*sol - b).norm() / (1+b.norm());
}

void benckmark_EigenPerformacneLargeSingular()
{
    BenchTimer timer;

    int m = 1000;
    int n = 1000;

    vXd x(n);
    mXd A;
    vXd b;
    double* nx = new double[n];
    double* ox = new double[n];
    double* nA = new double[n*m];
    double* oA = new double[n*m];
    for(int i=0; i<m; i++) {
        for(int j=0; j<n; j++) {
            nA[i + j*n] = i+j;
            oA[i + j*n] = 0;
        }
    }

    for (int i = 0; i<n; i++)
    {
        x(i)  = i;
        nx[i] = i;
        ox[i] = 0;
    }

    randomExample(A,b,m,n);

    cout<<"\nEigen performance test: "<<endl;
    cout << "m = "<<m<<", n = "<<n << "; time unit: second."<<endl;


    for (int i=0; i<m; i++)
        A(i,n-1) = 0.0;
    b(m-1) = 0;

    vXd V;
    mXd X;
    cout<<"* Matrix * Vector      ";
    timer.start();
    V = A * x;
    timer.stop();
    cout<< "\t: "<< timer.value() << endl;

    cout<<"* Matrix * Vector loop ";
    timer.start();
    for (int i=0; i<m; i++)
        for (int j=0; j<n; j++)
            ox[i] = ox[i] + nx[j]*nA[i + j*m];
    timer.stop();

    cout<< "\t: "<< timer.value() << endl;
    delete[] ox;
    delete[] nx;

    cout<<"* Matrix * Matrix      ";
    timer.start();
    X = A * A;
    timer.stop();
    cout<< "\t: "<< timer.value() << endl;

    cout<<"* Matrix * Matrix loop ";
    timer.start();
    for(int i=0; i<m; i++) {
        for(int j=0; j<n; j++) {
            for(int ci=0; ci<n; ci++) {
                oA[i + j*n] = oA[i + j*n] + nA[ci + j*n]*nA[i + ci*n];
            }
        }
    }
    timer.stop();
    cout<< "\t: "<<timer.value()<<endl;
    delete[] oA;
    delete[] nA;

    cout << "* Eigenvalues test     ";
    VectorXcd eigenvalues;
    timer.start();
    eigenvalues = A.eigenvalues();
    timer.stop();
    cout<< "\t: "<<timer.value()<<endl; // slow, matlab uses about 0.6s


    cout << "* fullPivLU test        ";
    timer.start();
    x = A.fullPivLu().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<",\t (Normalised Residual = "<<leastSquaresResidual(A,b,x)<<")"<<endl;

    /*
     * The residual for HouseholderQR is "nan" - it fails
     */
    cout << "* householderQr test      ";
    timer.start();
    x = A.householderQr().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<",\t (Normalised Residual = "<<leastSquaresResidual(A,b,x)<<")"<<endl;

    cout << "* fullPivHouseholderQr test ";
    timer.start();
    x = A.fullPivHouseholderQr().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<",\t (Normalised Residual = "<<leastSquaresResidual(A,b,x)<<")"<<endl;

    cout << "* colPivHouseholderQr test ";
    timer.start();
    x = A.colPivHouseholderQr().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<",\t (Normalised Residual = "<<leastSquaresResidual(A,b,x)<<")"<<endl;

    cout<<"* jacobiSvd test              ";
    timer.start();
    x = A.jacobiSvd(ComputeThinU | ComputeThinV).solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<",\t (Normalised Residual = "<<leastSquaresResidual(A,b,x)<<")"<<endl;

    cout << "* LDLT test      ";
    timer.start();
    x = A.ldlt().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<endl;

    cout << "*LLT test          ";
    timer.start();
    x = A.llt().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<endl;

    cout << "* PartialPivLU LU test  ";
    timer.start();
    x = A.partialPivLu().solve(b);
    timer.stop();
    cout<< "\t: "<<timer.value()<<endl;
}

int main()
{
    cout << "Running Benchmarks..." << endl;
    benckmark_EigenPerformacneLargeSingular();
    return 0;
}
