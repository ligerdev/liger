#include <tigon/Representation/Constraints/NonLinearConstraintsData.h>

namespace Tigon {
namespace Representation {

NonLinearConstraintsData::NonLinearConstraintsData()
{
    m_nvar = 0;
}

NonLinearConstraintsData::NonLinearConstraintsData(const NonLinearConstraintsData& data)
{
    m_nvar      = data.nVar();
    m_rules     = data.linearConstraintRules();
    m_constants = data.linearConstraintConstants();
}

NonLinearConstraintsData::~NonLinearConstraintsData()
{

}

NonLinearConstraintsData* NonLinearConstraintsData::clone()
{
    return (new NonLinearConstraintsData(*this));
}

void NonLinearConstraintsData::setNVar(int nvar)
{
    m_nvar = nvar;
}

void NonLinearConstraintsData::addLinearConstraint(TMap<int, double> rule,
                                                double constant)
{
    m_rules.push_back(rule);
    m_constants.push_back(constant);
}

int NonLinearConstraintsData::nVar() const
{
    return m_nvar;
}

TMatrixReal NonLinearConstraintsData::linearConstraintsAMatrix()
{
    bool sOK = sizeConsistencyCheck();
    if(!sOK) {
//        throw Tigon::TigonException("Sizes are incosistent in NonLinearConstraintsData.");
    }
    generateAMatrix();
    return m_aMatrix;
}

TCVectorReal NonLinearConstraintsData::linearConstraintsBVector()
{
    bool sOK = sizeConsistencyCheck();
    if(!sOK) {
//        throw Tigon::TigonException("Sizes are inconsistent in NonLinearConstraintsData.");
    }
    generateBVector();
    return m_bVector;
}

TVector<TMap<int, double> > NonLinearConstraintsData::linearConstraintRules() const
{
    return m_rules;
}

TVector<double> NonLinearConstraintsData::linearConstraintConstants() const
{
    return m_constants;
}

void NonLinearConstraintsData::clearConstraints()
{
    m_rules.clear();
    m_constants.clear();
    m_nvar = 0;
    m_aMatrix.resize(1,1);
    m_bVector.resize(1,1);
}

void NonLinearConstraintsData::generateAMatrix()
{
    m_aMatrix.resize(m_rules.size(), m_nvar);
    m_aMatrix.setZero(m_rules.size(), m_nvar);
    for(int i=0; i<m_rules.size(); i++) {
        TMap<int, double> map = m_rules.at(i);
        for(TMap<int, double>::iterator itr = map.begin();
            itr != map.end(); ++itr) {
            m_aMatrix(i, itr->first) = itr->second;
        }
    }
}

void NonLinearConstraintsData::generateBVector()
{
    m_bVector.resize(m_constants.size(),1);
    m_bVector.setZero();
    for(int i=0; i<m_constants.size(); i++) {
        m_bVector(i) = m_constants.at(i);
    }
}

bool NonLinearConstraintsData::sizeConsistencyCheck() const
{
    bool ret = false;
    if(m_rules.size() == m_constants.size()) {
        ret = true;
    }
    return ret;
}

} // namespace Representation
} // namespace Tigon
