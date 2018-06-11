#ifndef NONLINEARCONSTRAINTSDATA_H
#define NONLINEARCONSTRAINTSDATA_H
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Representation {
class NonLinearConstraintsData;
}
}

namespace Tigon {
namespace Representation {

// TODO - this is just a place holder. Not working
class LIGER_TIGON_EXPORT NonLinearConstraintsData
{
public:
    NonLinearConstraintsData();
    NonLinearConstraintsData(const NonLinearConstraintsData& data);
    ~NonLinearConstraintsData();
    NonLinearConstraintsData* clone();

    void setNVar(int nvar);
    void addLinearConstraint(TMap<int, double> rule, double constant);
    int  nVar() const;
    TMatrixReal  linearConstraintsAMatrix();
    TCVectorReal linearConstraintsBVector();
    TVector<TMap<int, double> > linearConstraintRules() const;
    TVector<double>  linearConstraintConstants() const;
    void clearConstraints();

private:
    // The TMap stores: (variable number, multiplier)
    int m_nvar;
    TVector<TMap<int, double> > m_rules;
    TVector<double> m_constants;
    TMatrixReal  m_aMatrix;
    TCVectorReal m_bVector;
    void generateAMatrix();
    void generateBVector();
    bool sizeConsistencyCheck() const;
};

} // namespace Representation
} // namespace Tigon

#endif // NONLINEARCONSTRAINTSDATA_H
