#ifndef CONSTRAINTPENALTY_H
#define CONSTRAINTPENALTY_H
#include <tigon/Operators/Fitness/IFitness.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT ConstraintPenalty : public IFitness
{
    
    DECLARE_CLASS(Tigon::Operators::ConstraintPenalty)

public:
    ConstraintPenalty();
    ConstraintPenalty(Tigon::Representation::IPSet* ipset);
    ~ConstraintPenalty();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    // Properties


private:
    void initialise();

};

} // namespace Operators
} // namespace Tigon

#endif // CONSTRAINTPENALTY_H
