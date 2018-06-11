#ifndef RANDSETREPLACEMENT_H
#define RANDSETREPLACEMENT_H
#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT RandSetReplacement : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(ReplacementProbability, TP_replacementProbability)
    READ(ReplacementSetTags, TP_replacementSetTags)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(ReplacementProbability, double, defineReplacementProbability)
    WRITE(ReplacementSetTags, TString, TP_defineReplacementSetTags)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::RandSetReplacement)

public:
    RandSetReplacement();
    RandSetReplacement(Tigon::Representation::IPSet* ipset);
    virtual ~RandSetReplacement();
    void initialise();

    void evaluateNode();

    // Information about the node
    TString     name();
    TString     description();
    
    

    /**************/
    /* Properties */
    /**************/

    // Replacement Probability
    double TP_replacementProbability()       const;
    void  defineReplacementProbability(double u);

    TString TP_replacementSetTags() const;
    void TP_defineReplacementSetTags(const TString &tags);

    TStringList replacementSetTags() const;
    void defineReplacementSetTags(TStringList tags);

    bool doSetReplacement();

private:
    TString m_name;
    TString m_description;
    double   m_replacementProbability;
    ISet*   m_replacementSet;
    TStringList m_replacementSetTags;
};

} // namespace Operators
} // namespace Tigon

#endif // RANDSETREPLACEMENT_H
