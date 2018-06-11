#ifndef IPMQPALGORITHM_H
#define IPMQPALGORITHM_H
#include <tigon/tigon_global.h>
#include <tigon/Algorithms/IAlgorithm.h>

namespace Tigon {
namespace Operators {
class IOperator;
}
}

namespace Tigon {
namespace Algorithms {

class LIGER_TIGON_EXPORT IPMQPAlgorithm : public IAlgorithm
{
    Q_OBJECT

public:
    IPMQPAlgorithm();
    IPMQPAlgorithm(Tigon::Representation::IPSet* ipset);
    ~IPMQPAlgorithm();

    void evaluateNode();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

private:
    Tigon::Operators::IOperator*  m_finalOperator;
};

} // namespace Algorithms
} // namespace Tigon

#endif // IPMQPALGORITHM_H
