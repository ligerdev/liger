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
#include <tigon/Representation/Properties/ObjectiveReductionData.h>
#include <tigon/ExceptionHandling/TException.h>

namespace Tigon {
namespace Representation {

ObjectiveReductionData::ObjectiveReductionData(const TVector<bool>& set)
{
    if(set.size() < 1) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "There needs to be at least one objective.");
    }
    m_essentialObjectiveSet = set;
    m_error.resize(set.size(),1.0/(double)set.size());
    m_variance.resize(set.size(),1.0);
    m_variancePerPCA.resize(set.size(), 1.0);
    for(int i=0; i<set.size(); i++) {

        TVector<CorrelatedObjectives> sub;
        sub.push_back(CorrelatedObjectives());
        sub[0].objectiveIdx = i;
        sub[0].correlationStrength = 1.0;
        m_correlatedObjectives[i] = sub;
    }
}

ObjectiveReductionData::ObjectiveReductionData(const TVector<bool>& set,
                                               const TVector<double>& err)
{
    if(set.size() < 1) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "There needs to be at least one objective.");
    }

    if(set.size() != err.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding error "
                         "vector need to have the same size.");
    }

    m_essentialObjectiveSet = set;
    m_error = err;
    m_variance.resize(set.size(), 1.0);
    m_variancePerPCA.resize(set.size(), 1.0);
    for(int i=0; i<set.size(); i++) {
        TVector<CorrelatedObjectives> sub;
        sub.push_back(CorrelatedObjectives());
        sub[0].objectiveIdx = i;
        sub[0].correlationStrength = 1.0;
        m_correlatedObjectives[i] = sub;
    }
}

ObjectiveReductionData::ObjectiveReductionData(
        const TVector<bool>& set, const TVector<double>& err,
        const TVector<double>& var)
{
    if(set.size() < 1) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "There needs to be at least one objective.");
    }

    if(set.size() != err.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding error "
                         "vector need to have the same size.");
    }
    if(set.size() != var.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding variance "
                         "vector need to have the same size.");
    }

    m_essentialObjectiveSet = set;
    m_error = err;
    m_variance = var;
    m_variancePerPCA.resize(set.size(), 1.0);
    for(int i=0; i<set.size(); i++) {
        TVector<CorrelatedObjectives> sub;
        sub.push_back(CorrelatedObjectives());
        sub[0].objectiveIdx = i;
        sub[0].correlationStrength = 1.0;
        m_correlatedObjectives[i] = sub;
    }
}

ObjectiveReductionData::ObjectiveReductionData(
        const TVector<bool>& set, const TVector<double>& err,
        const TVector<double>& var,
        const TVector<double>& varPCA)
{
    if(set.size() < 1) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "There needs to be at least one objective.");
    }

    if(set.size() != err.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding error "
                         "vector need to have the same size.");
    }
    if(set.size() != var.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding variance "
                         "vector need to have the same size.");
    }

    if(set.size() != varPCA.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding variance "
                         "per PCA vector need to have the same size.");
    }

    m_essentialObjectiveSet = set;
    m_error = err;
    m_variance = var;
    m_variancePerPCA = varPCA;
    for(int i=0; i<set.size(); i++) {
        TVector<CorrelatedObjectives> sub;
        sub.push_back(CorrelatedObjectives());
        sub[0].objectiveIdx = i;
        sub[0].correlationStrength = 1.0;
        m_correlatedObjectives[i] = sub;
    }
}

ObjectiveReductionData::ObjectiveReductionData(
        const TVector<bool>& set, const TVector<double>& err,
        const TVector<double>& var,
        const TVector<double>& varPCA,
        const TMap<int, TVector<CorrelatedObjectives>>& corObjs)
{
    if(set.size() < 1) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "There needs to be at least one objective.");
    }

    if(set.size() != err.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding error "
                         "vector need to have the same size.");
    }
    if(set.size() != var.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding variance "
                         "vector need to have the same size.");
    }
    if(set.size() != varPCA.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and corresponding variance "
                         "per PCA vector need to have the same size.");
    }
    if(set.size() != corObjs.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and the number of objectives "
                         "in the correlated map need to have the same size.");
    }
    m_essentialObjectiveSet = set;
    m_error = err;
    m_variance = var;
    m_variancePerPCA = varPCA;
    m_correlatedObjectives = corObjs;
}

