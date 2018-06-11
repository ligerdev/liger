#ifndef IPMQPDIRECTION_H
#define IPMQPDIRECTION_H
#include <tigon/Operators/Directions/IDirection.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT IPMQPDirection : public IDirection
{
    Q_OBJECT

public:
    IPMQPDirection();
    IPMQPDirection(IPSet* ipset);
    ~IPMQPDirection();
    void evaluateNode();

    // Information about the node.
    QString     name();
    QString     description();
    QStringList propertyNames();
    QStringList propertyDescriptions();

private:
    void getStepSizes(qreal& alphaz, qreal& alphas, TCVectorReal z, TCVectorReal s,
                              TCVectorReal dz, TCVectorReal ds);
};

} // namespace Operators
} // namespace Tigon

#endif // IPMQPDIRECTION_H
