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
#ifndef ISET_H
#define ISET_H
#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

namespace Tigon {
namespace Representation {

/*!
 * \brief ISet represents a population set
 * \todo Make this class STL compatible
 * \todo Rename this class to PopulationSet
 */
class LIGER_TIGON_EXPORT ISet
{
public:
    ISet();
    ISet(ISet* sSet);
    ISet(const TVector<IMappingSPtr>  &sVec);
    ISet(const TStringList &tags);
    ISet(ISet* sSet, const TStringList &tags);
    ISet(const TVector<IMappingSPtr> &sVec, const TStringList &tags);
    ISet(const TString &tag);
    ISet(ISet* sSet, const TString &tag);
    ISet(const TVector<IMappingSPtr> &sVec, const TString &tag);

    ~ISet();

    ISet* clone();

    /// Tags
    TStringList  tags()                 const;
    void         defineTags(const TStringList &tags);
    void         clearTags();
    void         addTag(const TString &tag);
    void         removeTag(const TString &tag);
    bool         isTagged(const TString &tag);
    bool         isTagged(const TStringList &tags);

    /// IMappings
    TVector<IMappingSPtr> all() const;
    TVector<IMappingSPtr> range(int from, int length=-1) const;
    int size() const;

    IMappingSPtr at(int idx);

    void define(int idx, IMappingSPtr iMap); //does nothing if the index is out of bounds
    void define(const TVector<IMappingSPtr> &sVec);
    void append(IMappingSPtr iMap);
    void append(ISet* iSet);
    bool contains(IMappingSPtr iMap);
    int  indexOf(IMappingSPtr iMap);
    bool remove(int idx);
    bool remove(IMappingSPtr iMap);
    bool replace(IMappingSPtr oldMap, IMappingSPtr newMap);
    void shuffle();
    void clearMappings();

private:
    TVector<IMappingSPtr>  m_mappings;
    TStringList            m_tags;
};

} // namespace Representation
} // namespace Tigon

#endif // ISET_H
