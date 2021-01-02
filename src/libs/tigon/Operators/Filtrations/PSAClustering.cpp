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
#include <tigon/Operators/Filtrations/PSAClustering.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Utils/TigonUtils.h>

using namespace Eigen;
using namespace Tigon::Representation;

namespace Tigon {
namespace Operators {

PSAClustering::PSAClustering()
{
    initialise();
}

PSAClustering::PSAClustering(IPSet* ipset)
    : IFiltration(ipset)
{
    initialise();
}

PSAClustering::~PSAClustering()
{

}

void PSAClustering::initialise()
{
    addProperty("numberOfClusters"
                , TString("Number of cluster with similar IMappings")
                , getTType(int));

    addProperty("normalised"
                , TString("Cluster the set according to the objective "
                          "vectors or design vectors.\n"
                          "Evaluate DVec is false for objective vectors "
                          "(default) and true for design vectors.")
                , getTType(bool));

    addProperty("evaluateDVec"
                , TString("Normalise every dimension to values between 0-1. \n"
                          "Default is true.")
                , getTType(int));

    TP_defineNumberOfClusters(1);
    TP_defineNormalised(true);
    TP_defineEvaluateDVec(false);

    defineInputTags(TStringList({Tigon::TForClustering}));
}

TString PSAClustering::name()
{
    return TString("PSA Clustering");;
}

TString PSAClustering::description()
{
    return TString("Partition a set to a predefined number of subsets using the"
                   " 'Part and Select Algorithm' (PSA).\n"
                   "The algorithm partitions one set into two subsets at each "
                   "iteration, increasing the number of subsets by one.\n"
                   "The set with the largest difference in any dimension amongst"
                   " its members is the one to be partitioned.\n"
                   "The partition is made according to the values of the most "
                   "diverse dimension:\n"
                   "all IMappings with values larger than the mid value are "
                   "kept in one subset and the rest in another.");
}

int PSAClustering::TP_numberOfClusters() const
{
    return m_numberOfClusters;
}

bool PSAClustering::TP_evaluateDVec()     const
{
    return m_evaluateDVec;
}

bool PSAClustering::TP_normalised()       const
{
    return m_normalised;
}

void PSAClustering::TP_defineNumberOfClusters(int n)
{
    if(n<=1) {
        m_numberOfClusters = 1;
    } else {
        m_numberOfClusters = n;
    }
}

void PSAClustering::TP_defineEvaluateDVec(bool e)
{
    m_evaluateDVec = e;
}

void PSAClustering::TP_defineNormalised(bool n)
{
    m_normalised = n;
}

void PSAClustering::evaluateNode()
{
    // Init
    clearOutputSets();

    initArrays();

    // the first subset is a duplicate of the input set
    appendOutputSet(inputSet(0));

    calculateClusterDistances(0);
    for(int nextCluster = 1;
        (nextCluster < m_numberOfClusters) & !allDuplicates(); nextCluster++) {
        cutLargestCluster(nextCluster);
    }

    createRepresentativeSet();
}


/***********************
*  Private functions  *
************************/

void PSAClustering::initArrays()
{
    m_dimensions = ( m_evaluateDVec? decisionVecSize() : objectiveVecSize() );

    //set the data for normalisation:
    m_normMinValues.setZero(m_dimensions);
    m_normDistances.setOnes(m_dimensions);
    MatrixXd allData;
    getClusterData(inputSet(0), allData);
    m_normMinValues = allData.rowwise().minCoeff();
    VectorXd normMaxValues = allData.rowwise().maxCoeff();
    m_normDistances = normMaxValues - m_normMinValues;
    if(m_normDistances.maxCoeff() == 0.0) {
        m_allDuplicates = true;
    } else {
        m_allDuplicates = false;
    }

    m_clustersMaxDistances.setConstant(m_numberOfClusters, -1.0);
    m_largestDimensions.setConstant(m_numberOfClusters, -1.0);
    m_largestCluster = 0;
    m_distances.setConstant(m_dimensions, m_numberOfClusters, -1.0);
    m_centerValues.setConstant(m_dimensions, m_numberOfClusters, -1.0);
    m_maxValues.setConstant(m_dimensions, m_numberOfClusters, -1.0);
    m_minValues.setConstant(m_dimensions, m_numberOfClusters, -1.0);
}

void PSAClustering::getClusterData(ISet* cluster, MatrixXd& data) const
{
    int s = cluster->size();
    data = MatrixXd(m_dimensions, s);
    TVector<IMappingSPtr> clusterMappings = cluster->all();
    for (int i = 0; i < m_dimensions; i++) {
        for (int j = 0; j < s; j++) {
            if(m_evaluateDVec) {
                data(i, j) = clusterMappings[j]->decisionVar(i)->value<double>();
            } else {
                data(i, j) = clusterMappings[j]->objectiveVar(i)->value<double>();
            }
        }
    }
    if(m_normalised) {
        data = (data - m_normMinValues.replicate(1, s)).array()
                / (m_normDistances.replicate(1, s)).array();
    }
}

void PSAClustering::calculateClusterDistances(int clusterNumber)
{
    MatrixXd data;
    ISet* cluster = outputSet(clusterNumber);
    getClusterData(cluster, data);

    m_maxValues.col(clusterNumber) = data.rowwise().maxCoeff();
    m_minValues.col(clusterNumber) = data.rowwise().minCoeff();

    m_distances.col(clusterNumber) =  m_maxValues.col(clusterNumber) - m_minValues.col(clusterNumber);
    m_centerValues.col(clusterNumber) = m_minValues.col(clusterNumber) + m_distances.col(clusterNumber)/2.0;

    m_clustersMaxDistances(clusterNumber) = m_distances.col(clusterNumber).maxCoeff(&m_largestDimensions(clusterNumber));
}


void PSAClustering::cutLargestCluster(int nextCluster)
{
    ISet* newCluster = appendOutputSet();
    ISet* cluster = outputSet(m_largestCluster);
    MatrixXd clusterData;
    getClusterData(cluster, clusterData);
    int64 clusterSize = clusterData.cols();

    // Move members from the largest set to the new set:
    int dim = m_largestDimensions(m_largestCluster);

    // iterate backward to avoid problems when removing members from the set
    for(int idx=clusterSize-1; idx>=0; idx--) {
        if(clusterData(dim, idx) > m_centerValues(dim, m_largestCluster)) {
            IMappingSPtr member = cluster->at(idx);
            cluster->remove(idx);
            newCluster->append(member);
        }
    }
    calculateClusterDistances(m_largestCluster);
    calculateClusterDistances(nextCluster);

    m_clustersMaxDistances.maxCoeff(&m_largestCluster);
}

bool PSAClustering::allDuplicates() const
{
    double maxDist = m_clustersMaxDistances(m_largestCluster);
    return (maxDist < Tigon::Epsilon) || m_allDuplicates;
}

TVector<ISet *> PSAClustering::clusters() const
{
    return mid(outputSets(), 0, outputSets().size()-1);
}

ISet* PSAClustering::represenativeSet() const
{
    return outputSets().back();
}

// TODO Shaul change this method
void PSAClustering::createRepresentativeSet()
{
    int nSets = outputSets().size();
    ISet* repSet = appendOutputSet();
    for(int clusterNumber=0; clusterNumber<nSets; clusterNumber++) {
        int dim = m_largestDimensions(clusterNumber);
        double centerValue = m_centerValues(dim, clusterNumber);
        MatrixXd clusterData;
        ISet* cluster = outputSet(clusterNumber);
        getClusterData(cluster, clusterData);
        RowVectorXd dimValues = clusterData.row(dim);
        RowVectorXd dist = (dimValues.array() - centerValue).abs();
        int idx;
        dist.minCoeff(&idx);
        repSet->append(cluster->at(idx));
    }
}

} // namespace Operators
} // namespace Tigon
