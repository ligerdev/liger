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
#include <tigon/Utils/ObjectiveReduction/PCABasedObjectiveReduction.h>

namespace Tigon {

PCABasedObjectiveReduction::PCABasedObjectiveReduction()
{
    // initialize the correlation matrix object
    m_corObject = new CorrelationMatrix();

    // default values
    int M = 3; // number of objectives
    int N = 6; // number of points

    TMatrixReal data(M,N);
    data << 0.683833, 0.0566770, 0.698944, 0.0081363, 0.269922, 0.5778188,
            0.684069, 0.0566735, 0.699460, 0.0081363, 0.269910, 0.5778153,
            0.254848, 0.9968276, 0.152550, 0.9999372, 0.924359, 0.5764336;

    updateData(data);
}

PCABasedObjectiveReduction::~PCABasedObjectiveReduction()
{
    if(m_corObject != nullptr) {
        delete m_corObject;
    }
}

void PCABasedObjectiveReduction::updateData(ISet* data)
{
    // There needs to be at least two points
    if(data->size() < 2) {
        throw TException("PCABasedObjectiveReduction",
                         "The dataset needs to have at least two points.");
    }

    // There needs to be at least one objective
    if(data->at(0)->objectiveVec().size() < 2) {
        throw TException("PCABasedObjectiveReduction",
                         "Objective reduction requires at least two objectives.");
    }

    int M = data->at(0)->objectiveVec().size(); // number of objectives
    int N = data->size();                       // number of points

    // extract the objective vectors from the population of solutions
    TMatrixReal objectives;
    objectives.resize(M,N);

    for(int i=0; i<N; i++) {
        TVector<double> objs = data->at(i)->doubleObjectiveVec();
        for(int j=0; j<M; j++) {
            objectives(j,i) = objs.at(j);
        }
    }

    updateData(objectives);
}

void PCABasedObjectiveReduction::updateData(const TVector<TVector<double>>& data)
{
    // There needs to be at least two points
    if(data.size() < 2) {
        throw TException("PCABasedObjectiveReduction",
                         "The dataset needs to have at least two points.");
    }

    // There needs to be at least one objective
    if(data[0].size() < 2) {
        throw TException("PCABasedObjectiveReduction",
                         "Objective reduction requires at least two objectives.");
    }

    int M = data[0].size(); // number of objectives
    int N = data.size();    // number of points

    // extract the objective vectors from the population of solutions
    TMatrixReal objectives;
    objectives.resize(M,N);

    for(int i=0; i<N; i++) {
        for(int j=0; j<M; j++) {
            objectives(j,i) = data[i][j];
        }
    }

    updateData(objectives);
}

void PCABasedObjectiveReduction::updateData(const TMatrixReal& data)
{
    // There needs to be at least two points
    if(data.cols() < 2) {
        throw TException("PCABasedObjectiveReduction",
                         "The dataset needs to have at least two points.");
    }

    // There needs to be at least two objectives
    if(data.rows() < 2) {
        throw TException("PCABasedObjectiveReduction",
                         "Objective reduction requires at least two objectives.");
    }

    m_M = data.rows(); // set number of objectives

    // update the correlation matrix
    m_corObject->updateData(data);
    // run the objective reduction algorithm
    runObjectiveReductionAlgorithm();
}

TVector<int> PCABasedObjectiveReduction::essentialSet() const
{
    return m_essentialSet;
}

void PCABasedObjectiveReduction::defineEssentialSet(const TVector<int>& set)
{
    m_essentialSet = set;
}

TVector<int> PCABasedObjectiveReduction::redundantSet() const
{
    return m_redundantSet;
}

void PCABasedObjectiveReduction::defineRedundantSet(const TVector<int>& set)
{
    m_redundantSet = set;
}

TVector<double> PCABasedObjectiveReduction::error() const
{
    return m_error;
}

void PCABasedObjectiveReduction::defineError(const TVector<double>& err)
{
    m_error = err;
}

TVector<double> PCABasedObjectiveReduction::variance() const
{
    return m_variance;
}

void PCABasedObjectiveReduction::defineVariance(const TVector<double>& var)
{
    m_variance = var;
}

TMap<int, TVector<int>>
PCABasedObjectiveReduction::correlatedObjectiveSubSets() const
{
    return m_corrSubsets;
}
void PCABasedObjectiveReduction::defineCorrelatedObjectiveSubSets(
        const TMap<int, TVector<int>>& corr)
{
    m_corrSubsets = corr;
}

CorrelationMatrix* PCABasedObjectiveReduction::correlationMatrix() const
{
    return m_corObject;
}

int PCABasedObjectiveReduction::numberObjectives() const
{
    return m_M;
}

TVector<double> PCABasedObjectiveReduction::eigenValues() const
{
    return m_lambda;
}

void PCABasedObjectiveReduction::defineEigenValues(const TVector<double>& lambda)
{
    m_lambda = lambda;
}


TVector<TVector<double>> PCABasedObjectiveReduction::eigenVectors() const
{
    return m_V;
}

void PCABasedObjectiveReduction::defineEigenVectors(const TVector<TVector<double>>& V)
{
    m_V = V;
}

void PCABasedObjectiveReduction::algorithmCommonCode()
{
    // 1. determine the number of significant principal components
    int Nv = significantPCs(0.997);

    // 2. First reduction based on eigenvalues and eigenvectors
    TVector<int> subSet = objectiveReductionByEigenAnalysis(Nv);

    // 3. Determine the dynamic correlation threshold
    int M2e = significantPCs(0.954);
    double Tcor = 1.0 - eigenValues()[0]*(1.0 - M2e/numberObjectives());

    // 4. get the correlation matrix
    TMatrixReal corr = correlationMatrix()->correlationMatrix();

    // 5. Identification of subsets of correlated objectives
    // based on the analysis of the correlation matrix
    defineCorrelatedObjectiveSubSets(correlatedSubsets(corr, subSet, Tcor));

    // 6. Second reduction based on the correlated subsets
    TVector<bool> selected = finalReductionRCM(correlatedObjectiveSubSets(),
                                               subSet, Nv);
    TVector<int> essentialSet;
    TVector<int> redundantSet;
    for(int i=0; i<numberObjectives(); i++) {
        if(selected[i]) {
            essentialSet << i;
        }
        else {
            redundantSet << i;
        }
    }
    defineEssentialSet(essentialSet);
    defineRedundantSet(redundantSet);

    // 7: compute the error measure
    // compute the correlated objectives w.r.t the entire objective set
    TVector<int> set(numberObjectives());
    std::iota(set.begin(), set.end(), 0);
    defineCorrelatedObjectiveSubSets(correlatedSubsets(corr, set, Tcor));
    // compute error
    defineError(error_measure(correlatedObjectiveSubSets()));
}

void PCABasedObjectiveReduction::runObjectiveReductionAlgorithm()
{
    // Eigen-decompose the Correlation matrix
    // determines m_lambda (eigenvalues) and m_V (eigenvectors)
    eigenDecomposition(correlationMatrix()->correlationMatrix());

    algorithmCommonCode();
}

TVector<double> PCABasedObjectiveReduction::error_measure(
        const TMap<int, TVector<int>>& corrSubsets)
{

    TVector<double> error(numberObjectives());

    TVector<double> c(numberObjectives(),0.0);
    for(int i=0; i<numberObjectives(); i++) {
        for(int k=0; k<numberObjectives(); k++) {
            c[i] += eigenValues()[k] * std::pow(eigenVectors()[k][i],2.0);
        }
    }
    defineVariance(c);

    for(auto i: essentialSet()) {
        error[i] = c[i];
    }

    TMatrixReal corr = correlationMatrix()->correlationMatrix();

    for(auto i: redundantSet()) {

        TVector<int> v = corrSubsets.at(i);

        double max = 0.0;
        if(!v.empty()) {
            for(auto j: essentialSet()) {
                auto result = std::find(v.begin(), v.end(), j);
                if(result != v.end()) {
                    if(fabs(corr(i,j)) > max) {
                        max = fabs(corr(i,j));
                    }
                }
            }
        }
        // fabs here is to prevent very small numbers in case max ~= 1.0
        // becoming negative
        error[i] = c[i] * fabs(1.0 - max);
    }

    double sum = 0.0;
    for(double elem: error) {
        sum += elem;
    }

    // normalise
    for(int i=0; i<numberObjectives(); i++) {
        error[i] = error[i]/sum;
    }

    return error;
}

TVector<bool> PCABasedObjectiveReduction::finalReductionRCM(
        const TMap<int, TVector<int>>& corrSubsets,
        const TVector<int>& subSet,
        int Nv)
{
    TVector<bool> sel(numberObjectives(),false);
    for(auto fi: subSet) {

        TVector<int> fi_correlated = corrSubsets.at(fi);
        if(fi_correlated.size()<1) {
            throw TException("PCABasedObjectiveReduction",
                             "The correlated set is smaller than 1");
        }

        if(fi_correlated.size()==1) {
            // uncorrolated objective gets picked
            sel[fi]=true;
        }
        else {

            TVector<double> sc;
            for(auto sfi : fi_correlated) {

                double sum = 0.0;
                for(int j=0; j<Nv; j++) {
                    sum += eigenValues()[j]*std::fabs(eigenVectors()[j][sfi]);
                }
                sc.push_back(sum);
            }

            // index of the maximum element
            int idx = std::distance(sc.begin(),
                                    std::max_element(sc.begin(), sc.end()));

            // the objective with the maximum score gets picked
            sel[fi_correlated[idx]] = true;
        }
    }

    return sel;
}

TMap<int, TVector<int>> PCABasedObjectiveReduction::correlatedSubsets(
        const TMatrixReal &corr, const TVector<int>& subSet, double Tcor)
{
    TMap<int, TVector<int>> corrSubsets;

    for(auto fi : subSet) {

        TVector<int> fi_correlated;

        for(auto fj : subSet) {

            // note that an objective will be identified as being correlated
            // with itself, therefore min(|fi_correlated|)==1

            bool sameCorSign = true;
            for(int k=0; k<numberObjectives(); k++) {
                if(sign(corr(fi,k))!=sign(corr(fj,k))) {
                    sameCorSign = false;
                    break;
                }
            }

            if( sameCorSign && fabs(corr(fi,fj))>=Tcor ) {
                fi_correlated.push_back(fj);
            }
        }
        corrSubsets[fi] = fi_correlated;
    }

    return corrSubsets;
}

TVector<int> PCABasedObjectiveReduction::objectiveReductionByEigenAnalysis(int Nv)
{
    TVector<bool> sel(numberObjectives(),false); // selected objectives

    // Move along the significant principal components
    for(int ipc=0; ipc<Nv; ipc++) {

        // current principal component
        TVector<double> pc = eigenVectors()[ipc];

        // check if all elements have the same sign
        bool doAllElementsHaveSameSign = true;
        int val = sgn(pc[0]);
        for(size_t i=1; i<pc.size(); i++) {
            int currentVal = sgn(pc[i]);
            if(currentVal!=val) {
                doAllElementsHaveSameSign = false;
                break;
            }
        }

        if(doAllElementsHaveSameSign) {

            // sort the elements of the principal component by magnitude
            TVector<int> idx(pc.size());
            std::iota(idx.begin(), idx.end(), 0);

            std::sort(idx.begin(), idx.end(),
                 [&pc](int i1, int i2) {
                     return std::fabs(pc[i1]) > std::fabs(pc[i2]);
                 });

            // the objectives with the top two contributions by magnitude,
            // are picked
            sel[idx[0]]=true;
            sel[idx[1]]=true;
        }
        else {

            // index of the element with the highest magnitude
            int idx = std::distance(pc.begin(), std::max_element(
                                    pc.begin(), pc.end(),
                                        [](double v1, double v2){
                                         return std::fabs(v1) < std::fabs(v2);
                                    }));
            // the objective with the highest contribution to the principal
            // component by magnitude is picked
            sel[idx]=true;

            // get the sign of the element with the highest contribution to the
            // principal component by magnitude
            int val = sgn(pc[idx]);

            // all objectives with the opposite sign to the objective with the
            // highest contribution to the principal component by magnitude are
            // also picked
            for(int i=0; i<numberObjectives(); i++) {
                if(i!=idx) {
                    int currentVal = sgn(pc[i]);
                    if(val!=currentVal) {
                        sel[i]=true;
                    }
                }
            }
        } // if condition: not all elements have the same sign
    }  // moving along the significant principal components

    TVector<int> selectedObjectives;
    for(int i=0; i<numberObjectives(); i++) {
        if(sel[i]==true) {
            selectedObjectives << i;
        }
    }

    return selectedObjectives;
}

int PCABasedObjectiveReduction::significantPCs(double theta)
{
    // determine the number of significant principal components (Nv)
    int Nv=numberObjectives();
    double sum=0.0;
    for(size_t i=0; i<eigenValues().size(); i++) {
        sum += eigenValues()[i];
        if(sum>theta) {
            Nv=i+1;
            break;
        }
    }
    return Nv;
}

void PCABasedObjectiveReduction::eigenDecomposition(const TMatrixReal &mx)
{
    // eigen-decompose the correlation matrix
    Eigen::EigenSolver<TMatrixReal> es(mx);

    TCVectorReal lambda = es.eigenvalues().real();
    TMatrixReal V = es.eigenvectors().real();

    TVector<int> idx(lambda.size());
    std::iota(idx.begin(), idx.end(), 0);

    std::sort(idx.begin(), idx.end(),
         [&lambda](int i1, int i2) {
        return std::fabs(lambda(i1)) > std::fabs(lambda(i2));});

    // normalise the eigenvalues
    double lsum = lambda.sum();
    TVector<double> eigenL(lambda.size());
    TVector<TVector<double>> eigenVectors;
    eigenVectors.reserve(lambda.size());
    for(int i=0; i<lambda.size(); i++) {
        eigenL[i] = lambda(idx[i])/lsum;

        // column is an eigenvector
        TCVectorReal pca = V.col(idx[i]);
        TVector<double> v;
        v.resize(pca.size());
        Eigen::VectorXd::Map(&v[0], pca.size()) = pca;

        eigenVectors.push_back(v);
    }

    defineEigenValues(eigenL);
    defineEigenVectors(eigenVectors);

    return;
}

} // namespace Tigon
