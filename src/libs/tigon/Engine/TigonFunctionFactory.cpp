#include <tigon/Engine/TigonFunctionFactory.h>
#include <tigon/Representation/Functions/IFunction.h>

#include <boost/range/adaptor/map.hpp>
#include <boost/range/algorithm/copy.hpp>

namespace Tigon {

TigonFunctionFactory::TigonFunctionFactory()
{

}

TigonFunctionFactory* TigonFunctionFactory::instance()
{
    static TigonFunctionFactory instance;
    return &instance;
}

TigonFunctionFactory::~TigonFunctionFactory()
{
    m_ifunctionFactoryRegistry.clear();
}

Representation::IFunction* TigonFunctionFactory::createFunction(const TString &className)
{
    Representation::IFunction* op = 0;
    if(m_ifunctionFactoryRegistry.find(className)
            != m_ifunctionFactoryRegistry.end()) {
        op = m_ifunctionFactoryRegistry[className]->create();
    }
    return op;
}

void TigonFunctionFactory::registerFunctionFactory(const TString &name,
                                                   IFunctionFactory* fact)
{
    if((!name.empty()) && fact) {
        m_ifunctionFactoryRegistry[name] = fact;
    }
}

IFunctionFactory *TigonFunctionFactory::unregisterFactory(const TString &name)
{
    if(mapHasKey(m_ifunctionFactoryRegistry, name)) {
        IFunctionFactory* factory = m_ifunctionFactoryRegistry[name];
        m_ifunctionFactoryRegistry.erase(name);
        return factory;
    } else {
        return nullptr;
    }
}

TStringList TigonFunctionFactory::registeredFunctions() const
{
    TStringList lst;
    boost::copy(m_ifunctionFactoryRegistry | boost::adaptors::map_keys,
                std::back_inserter(lst));

    return lst;
}

}
