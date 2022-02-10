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
#include <tigon/Operators/ObjectiveReduction/NLMVUPCA.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/TigonUtils.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Representation/Problems/Problem.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Operators {

NLMVUPCA::NLMVUPCA()
{
    initialise();
}

NLMVUPCA::NLMVUPCA(Tigon::Representation::IPSet* ipset)
    : IObjectiveReduction(ipset)
{
    initialise();
}

NLMVUPCA::~NLMVUPCA()
{
    delete m_objred;
}

void NLMVUPCA::initialise()
{
    m_objred = new MVUPCABasedObjectiveReduction();

    addProperty("Standardize"
                , TString("Indicates if the data is to be standardized, that is,"
                          "normalised to unit variance.\n"
                          "Default is true.")
                , getTType(bool));

    TP_defineStandardize(true);
}

void NLMVUPCA::evaluateNode()
{
    if(TP_operateOnFinal() && !isTerminate()) {
        return;
    }

    if(!hasNextInputSet()) {
        return;
    }

    ISet* iSet = setsUnion(inputSets());

    ISet* evaluatedSolutions = new ISet();
    for(IMappingSPtr sol: iSet->all()) {
        if(sol->isEvaluated()) {
            evaluatedSolutions->append(sol);
        }
    }

    if(evaluatedSolutions->size()==0) {
        throw TException(className(),
                         "The input set has no evaluated solutions");
    }

    m_objred->updateData(evaluatedSolutions);


    TVector<bool> essentialObjectiveSet(objectiveVecSize(), false);
    for(int e : m_objred->essentialSet()) {
        essentialObjectiveSet[e] = true;
    }

    ObjectiveReductionData* data = objectiveReductionData();
    data->defineEssentialObjectiveSet(essentialObjectiveSet);
    data->defineError(m_objred->error());
    data->defineVariance(m_objred->variance());
    data->defineVariancePerPCA(m_objred->eigenValues());

    CorrelationMatrix* corMatrix = m_objred->correlationMatrix();
    TMap<int, TVector<int>> corrIdx = m_objred->correlatedObjectiveSubSets();

    TMap<int, TVector<CorrelatedObjectives>> corr;

    auto keys = getMapKeys(corrIdx);
    for(int key : keys) {
        TVector<int> v = corrIdx[key];
        TVector<CorrelatedObjectives> gg;
        for(int elem : v) {
            CorrelatedObjectives ss;
            ss.objectiveIdx = elem;
            ss.correlationStrength = corMatrix->correlationMatrix()(key,elem);
            gg.push_back(ss);
        }
        corr[key] = gg;
    }

    data->defineCorrelatedObjectives(corr);
}

TString NLMVUPCA::name()
{
    return TString("Non-linear Maximum Variance Unfolding PCA (NL-MVU-PCA)");
}

TString NLMVUPCA::description()
{
    return TString("NL-MVU-PCA. Objective reduction algorithm based on maximum "
                   "variance unfolding and linear PCA, "
                   "an unsupervised machine learning technique. The algorithm "
                   "operates on a non-dominated set of objective vectors, and "
                   "reveals an 'essential' objective set, and the error incurred "
                   "per objective. The essential objective set is the smallest "
                   "subset of objectives that provide the same Pareto-optimal "
                   "set as the entire objective set. The error incurred by each "
                   "objective indicates which objectives to omit in case it is "
                   "desirable to reduce the number of objectives futher than the "
                   "essential objective set, and also can serve as a way to "
                   "generate a preference-ordering ranking between the "
                   "objectives.\n"
                   "Taken from D. K. Saxena, J. A. Duro, A. Tiwari, K. Deb and "
                   "Q. Zhang, \"Objective Reduction in Many-Objective Optimization: "
                   "Linear and Nonlinear Algorithms,\" in IEEE Transactions on "
                   "Evolutionary Computation, vol. 17, no. 1, pp. 77-99, Feb. "
                   "2013, doi: 10.1109/TEVC.2012.2185847.");
}

bool NLMVUPCA::TP_standardize() const
{
    return m_objred->getStandardize();
}

void NLMVUPCA::TP_defineStandardize(bool s)
{
    m_objred->defineStandardize(s);
}

} // namespace Operators
} // namespace Tigon
