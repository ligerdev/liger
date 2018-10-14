#ifndef SCALARIZATION_H
#define SCALARIZATION_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT Scalarization : public IFitness
{
    //HANDLE_READ_PROPERTIES_BEGIN(IFitness)
    //READ(ScalarisingFunction, TP_scalarisingFunction)
    //READ(WeightScopeApproach, TP_weightScopeApproach)
    //HANDLE_READ_PROPERTIES_END

    //HANDLE_WRITE_PROPERTIES_BEGIN(IFitness)
    //WRITE(ScalarisingFunction, Tigon::ScalarisationType, TP_defineScalarisingFunction)
    //WRITE(WeightScopeApproach, Tigon::WeightScope, TP_defineWeightScopeApproach)
    //HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::Scalarization)

public:
    Scalarization();
    Scalarization(Tigon::Representation::IPSet* ipset);
    virtual ~Scalarization();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    // Properties
    void  TP_defineScalarisingFunction(Tigon::ScalarisationType t, double p=1.0);
    Tigon::ScalarisationType TP_scalarisingFunction() const;

    void TP_defineWeightScopeApproach(Tigon::WeightScope s);
    Tigon::WeightScope TP_weightScopeApproach() const;

    double pNorm() const;
    Tigon::WeightScope weightScopeApproach();

private:
    void initialise();
    TString   m_name;
    TString   m_description;

    TVector<double>             m_weight;
    Tigon::ScalarisationType m_scalarisingFunc;
    double                       m_pNorm;
    Tigon::WeightScope m_weightScopeApproach;
};

} // namespace Operators
} // namespace Tigon

#endif // SCALARIZATION_H
