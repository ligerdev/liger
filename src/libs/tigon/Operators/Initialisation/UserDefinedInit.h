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
#ifndef USERDEFINEDINIT_H
#define USERDEFINEDINIT_H
#include <tigon/tigon_global.h>
#include <tigon/Operators/Initialisation/IInitialisation.h>
#include <tigon/Utils/TigonIOUtils.h>
#include <tigon/Utils/JsonUtils.h>

namespace Tigon {
namespace Operators {

class LIGER_TIGON_EXPORT UserDefinedInit : public IInitialisation
{
    HANDLE_READ_PROPERTIES_BEGIN(IInitialisation)
    READ(InitialSetFilePath, initialSetFilePath)
    HANDLE_READ_PROPERTIES_END

    HANDLE_WRITE_PROPERTIES_BEGIN(IInitialisation)
    WRITE(InitialSetFilePath, TString, defineInitialSetFromFile)
    HANDLE_WRITE_PROPERTIES_END

    DECLARE_CLASS(Tigon::Operators::UserDefinedInit)

public:
    UserDefinedInit();
    UserDefinedInit(Representation::IPSet* ipset);
    ~UserDefinedInit();

    void  defineSetFromJson(const TString &jsonStr);

    void evaluateNode();

    TString initialSetFilePath() const;
    void defineInitialSetFromFile(const TString& file);

    int totalIterations();
    void defineTotalIterations();
    void defineTotalIterations(const int& unusedIterationNumber);
    int selectedIteration();
    void defineSelectedIteration(const int& iterationNumber);

    // Information about the node
    TString     name();
    TString     description();

protected:
    void appendInitialSet();

private:
    void initialise();
    JsonArray getSinglePopArray();
    JsonArray getSelectedIterationPopArray();
    JsonArray getEvaluationsArray();
    JsonArray getSingleSolutionArray();
    void getJsonObjectFromJsonString();

    void getSolutionsFromArray(JsonArray jsonArray);

    TString      m_filePath;
    TString      m_jsonString;
    int          m_selectedIteration;
    int          m_totalIterations;
    JsonObject   m_jsonObj;
};

} // namespace Operators
} // namespace Tigon

#endif // USERDEFINEDINIT_H
