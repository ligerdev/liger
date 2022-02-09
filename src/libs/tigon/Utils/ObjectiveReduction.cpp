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
#include <tigon/Utils/ObjectiveReduction.h>

namespace Tigon {

ObjectiveReduction::ObjectiveReduction()
{
    // default values
    m_M = 3;
    TMatrixReal objectives(m_M,6);
    objectives << 0.683833, 0.0566770, 0.698944, 0.0081363,	0.269922, 0.5778188,
                  0.684069, 0.0566735, 0.699460, 0.0081363,	0.269910, 0.5778153,
                  0.254848, 0.9968276, 0.152550, 0.9999372,	0.924359, 0.5764336;

    framework(objectives);
}

ObjectiveReduction::ObjectiveReduction(ISet* set)
{
    m_M = set->at(0)->objectiveVec().size(); // number of objectives
    int N = set->size();                     // number of samples

    // extract the objective vectors from the population of solutions
    TMatrixReal objectives;
    objectives.resize(m_M,N);

    for(int i=0; i<N; i++) {
        TVector<double> objs = set->at(i)->doubleObjectiveVec();
        for(int j=0; j<m_M; j++) {
            objectives(j,i) = objs.at(j);
        }
    }

    framework(objectives);
}

ObjectiveReduction::ObjectiveReduction(const TVector<IMappingSPtr> &sols)
{
    m_M = sols[0]->objectiveVec().size(); // number of objectives
    int N = sols.size();                  // number of samples

    // extract the objective vectors from the population of solutions
    TMatrixReal objectives;
    objectives.resize(m_M,N);

    for(int i=0; i<N; i++) {
        TVector<double> objs = sols[i]->doubleObjectiveVec();
        for(int j=0; j<m_M; j++) {
            objectives(j,i) = objs.at(j);
        }
    }

    framework(objectives);
}

ObjectiveReduction::~ObjectiveReduction()
{
    delete m_corObject;
}

TVector<int> ObjectiveReduction::essentialSet() const
{
    return m_essentialSet;
}

TVector<int> ObjectiveReduction::redundantSet() const
{
    return m_redundantSet;
}

TVector<double> ObjectiveReduction::error() const
{
    return m_error;
}

TMap<int, TVector<int>> ObjectiveReduction::correlationsBetweenObjectives() const
{
    return m_corrSubsets;
}

void ObjectiveReduction::framework(const TMatrixReal &objectives)
{
    // 1. compute the correlation matrix
    m_corObject = new CorrelationMatrix(objectives);
    TMatrixReal corr = m_corObject->correlationMatrix();

    // 2. eigen-decompose the correlation matrix
    // determines m_lambda (eigenvalues) and m_V (eigenvectors)
    eigenDecomposition(corr);

    // 3. determine the number of significant principal components
    int Nv = significantPCs(0.997);

    // 4. First reduction based on eigenvalues and eigenvectors
    TVector<int> subSet = objectiveReductionByEigenAnalysis(Nv);

    // 5. Determine the dynamic correlation threshold
    int M2e = significantPCs(0.954);
    double Tcor = 1.0 - m_lambda[0]*(1.0 - M2e/m_M);

    // 6. Identification of subsets of correlated objectives
    // based on the analysis of the correlation matrix
    m_corrSubsets = correlatedSubsets(corr, subSet, Tcor);

    // 7. Second reduction based on the correlated subsets
    TVector<bool> selected = finalReductionRCM(m_corrSubsets, subSet, Nv);
    m_essentialSet.clear();
    for(int i=0; i<m_M; i++) {
        if(selected[i]==true) {
            m_essentialSet << i;
        }
        else {
            m_redundantSet << i;
        }
    }

    // 8: compute the error measure
    m_error = error_measure(m_corrSubsets);
}

TVector<double> ObjectiveReduction::error_measure(
        const TMap<int, TVector<int>>& corrSubsets)
{

    TVector<double> error(m_M);

    TVector<double> c(m_M,0.0);
    for(int i=0; i<m_M; i++) {
        for(int k=0; k<m_M; k++) {
            c[i] += m_lambda[k] * std::pow(m_V[k][i],2.0);
        }
    }

    for(auto i: m_essentialSet) {
        error[i] = c[i];
    }

    TMatrixReal corr = m_corObject->correlationMatrix();

    for(auto i: m_redundantSet) {

        TVector<int> v = corrSubsets.at(i);

        double max = 0.0;
        for(auto j: m_essentialSet) {
            if( (corr(i,j) > max) &&
                    (std::find(v.begin(), v.end(), j)!=v.end()) ) {
                max = corr(i,j);
            }
        }
        error[i] = c[i] * (1.0 - max);
    }

    double sum = 0.0;
    for(auto elem: error) {
        sum += elem;
    }

    // normalise
    for(int i=0; i<m_M; i++) {
        error[i] = error[i]/sum;
    }

    return error;
}

TVector<bool> ObjectiveReduction::finalReductionRCM(
        const TMap<int, TVector<int>>& corrSubsets,
        const TVector<int>& subSet,
        int Nv)
{
    TVector<bool> sel(m_M,false);
    for(auto fi: subSet) {

        TVector<int> fi_correlated = corrSubsets.at(fi);
        if(fi_correlated.size()<1) {
            throw TException("Objective Reduction",
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
                    sum += m_lambda[j]*std::fabs(m_V[j][sfi]);
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

TMap<int, TVector<int>> ObjectiveReduction::correlatedSubsets(
        const TMatrixReal &corr, const TVector<int>& subSet, double Tcor)
{
    TMap<int, TVector<int>> corrSubsets;

    for(auto fi : subSet) {

        TVector<int> fi_correlated;

        for(auto fj : subSet) {

            // note that an objective will be identified as being correlated
            // with itself, therefore min(|fi_correlated|)==1

            bool sameCorSign = true;
            for(int k=0; k<m_M; k++) {
                if(sign(corr(fi,k))!=sign(corr(fj,k))) {
                    sameCorSign = false;
                    break;
                }
            }

            if( sameCorSign && corr(fi,fj)>=Tcor ) {
                fi_correlated.push_back(fj);
            }
        }
        corrSubsets[fi] = fi_correlated;
    }

    return corrSubsets;
}

TVector<int> ObjectiveReduction::objectiveReductionByEigenAnalysis(int Nv)
{
    TVector<bool> sel(m_M,false); // selected objectives

    // Move along the significant principal components
    for(int ipc=0; ipc<Nv; ipc++) {

        // current principal component
        TVector<double> pc = m_V[ipc];

        // check if all elements have the same sign
        bool doAllElementsHaveSameSign = true;
        int val = sgn(pc[0]);
        for(int i=1; i<pc.size(); i++) {
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
            for(int i=0; i<m_M; i++) {
                if(i!=idx) {
                    int currentVal = sgn(pc[i]);
                    if(val!=currentVal) {
                        sel[i]=true;
                    }
                }
            }
        } // if condition: not all elements have same sign
    }  // moving along the significant principal components

    TVector<int> selectedObjectives;
    for(int i=0; i<m_M; i++) {
        if(sel[i]==true) {
            selectedObjectives << i;
        }
    }

    return selectedObjectives;
}

int ObjectiveReduction::significantPCs(double theta)
{
    // determine the number of significant principal components (Nv)
    int Nv=m_M;
    double sum=0.0;
    for(int i=0; i<m_lambda.size(); i++) {
        sum += m_lambda[i];
        if(sum>theta) {
            Nv=i+1;
            break;
        }
    }
    return Nv;
}

void ObjectiveReduction::eigenDecomposition(const TMatrixReal &mx)
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
    m_lambda.resize(lambda.size());
    m_V.reserve(lambda.size());
    for(int i=0; i<lambda.size(); i++) {
        m_lambda[i] = lambda(idx[i])/lsum;

        // column is an eigenvector
        TCVectorReal pca = V.col(idx[i]);
        TVector<double> v;
        v.resize(pca.size());
        Eigen::VectorXd::Map(&v[0], pca.size()) = pca;

        m_V.push_back(v);
    }
    return;
}

} // namespace Tigon
