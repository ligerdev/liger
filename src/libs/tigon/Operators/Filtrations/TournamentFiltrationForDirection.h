#ifndef TOURNAMENTFILTRATIONFORDIRECTION_H
#define TOURNAMENTFILTRATIONFORDIRECTION_H
#include <tigon/Operators/Filtrations/IFiltration.h>
#include <tigon/tigon_global.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT TournamentFiltrationForDirection : public IFiltration
{
    HANDLE_READ_PROPERTIES_BEGIN(IFiltration)
    READ(MappingsPerSet, TP_mappingsPerSet)
    READ(NumberOfMappings, TP_numberOfMappings)
    //READ(SelectionMethod, TP_selectionMethod)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IFiltration)
    WRITE(MappingsPerSet, int, TP_defineMappingsPerSet)
    WRITE(NumberOfMappings, int, TP_defineNumberOfMappings)
    //WRITE(SelectionMethod, Tigon::SelectionType, TP_defineSelectionMethod)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::TournamentFiltrationForDirection)

public:
    TournamentFiltrationForDirection();
    TournamentFiltrationForDirection(Tigon::Representation::IPSet* ipset);
    virtual ~TournamentFiltrationForDirection();
    void initialise();

    void evaluateNode();

    // Information about the node.
    TString     name();
    TString     description();

    // Properties
    void  TP_defineMappingsPerSet(int n);
    int   TP_mappingsPerSet()      const;

    void  TP_defineNumberOfMappings(int n);
    int   TP_numberOfMappings()      const;

    void TP_defineSelectionMethod(SelectionType q);
    SelectionType TP_selectionMethod()       const;

private:
    TString m_name;
    TString m_description;

    int   m_mappingsPerSet;
    int   m_numberOfMappings;
    SelectionType m_selectionMethod;
};

} // namespace Operators
} // namespace Tigon

#endif // TOURNAMENTFILTRATIONFORDIRECTION_H
