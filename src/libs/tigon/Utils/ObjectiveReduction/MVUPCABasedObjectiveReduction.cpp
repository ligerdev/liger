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
#include <tigon/Utils/ObjectiveReduction/MVUPCABasedObjectiveReduction.h>
#include <tigon/Utils/NormalisationUtils.h>
extern "C"
{
#include <csdp/declarations.h>
}

namespace Tigon {

MVUPCABasedObjectiveReduction::MVUPCABasedObjectiveReduction()
    : PCABasedObjectiveReduction()
    , m_standardize(true)
{

}

MVUPCABasedObjectiveReduction::~MVUPCABasedObjectiveReduction()
{

}

void MVUPCABasedObjectiveReduction::defineStandardize(bool s)
{
    m_standardize = s;
}

bool MVUPCABasedObjectiveReduction::getStandardize() const
{
    return m_standardize;
}

void MVUPCABasedObjectiveReduction::runObjectiveReductionAlgorithm()
{
    int M = numberObjectives();
    int k = M-1;

    // Eigen-decompose the Kernel matrix
    // determines m_lambda (eigenvalues) and m_V (eigenvectors)
    eigenDecomposition(learnKernel(k, m_standardize));

    algorithmCommonCode();
}

TMatrixReal MVUPCABasedObjectiveReduction::learnKernel(int k, bool standardize) const
{
    TMatrixReal x = correlationMatrix()->inputData();

    // 1) mean centre the data
    TMatrixReal centered = x.colwise() - x.rowwise().mean();

    // 2) standardise the data if requested
    TMatrixReal xFormat;
    if(standardize) {

        // determine the inverse of the standard deviation
        int N = x.cols();
        double scaler = 1.0 / double(N-1);
        TCVectorReal variance = scaler*centered.rowwise().squaredNorm();
        TCVectorReal istd     = variance.cwiseSqrt().cwiseInverse();

        // normalise the data by using the standard deviation
        xFormat = istd.asDiagonal() * centered;

    } else {

        xFormat = centered;
    }

    // 3) Compute the Gram matrix (G)
    TMatrixReal G = xFormat * xFormat.transpose();

    // 4) determine the k nearest neighbours
    TVector<TVector<int>> a = kNearestNeighbours(xFormat, k);

    // 5) Compute the right hand side constraints (b)
    TVector<double> b = constraintsRightHandSide(a, G, k);

    // 6) Compute the left hand side constraints (At)
    TVector<TVector<int>> At = constraintsLeftHandSide(a, k);

    // 7) Compute coefficients of the function to maximise (Tr(K))
    int M = numberObjectives();
    TVector<double> c(M*M,1.0/(double)M);
    for(int i=0; i<M; i++) {
        c[(M+1)*i] = -2.0/(double)M;
    }

    // 8) Compute the Kernel Matrix using 'At' 'b' and 'c'
    TMatrixReal K = callCSDP(c, At, b, k);

    return K;
}

TVector<TVector<int>> MVUPCABasedObjectiveReduction::
kNearestNeighbours(const TVector<TVector<double>>& matrix, int k) const
{
    // input is MxN
    int M=matrix.size();
    if(k>M-1) {
        throw TException("MVUPCABasedObjectiveReduction",
                         "The parameter k in the k-nearest neighbours algorithm "
                         "cannot be higher than M-1, where M is the number of "
                         "objectives.");
    }

    TVector<TVector<int>> res;
    res.reserve(M);

    if(k==(M-1)) { // fully connected graph
        for(int i=0; i<M; i++) {
            TVector<int> nV;
            nV.reserve(M-1);
            for(int j=0; j<M; j++) {
                if(i!=j) {
                    nV.push_back(j);
                }
            }
            res.push_back(nV);
        }
        return res;
    }

    for(int i=0; i<M; i++) {

        TVector<double> v1 = matrix[i];

        TVector<double> dist;
        dist.reserve(M-1);
        TVector<int> indices;
        indices.reserve(M-1);
        for(int j=0; j<M; j++) {
            if(i!=j) {

                indices.push_back(j);
                TVector<double> v2 = matrix[j];
                double d = lpdist(v1, v2, 1);
                dist.push_back(d);
            }
        }

        TVector<int> sorted = indSort(dist);
        TVector<int> kNearest;
        kNearest.reserve(k);
        for(int j=0; j<k; j++) {
            kNearest.push_back(indices[sorted[j]]);
        }
        res.push_back(kNearest);
    }
    return res;
}

TVector<TVector<int>> MVUPCABasedObjectiveReduction::
kNearestNeighbours(const TMatrixReal& matrix, int k) const
{
    // input is MxN
    int M=matrix.rows();
    if(k>M-1) {
        throw TException("MVUPCABasedObjectiveReduction",
                         "The parameter k in the k-nearest neighbours algorithm "
                         "cannot be higher than M-1, where M is the number of "
                         "objectives.");
    }

    TVector<TVector<int>> res;
    res.reserve(M);

    if(k==(M-1)) { // fully connected graph
        for(int i=0; i<M; i++) {
            TVector<int> nV;
            nV.reserve(M-1);
            for(int j=0; j<M; j++) {
                if(i!=j) {
                    nV.push_back(j);
                }
            }
            res.push_back(nV);
        }
        return res;
    }

    for(int i=0; i<M; i++) {

        TCVectorReal v1 = matrix.row(i);

        TVector<double> dist;
        dist.reserve(M-1);
        TVector<int> indices;
        indices.reserve(M-1);
        for(int j=0; j<M; j++) {
            if(i!=j) {

                indices.push_back(j);
                TCVectorReal v2 = matrix.row(j);

                double d = (v1-v2).sum();
                dist.push_back(d);
            }
        }

        TVector<int> sorted = indSort(dist);
        TVector<int> kNearest;
        kNearest.reserve(k);
        for(int j=0; j<k; j++) {
            kNearest.push_back(indices[sorted[j]]);
        }
        res.push_back(kNearest);
    }
    return res;
}

TVector<double> MVUPCABasedObjectiveReduction::
constraintsRightHandSide(const TVector<TVector<int>>& a, const TMatrixReal& G,
                         int k) const
{
    int M = numberObjectives();

    TVector<double> b(M*k+1);

    // Set first constraint (Sum(Kij)==0)
    b[0]= 0.0;

    int c = 1;
    for(int i=0; i<M; i++) {
        for(int j : a[i]) {

            double value = G(i,i) + G(j,j) - G(i,j) - G(j,i);

            b[c] = value;
            c++;
        }
    }

    return b;
}

TVector<TVector<int>> MVUPCABasedObjectiveReduction::
constraintsLeftHandSide(const TVector<TVector<int>>& a, int k) const
{
    int M = numberObjectives();
    int nVariables = M*M;

    TVector<TVector<int>> At(M*k+1,TVector<int>(nVariables,0));

    // Set first constraint (Sum(Kij)==0)
    for(int i=0; i<nVariables; i++) {
        At[0][i] = 1;
    }

    // Set remaining constraints according with Nearest-Neighbours
    int p = 1;
    for(int i=0; i<M; i++) {
        for(int j : a[i]) {
            At[p][i * M + i] = 1;
            At[p][j * M + j] = 1;
            At[p][i * M + j] = -1;
            At[p][j * M + i] = -1;
            p++;
        }
    }
    return At;
}

TMatrixReal MVUPCABasedObjectiveReduction::callCSDP(
        const TVector<double>& c, const TVector<TVector<int>>& At,
                      const TVector<double>& b_in, int k) const {

    int M = numberObjectives();

    /***********************************/
    /* Objective Function Coefficients */
    /***********************************/

    struct blockmatrix C;
    C.nblocks = 1;
    C.blocks = (struct blockrec *) malloc(2 * sizeof(struct blockrec));
    if (C.blocks == NULL) {
        printf("Couldn't allocate storage for C!\n");
        exit(1);
    };

    /* fill the block */
    C.blocks[1].blockcategory = MATRIX;
    C.blocks[1].blocksize = M;
    C.blocks[1].data.mat = (double *) malloc(M * M * sizeof(double));
    if (C.blocks[1].data.mat == NULL) {
        printf("Couldn't allocate storage for C!\n");
        exit(1);
    }
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            C.blocks[1].data.mat[ijtok(i+1,j+1,M)] = c[(M*i)+j];
        }
    }

    /*******************************/
    /* Constraints right hand side */
    /*******************************/

    int n_constraints = M*k+1;
    double* b;
    b = (double*) malloc((n_constraints + 1) * sizeof(double));
    if (b == NULL) {
        printf("Could not allocate storage for b!\n");
        exit(-1);
    }
    for (int constr = 1; constr <= n_constraints; constr++) {
        b[constr] = b_in[constr - 1];
    }

    /******************************/
    /* Constraints left hand side */
    /******************************/

    struct constraintmatrix *constraints;
    constraints = (struct constraintmatrix*) malloc(
                      (n_constraints + 1) * sizeof(struct constraintmatrix));
    if (constraints == NULL) {
        printf("Failed to allocate storage for constraints");
    }

    struct sparseblock* blockptr;
    for (int constr = 1; constr <= n_constraints; constr++) {

        constraints[constr].blocks = NULL;

        blockptr = (struct sparseblock *) malloc(sizeof(struct sparseblock));
        if (blockptr == NULL) {
            printf("Allocation of constraint block failed!\n");
            exit(1);
        }

        /* find out the number of non-zero entries */
        int n_entries = 0;
        for (int i = 0; i < M; i++) {
            for (int j = i; j < M; j++) {
                if (At[constr - 1][(M * i) + j] != 0)
                    n_entries++;
            }
        }

        blockptr->blocknum = 1;
        blockptr->blocksize = M;
        blockptr->constraintnum = constr;
        blockptr->next = NULL;
        blockptr->nextbyblock = NULL;
        blockptr->entries = (double *) malloc((n_entries + 1) * sizeof(double));
        if (blockptr->entries == NULL) {
            printf("Allocation of constraint block failed!\n");
            exit(1);
        }
        blockptr->iindices = (unsigned short *) malloc((n_entries + 1) * sizeof(unsigned short));
        if (blockptr->iindices == NULL) {
            printf("Allocation of constraint block failed!\n");
            exit(1);
        }
        blockptr->jindices = (unsigned short *) malloc((n_entries + 1) * sizeof(unsigned short));
        if (blockptr->jindices == NULL) {
            printf("Allocation of constraint block failed!\n");
            exit(1);
        }

        blockptr->numentries = n_entries;

        int counter = 1;
        for (int i = 0; i < M; i++) {
            for (int j = i; j < M; j++) {
                if (At[constr - 1][M * i + j] != 0) {
                    blockptr->iindices[counter] = i + 1;
                    blockptr->jindices[counter] = j + 1;
                    blockptr->entries[counter] = At[constr - 1][(M * i) + j];
                    counter++;
                }
            }
        }

        blockptr->next = constraints[constr].blocks;
        constraints[constr].blocks = blockptr;
    }

    /*
    write_prob("prob.dat-s", nobj, n_constraints, C, b, constraints);
    ret = read_prob("prob.dat-s", &nobj, &counter, &C, &b, &constraints, 1);
    if (ret != 0) {
        printf("Giving up.\n");
        exit(10);
}
    */

    double* y;
    double pobj, dobj;
    struct blockmatrix X, Z;
    initsoln(M, n_constraints, C, b, constraints, &X, &y, &Z);
    int ret = easy_sdp(M, n_constraints, C, b, constraints, 0.0, &X, &y, &Z,
                   &pobj, &dobj);

//    if (ret == 0)
//        printf("The objective value is %.7e\n", (dobj + pobj) / 2);
//    else
//        printf("SDP failed.\n");
//    write_sol("prob.sol", M, n_constraints, X, y, Z);

    TMatrixReal K(M,M);
    for(int i=0; i<M; i++) {
        for(int j=0;j<M; j++) {
            K(i,j) = X.blocks[1].data.mat[ijtok(i+1,j+1,M)];
        }
    }

    free_prob(M, n_constraints, C, b, constraints, X, y, Z);

    return K;
}



} // namespace Tigon
