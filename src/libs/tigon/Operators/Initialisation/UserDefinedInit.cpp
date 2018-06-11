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
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Utils/TigonIOUtils.h>
#include <tigon/Utils/JsonUtils.h>
#include <tigon/tigonengineregistry.h>

#include <json/json.h>
using namespace Json;

namespace Tigon {
namespace Operators {

UserDefinedInit::UserDefinedInit()
{
    initialise();
}

UserDefinedInit::UserDefinedInit(Representation::IPSet* ipset)
    : IInitialisation(ipset)
{
    initialise();
}

UserDefinedInit::~UserDefinedInit()
{

}

void UserDefinedInit::defineSetFromJson(const TString &jsonStr)
{
    m_jsonString = jsonStr;
}

void UserDefinedInit::evaluateNode()
{
    if( outputSets().empty() ) {
        appendOutputSet();
        appendInitialSet();
    }
}

TString UserDefinedInit::initialSetFilePath() const
{
    return m_filePath;
}

void UserDefinedInit::defineInitialSetFromFile(const TString &file)
{
    TString content = readAll(file);
    if(!content.empty()) {
        m_filePath = file;
        defineSetFromJson(content);
    }
    return;
}

TString UserDefinedInit::name()
{
    return TString("User Defined Initialisation");
}

TString UserDefinedInit::description()
{
    return TString("Generate a set of inital solutions from copies of "
                   "the decision vectors of user defined solutions.");
}

void UserDefinedInit::appendInitialSet()
{
    IInitialisation::appendInitialSet();

    // Copy from Json string
    if(!m_jsonString.empty()) {
        JsonDocument jdoc = JsonDocument::fromJson(m_jsonString);
        JsonObject json = jdoc.object();
        JsonArray jarray;

        if(json.contains(Log::LSinglePop)) {
            /// json object holds a single population array
            jarray = json[Log::LSinglePop].toArray();
        } else if(json.contains(Log::LPops)) {
            /// json object has a population log
            JsonArray popLogArr = json[Log::LPops].toArray();
            if(!(popLogArr.empty())) {
                // Only use the last population entry
                jarray = popLogArr.last().toObject()[Log::LSolutions].toArray();
            }
        } else if(json.contains(Log::LEvals)) {
            /// json object has an evaluation log
            jarray = json[Log::LEvals].toArray();
        } else if(json.contains(Log::LDVec)) {
            /// json object is a single solution
            jarray.append(json);
        } else {
            /// qDebug() << "Invalid format for initial set";
        }

        while(!jarray.empty()) {
            JsonObject jObj = jarray.takeAt(0).toObject();
            IMappingSPtr imap = createMapping();
            bool jsonOK = fromJSonObj(imap, jObj);
            if(!jsonOK) {
                throw TException(this->className(), "Solution could not be added from JSON");
            }
            this->updateIdealNadirVec(imap);
        }
        m_jsonString.clear();
    }

}

void UserDefinedInit::initialise()
{
    addProperty("InitialSetFilePath",
                TString("Path of file that contains initial decision vectors")
                , getType(TString));
    addProperty("SelectedIteration",
                TString("Selected iteration from file"),
                getType(int));
    addProperty("TotalIterations",
                TString("Total number of iterations in the populations file"),
                getType(int));

    TP_defineSetSize(0);
    m_filePath = "";
}

} // namespace Operators
} // namespace Tigon


REGISTER_IPSET_FACTORY(Operators, UserDefinedInit)
