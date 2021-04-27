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
#ifndef PSACLUSTERING_H
#define PSACLUSTERING_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {
using namespace Representation;

class LIGER_TIGON_EXPORT PSAClustering : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(numberOfClusters, TP_numberOfClusters)
    READ(normalised, TP_normalised)
    READ(evaluateDVec, TP_evaluateDVec)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(numberOfClusters, int, TP_defineNumberOfClusters)
    WRITE(normalised, bool, TP_defineNormalised)
    WRITE(evaluateDVec, bool, TP_defineEvaluateDVec)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::PSAClustering)

public:
    PSAClustering();
    PSAClustering(Tigon::Representation::IPSet* ipset);
    virtual ~PSAClustering();

    virtual void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    //Properties
    int  TP_numberOfClusters()      const;
    bool TP_evaluateDVec()          const;
    bool TP_normalised()            const;
    void TP_defineNumberOfClusters(int n);
    void TP_defineEvaluateDVec(bool    e);
    void TP_defineNormalised(bool      n);

    bool  allDuplicates()    const;

    TVector<ISet*> clusters() const;
    ISet* represenativeSet()  const; //the members closest to the center in each cluster

private:
    void initialise();

    int         m_numberOfClusters;
    bool        m_evaluateDVec;
    bool        m_normalised;
    bool        m_allDuplicates;

    int         m_dimensions;
    Eigen::RowVectorXi m_clusters;
    Eigen::RowVectorXd m_clustersMaxDistances;
    Eigen::RowVectorXi m_largestDimensions;
    int         m_largestCluster;
    Eigen::MatrixXd    m_distances;
    Eigen::MatrixXd    m_centerValues;
    Eigen::MatrixXd    m_maxValues;
    Eigen::MatrixXd    m_minValues;
    Eigen::VectorXd    m_normMinValues;
    Eigen::VectorXd    m_normDistances;

protected:
    void initArrays();
    void getClusterData(ISet* cluster, Eigen::MatrixXd& data) const;
    void calculateClusterDistances(int cluster);
    void cutLargestCluster(int nextCluster);
    void createRepresentativeSet();
};

} // namespace Operators
} // namespace Tigon

#endif // PSACLUSTERING_H
