#ifndef SMSEMOAREDUCE_H
#define SMSEMOAREDUCE_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT SMSEMOAReduce : public IFiltration
{
    DECLARE_CLASS(Tigon::Operators::SMSEMOAReduce)

public:
    SMSEMOAReduce();
    SMSEMOAReduce(Tigon::Representation::IPSet* ipset);
    ~SMSEMOAReduce();

    void evaluateNode();


    // Information about the node
    TString name();
    TString description();

protected:
     void initialise();

private:


};

} // namespace Operators
} // namespace Tigon

#endif // SMSEMOAREDUCE_H
