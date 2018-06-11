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
#include <tigon/Representation/Properties/ProblemPropertiesFactory.h>
#include <tigon/Representation/Properties/ProblemProperties.h>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include <botan/botan.h>

namespace Tigon {
namespace Representation {

ProblemPropertiesFactory* ProblemPropertiesFactory::m_instance = 0;
static boost::mt19937_64 uuidran;
static boost::uuids::basic_random_generator<boost::mt19937_64> uuidgen(&uuidran);


ProblemPropertiesFactory* ProblemPropertiesFactory::instance()
{
    if(m_instance == 0) {
        m_instance = new ProblemPropertiesFactory();
    }
    return m_instance;
}

ProblemProperties ProblemPropertiesFactory::createProblemProperties()
{
    TString name("Generic Problem");
    TString desc("No description available for Generic Problem.");
    ProblemType ptype = Tigon::NonConvex;
    // Internally we only handle minimization type
    //OptimizationType otype = Tigon::Minimization;
    return createProblemProperties(name,desc,ptype);
}

ProblemProperties ProblemPropertiesFactory::createProblemProperties(TString name, TString desc)
{
    ProblemType ptype = Tigon::NonConvex;
    // Internally we only handle minimization type
    //OptimizationType otype = Tigon::Minimization;
    return createProblemProperties(name,desc,ptype);
}

ProblemProperties ProblemPropertiesFactory::createProblemProperties(TString name, TString desc, ProblemType ptype)
{
    ProblemProperties newid = ProblemProperties(name, desc, ptype);
    Botan::SHA_512 sha;
    TString hash = name + desc;
    Botan::SecureVector<Botan::byte> bhash = sha.process(hash);
    std::string chash(reinterpret_cast<const char*>(bhash.begin()),
                  bhash.size());
    TString hashed(chash.c_str());
    newid.setProblemID(hashed);

    return newid;
}


ProblemPropertiesFactory::ProblemPropertiesFactory()
{
    uuidran.seed(time(nullptr));
}

ProblemPropertiesFactory::~ProblemPropertiesFactory()
{
    if(m_instance != 0) {
        delete m_instance;
        m_instance = 0;
    }
}

} // namespace Represenation
} // namespace Tigon
