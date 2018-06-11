#ifndef NSGAIIELITESELECTION_H
#define NSGAIIELITESELECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>


namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT NSGAIIEliteSelection : public IFiltration
{
    DECLARE_CLASS(Tigon::Operators::NSGAIIEliteSelection)

public:
    NSGAIIEliteSelection();

    NSGAIIEliteSelection(Tigon::Representation::IPSet* ipset);
    virtual ~NSGAIIEliteSelection();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();
    
    

    // Properties
    double TP_eliteRatio()        const;
    void  TP_defineEliteRatio(double r);

private:
    void initialise();

    double          m_eliteRatio;
};

} // namespace Operators
} // namespace Tigon

#endif // NSGAIIELITESELECTION_H
