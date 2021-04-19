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
#include <tigon/Log/PopulationEntry.h>
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Utils/JsonUtils.h>

namespace Tigon {
namespace Log {

const TString PopulationEntryClass("PopulationEntry");

PopulationEntry::PopulationEntry()
{

}

PopulationEntry::PopulationEntry(ISet* pop)
{
    if(pop != nullptr) {

        TStringList list = pop->tags();

        record(LTags, JsonValue(join(list,Tigon::TagDelimiter)));

        record(LSize, pop->size());

        record(LSolutions, toJsonArray(pop));
    }

}

PopulationEntry::PopulationEntry(ISet *pop, const TString &name,
                                 const JsonObject &additinalFields)
{
    if(pop != nullptr) {
        if(!name.empty()) {
            record(LName, JsonValue(name));
        } else {
            TStringList list = pop->tags();
            record(LTags, JsonValue(join(list,Tigon::TagDelimiter)));
        }

        record(LSize, pop->size());

        record(LSolutions, toJsonArray(pop));

        TStringList keys = additinalFields.keys();
        for(size_t i = 0; i < keys.size(); i++) {
            record(keys[i], additinalFields[keys[i]]);
        }
    }
}

PopulationEntry::~PopulationEntry()
{

}

} // namespace Log
} // namespace Tigon
