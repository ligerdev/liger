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
#include <tigon/Log/EvaluationEntry.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Functions/IFunction.h>
#include <tigon/ExceptionHandling/TException.h>
#include <tigon/Utils/JsonUtils.h>

using namespace Json;

namespace Tigon {
namespace Log {

const TString EvaluationEntryClass("EvaluationEntry");


EvaluationEntry::EvaluationEntry()
{

}

EvaluationEntry::EvaluationEntry(IFunctionSPtr func,
                                 const TVector<IElementSPtr> &inputs,
                                 const TVector<IElementSPtr> &outputs,
                                 JsonObject additinalFields)
{
    // Consistency check
    if(  (inputs.size()  != func->TP_nInputs()) ||
         (outputs.size() != func->TP_nOutputs()) ) {
        throw TException(EvaluationEntryClass, RangeException);
    }

    record(LFunc, JsonValue(func->name()));
    record(LInputs,   toJsonObjWithNames(func->inputPrpts(),  inputs));
    record(LOutputs,  toJsonObjWithNames(func->outputPrpts(), outputs));

    TStringList keys = additinalFields.keys();
    for(int i = 0; i < keys.size(); i++) {
        record(keys[i], additinalFields[keys[i]]);
    }
}

EvaluationEntry::~EvaluationEntry()
{

}

} // namespace Log
} // namespace Tigon
