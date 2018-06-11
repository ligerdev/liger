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
#include <tigon/Representation/Sets/ISet.h>
#include <tigon/Representation/Mappings/IMapping.h>
#include <tigon/Utils/TigonUtils.h>

namespace Tigon {
namespace Representation {

ISet::ISet()
{

}

// When initialised with an existing set, a new copy is created:
ISet::ISet(ISet* sSet)
{
    defineTags(sSet->tags());
    define(sSet->all());
}

ISet::ISet(const TVector<IMappingSPtr> &sVec)
{
    define(sVec);
}

ISet::ISet(const TStringList &tags)
{
    defineTags(tags);
}

ISet::ISet(ISet *sSet, const TStringList &tags)
{
    define(sSet->all());
    defineTags(tags);
}

ISet::ISet(const TVector<IMappingSPtr> &sVec, const TStringList &tags)
{
    define(sVec);
    defineTags(tags);
}

ISet::ISet(const TString &tag)
{
    addTag(tag);
}

ISet::ISet(ISet *sSet, const TString &tag)
{
    define(sSet->all());
    addTag(tag);
}

ISet::ISet(const TVector<IMappingSPtr> &sVec, const TString &tag)
{
    define(sVec);
    addTag(tag);
}

ISet::~ISet()
{
    clearMappings();
}

ISet* ISet::clone()
{
    return new ISet(this);
}

// Tags methods
TStringList ISet::tags() const
{
    return m_tags;
}

void ISet::defineTags(const TStringList &tags)
{
    m_tags = tags;
}

void ISet::clearTags()
{
    m_tags.clear();
}

void ISet::addTag(const TString &tag)
{
    if(!isTagged(tag)) {
        m_tags.push_back(tag);
    }
}

void ISet::removeTag(const TString &tag)
{
    removeAll(m_tags, tag);
}

bool ISet::isTagged(const TString &tag)
{
    return Tigon::contains(m_tags, tag);
}

bool ISet::isTagged(const TStringList &tags)
{
    for(int t=0; t<tags.size(); t++) {
        if(!isTagged(tags[t])) {
            return false;
        }
    }
    return true;
}

// IMapping containter methods
TVector<IMappingSPtr> ISet::all() const
{
    return m_mappings;
}

/* Returns a vector whose elements are copied from this vector,
 * starting at position pos. If length is -1 (the default),
 * all elements after pos are copied; otherwise length elements
 * (or all remaining elements if there are less than length elements) are copied.
 */
TVector<IMappingSPtr> ISet::range(int from, int length) const
{
    return mid(m_mappings, from, length);
}

int ISet::size() const
{
    return static_cast<int>(m_mappings.size());
}

IMappingSPtr ISet::at(int idx)
{
    IMappingSPtr ret;
    if(idx<=size() && idx >= 0) {
        ret = m_mappings.at(idx);
    }
    return ret;
}

void ISet::define(int idx, IMappingSPtr iMap)
{
    if(idx<size() && idx >= 0) {
        m_mappings[idx] = iMap;
    }
}

void ISet::define(const TVector<IMappingSPtr> &sVec)
{
    m_mappings = sVec;
}

void ISet::append(IMappingSPtr iMap)
{
    m_mappings.push_back(iMap);
}

void ISet::append(ISet* iSet)
{
    for(int idx = 0; idx < iSet->size(); idx++) {
        append(iSet->at(idx));
    }
}

bool ISet::contains(IMappingSPtr iMap)
{
    return vectorContains(m_mappings, iMap);
}

int ISet::indexOf(IMappingSPtr iMap)
{
    return vectorIndexOf(m_mappings, iMap);
}

bool ISet::remove(int idx)
{
    bool inBounds = ((idx < 0) | (idx >= m_mappings.size())) ? false : true;
    if(inBounds) {
        m_mappings.erase(m_mappings.begin() + idx);
    }
    return inBounds;
}

bool ISet::remove(IMappingSPtr iMap)
{
    int idx = vectorIndexOf(m_mappings, iMap);
    if(idx < 0) {
        return false;
    }

    while(idx >= 0) {
        remove(idx);
        idx = vectorIndexOf(m_mappings, iMap);
    }
    return true;
}

bool ISet::replace(IMappingSPtr oldMap, IMappingSPtr newMap)
{
    int idx = vectorIndexOf(m_mappings, oldMap);
    if(idx > -1) {
        m_mappings[idx]= newMap;
        return true;
    } else {
        return false;
    }
}

void ISet::shuffle()
{
    TVector<IMappingSPtr> shuffled = Tigon::shuffle(m_mappings);
    m_mappings.swap(shuffled);
}

void ISet::clearMappings()
{
    m_mappings.clear();
}

} // namespace Representation
} // namespace Tigon
