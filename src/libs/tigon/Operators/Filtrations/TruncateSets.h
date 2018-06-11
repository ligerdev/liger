#ifndef RESIZESETS_H
#define RESIZESETS_H

#include <tigon/Operators/Filtrations/IFiltration.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT TruncateSets: public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(SetSize, TP_setSize)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(SetSize, int, TP_defineSetSize)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::TruncateSets)

public:
    TruncateSets();
    TruncateSets(Representation::IPSet* ipset);
    virtual ~TruncateSets();
    void initialise();

    void evaluateNode();

    // Information about the node
    TString     name();
    TString     description();
    
    

    /**************/
    /* Properties */
    /**************/

    // Set size
    int  TP_setSize() const;
    void TP_defineSetSize(int s);

private:
    TString m_name;
    TString m_description;
    int m_setSize;
};

} // namespace Operators
} // namespace Tigon

#endif // RESIZESETS_H
