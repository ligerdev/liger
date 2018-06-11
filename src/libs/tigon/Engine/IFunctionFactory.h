#ifndef IFUNCTIONFACTORY_H
#define IFUNCTIONFACTORY_H
#include <tigon/tigon_global.h>

namespace Tigon {

class LIGER_TIGON_EXPORT IFunctionFactory
{
public:
    IFunctionFactory(){}
    virtual Tigon::Representation::IFunction* create() = 0;
};
}
#endif // IFUNCTIONFACTORY_H
