/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#include <tigon/Operators/Initialisation/UserDefinedInit.h>
#include <tigon/ExceptionHandling/TException.h>
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
    if(!m_jsonString.empty()){
        getJsonObjectFromJsonString();
        defineTotalIterations();
        defineSelectedIteration(m_totalIterations);
    }
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
    m_filePath = file;
    if(!content.empty()) {
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
        JsonArray jarray;

        if(m_jsonObj.contains(Log::LSinglePop)) {
            jarray = getSinglePopArray();
        } else if(m_jsonObj.contains(Log::LPops)) {
            jarray = getSelectedIterationPopArray();
        } else if(m_jsonObj.contains(Log::LEvals)) {
            jarray = getEvaluationsArray();
        } else if(m_jsonObj.contains(Log::LDVec)) {
            jarray = getSingleSolutionArray();
        } else {
            /// qDebug() << "Invalid format for initial set";
        }
        getSolutionsFromArray(jarray);
        m_jsonString.clear();
    }

}

void UserDefinedInit::getJsonObjectFromJsonString()
{
    JsonDocument jdoc = JsonDocument::fromJson(m_jsonString);
    m_jsonObj = jdoc.object();
}

JsonArray UserDefinedInit::getSinglePopArray()
{
    JsonArray jarray;

    jarray = m_jsonObj[Log::LSinglePop].toArray();
    return jarray;
}

JsonArray UserDefinedInit::getSelectedIterationPopArray()
{
    JsonArray jarray;

    JsonArray logsArray = m_jsonObj[Log::LPops].toArray();
    if(!(logsArray.empty())){
        jarray = logsArray.at(m_selectedIteration-1).toObject()[Log::LSolutions].toArray();
    }
    return jarray;
}

JsonArray UserDefinedInit::getEvaluationsArray()
{
    JsonArray jarray;

    jarray = m_jsonObj[Log::LEvals].toArray();
    return jarray;
}

JsonArray UserDefinedInit::getSingleSolutionArray()
{
    JsonArray jarray;

    jarray.append(m_jsonObj);
    return jarray;
}

void UserDefinedInit::getSolutionsFromArray(JsonArray jsonArray)
{
    while(!jsonArray.empty()) {
        JsonObject jObj = jsonArray.takeAt(0).toObject();
        IMappingSPtr imap = createMapping();
        bool jsonOK = fromJSonObj(imap, jObj);
        if(!jsonOK) {
            throw TException(this->className(), "Solution could not be added from JSON");
        }
        this->updateIdealNadirVec(imap);
    }
}

int UserDefinedInit::totalIterations()
{
    return m_totalIterations;
}

void UserDefinedInit::defineTotalIterations()
{    
    m_totalIterations = 0;
    if(!m_jsonString.empty()) {
        if(m_jsonObj.contains(Log::LPops)){
            JsonArray popLogArr = m_jsonObj[Log::LPops].toArray();
            m_totalIterations = popLogArr.size();
        } else if(m_jsonObj.contains(Log::LSinglePop)){
            m_totalIterations = 1;
        }
    }
}

void UserDefinedInit::defineTotalIterations(const int &unusedIterationNumber)
{
    defineTotalIterations();
}


int UserDefinedInit::selectedIteration()
{
    return m_selectedIteration;
}

void UserDefinedInit::defineSelectedIteration(const int &iterationNumber)
{
    if(iterationNumber > totalIterations()){
        m_selectedIteration = m_totalIterations;
    } else if (iterationNumber < 0){
        m_selectedIteration = 1;
    } else {
        m_selectedIteration = iterationNumber;
    }

    if(m_totalIterations == 0){
        m_selectedIteration = 0;
    }
}

void UserDefinedInit::initialise()
{
    addProperty("InitialSetFilePath",
                TString("Path of file that contains initial decision vectors")
                , getTType(TString));
    addProperty("SelectedIteration",
                TString("Selected iteration from file"),
                getTType(int));
    addProperty("TotalIterations",
                TString("Total number of iterations in the populations file"),
                getTType(int));

    TP_defineSetSize(0);
    m_filePath = "";
    m_selectedIteration = 0;
    m_totalIterations = 0;
}

} // namespace Operators
} // namespace Tigon


REGISTER_IPSET_FACTORY(Operators, UserDefinedInit)
