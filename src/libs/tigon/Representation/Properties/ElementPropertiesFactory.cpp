/****************************************************************************
**
** Copyright (C) 2012-2018 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/
#include <tigon/Representation/Properties/ElementPropertiesFactory.h>
#include <tigon/Representation/Properties/ElementProperties.h>


#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <botan/botan.h>

namespace Tigon {
namespace Representation {

ElementPropertiesFactory* ElementPropertiesFactory::m_instance = 0;
static boost::mt19937_64 uuidran;
static boost::uuids::basic_random_generator<boost::mt19937_64> uuidgen(&uuidran);


ElementPropertiesFactory* ElementPropertiesFactory::instance()
{
    if(m_instance == nullptr) {
        m_instance = new ElementPropertiesFactory();
    }
    return m_instance;
}

ElementProperties ElementPropertiesFactory::createElementProperties(int idx)
{    
    const TString &name = TString("Element") + std::to_string(idx);
    const TString &desc = TString("No description available for element.");
    return createElementProperties(idx, name, desc);
}

ElementProperties ElementPropertiesFactory::
createElementProperties(int idx, const TString &name, const TString &desc)
{    
    return createElementProperties(idx,name,desc,Tigon::RealType);
}

ElementProperties ElementPropertiesFactory::
createElementProperties(int idx, const TString &name, const TString &desc,
                        ElementType type)
{
    OptimizationType otype = Tigon::NonOptimization;
    return createElementProperties(idx, name, desc, type, otype);
}

ElementProperties ElementPropertiesFactory::
createElementProperties(int idx, const TString &name, const TString &desc,
                        ElementType type,
                        OptimizationType otype)
{
    ElementProperties newid = ElementProperties(idx, name, desc, type, otype);
    TString hashed = generateID(name + desc);
    newid.setElementID(hashed);

    return newid;
}

TString ElementPropertiesFactory::generateID(const TString &str) const
{
    Botan::SHA_512 sha;
    TString hashed = Botan::hex_encode(sha.process(str));
    return hashed;
}

ElementPropertiesFactory::ElementPropertiesFactory()
{
    uuidran.seed(time(nullptr));
}

ElementPropertiesFactory::~ElementPropertiesFactory()
{
    if(m_instance != nullptr) {
        delete m_instance;
        m_instance = nullptr;
    }
}

} // namespace Represenation
} // namespace Tigon
