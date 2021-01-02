/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
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
#include <tigon/Representation/Properties/FunctionPropertiesFactory.h>
#include <tigon/Representation/Properties/FunctionProperties.h>


#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <botan/botan.h>

namespace Tigon {
namespace Representation {

FunctionPropertiesFactory* FunctionPropertiesFactory::m_instance = 0;

static boost::mt19937_64 uuidran;
static boost::uuids::basic_random_generator<boost::mt19937_64> uuidgen(&uuidran);

FunctionPropertiesFactory* FunctionPropertiesFactory::instance()
{
    if(m_instance == nullptr) {
        m_instance = new FunctionPropertiesFactory();
    }
    return m_instance;
}

FunctionProperties FunctionPropertiesFactory::createFunctionProperties(TString name,
                                                                       TString desc,
                                                                       FunctionType type)
{
    FunctionProperties newfp = FunctionProperties(name, desc, type);
    Botan::SHA_512 sha;
    TString hash = name + desc;
    Botan::SecureVector<Botan::byte> bhash = sha.process(hash);
    std::string chash(reinterpret_cast<const char*>(bhash.begin()), bhash.size());
    TString hashed = TString(chash.c_str());
    newfp.setFunctionID(hashed);

    return newfp;
}

FunctionPropertiesFactory::FunctionPropertiesFactory()
{
    uuidran.seed(time(nullptr));
}

FunctionPropertiesFactory::~FunctionPropertiesFactory()
{
    if(m_instance != nullptr) {
        delete m_instance;
        m_instance = nullptr;
    }
}

} // namespace Represenation
} // namespace Tigon