ObjectiveReductionData::ObjectiveReductionData(const ObjectiveReductionData& data)
{
    m_essentialObjectiveSet = data.m_essentialObjectiveSet;
    m_error = data.m_error;
    m_variance = data.m_variance;
    m_variancePerPCA = data.m_variancePerPCA;
    m_correlatedObjectives = data.m_correlatedObjectives;
}

ObjectiveReductionData::~ObjectiveReductionData()
{

}

ObjectiveReductionData&
ObjectiveReductionData::operator=(const ObjectiveReductionData& other)
{
    if(this != &other) {
        m_essentialObjectiveSet = other.m_essentialObjectiveSet;
        m_error = other.m_error;
        m_variance = other.m_variance;
        m_variancePerPCA = other.m_variancePerPCA;
        m_correlatedObjectives = other.m_correlatedObjectives;
    }
    return *this;
}

ObjectiveReductionData* ObjectiveReductionData::clone() const
{
    return new ObjectiveReductionData(*this);
}

TVector<bool> ObjectiveReductionData::essentialObjectiveSet() const
{
    return m_essentialObjectiveSet;
}
void ObjectiveReductionData::defineEssentialObjectiveSet(const TVector<bool>& set)
{
    if(set.size() < 1) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "There needs to be at least one objective.");
    }
    m_essentialObjectiveSet = set;
    if(set.size() != m_error.size()) {
        m_error.resize(set.size(), 1.0/(double)set.size());
    }
    if(set.size() != m_variance.size()) {
        m_variance.resize(set.size(), 1.0);
    }
    if(set.size() != m_variancePerPCA.size()) {
        m_variancePerPCA.resize(set.size(), 1.0);
    }
    if(set.size() != m_correlatedObjectives.size()) {
        m_correlatedObjectives.clear();
        for(int i=0; i<set.size(); i++) {
            CorrelatedObjectives cor;
            cor.objectiveIdx = i;
            cor.correlationStrength = 1.0;
            m_correlatedObjectives[i] = TVector<CorrelatedObjectives>(1,cor);
        }
    }
}

TVector<double> ObjectiveReductionData::error() const
{
    return m_error;
}

void ObjectiveReductionData::defineError(const TVector<double>& error)
{
    if(error.size() != m_essentialObjectiveSet.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The size of the error vector needs to be equal to the "
                         "essential objective set.");
    }
    m_error = error;
}

TVector<double> ObjectiveReductionData::variance() const
{
    return m_variance;
}

void ObjectiveReductionData::defineVariance(const TVector<double>& var)
{
    if(var.size() != m_essentialObjectiveSet.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The size of the variance vector needs to be equal "
                         "to the essential objective set.");
    }
    m_variance = var;
}

TVector<double> ObjectiveReductionData::variancePerPCA() const
{
    return m_variancePerPCA;
}

void ObjectiveReductionData::defineVariancePerPCA(const TVector<double>& var)
{
    if(var.size() != m_essentialObjectiveSet.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The size of the variance per PCA vector needs to be "
                         "equal to the essential objective set.");
    }
    m_variancePerPCA = var;
}

TMap<int, TVector<CorrelatedObjectives>>
ObjectiveReductionData::correlatedObjectives() const
{
    return m_correlatedObjectives;
}

void ObjectiveReductionData::defineCorrelatedObjectives(
        const TMap<int, TVector<CorrelatedObjectives>>& corObjs)
{
    if(corObjs.size() != m_essentialObjectiveSet.size()) {
        throw TException("Tigon::Representation::ObjectiveReductionData",
                         "The essential objective set and the number of objectives "
                         "in the correlated map need to have the same size.");
    }
    m_correlatedObjectives = corObjs;
}

int ObjectiveReductionData::size( ) const
{
    return static_cast<int>(m_essentialObjectiveSet.size());
}

} // namespace Representation
} // namespace Tigon
