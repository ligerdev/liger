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
#include <tigon/Representation/Mappings/IMappingOperations.h>
#include <tigon/Representation/Elements/IElementOperations.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Mappings/IMapping.h>

namespace Tigon {
namespace Representation {

tribool operator< (const IMapping& m, const IMapping& c)
{
    //strong dominance relation
    if(!(m.isObjectiveVecEvaluated()) || !(c.isObjectiveVecEvaluated())) {
        return incomparable;
    }

    TVector<IElementSPtr> mo = m.objectiveVec();
    TVector<IElementSPtr> co = c.objectiveVec();

    if(mo.size() != co.size()) {
        return incomparable;
    }

    bool mDominate(false);
    bool cDominate(false);
    for(int i=0; i<mo.size(); i++) {
        if(*mo.at(i) == *co.at(i)) {
            return incomparable;
        } else if(*mo.at(i) < *co.at(i)) {
            if(cDominate) {
                return incomparable;
            }
            mDominate = true;
        } else {
            if(mDominate) {
                return incomparable;
            }
            cDominate = true;
        }
    }
    if(mDominate) {
        return true;
    } else if(cDominate) {
        return false;
    }

    // default - should never get this far.
    return incomparable;
}

tribool operator> (const IMapping& m, const IMapping& c)
{
    return c<m;
}

tribool operator<=(const IMapping& m, const IMapping& c)
{
    //weak dominance relation
    if(!(m.isObjectiveVecEvaluated()) || !(c.isObjectiveVecEvaluated())) {
        return incomparable;
    }

    TVector<IElementSPtr> mo = m.objectiveVec();
    TVector<IElementSPtr> co = c.objectiveVec();

    if(mo.size() != co.size()) {
        return incomparable;
    }

    bool mDominate(false);
    bool cDominate(false);
    for(int i=0; i<mo.size(); i++)
    {
        if(*mo.at(i) < *co.at(i)) {
            if(cDominate) {
                return incomparable;
            }
            mDominate = true;
        } else if(*co.at(i) < *mo.at(i)) {
            if(mDominate) {
                return incomparable;
            }
            cDominate = true;
        }
    }
    if(mDominate) {
        return true;
    } else if(cDominate) {
        return false;
    } else {
        return incomparable;
    }

    // default - should never get this far.
    return incomparable;
}

tribool operator>=(const IMapping& m, const IMapping& c)
{
    return c<=m;
}

tribool operator==(const IMapping& m, const IMapping& c)
{
    TVector<IElementSPtr> mo = m.objectiveVec();
    TVector<IElementSPtr> co = c.objectiveVec();

    TVector<IElementSPtr> md = m.decisionVec();
    TVector<IElementSPtr> cd = c.decisionVec();

    if( (mo.size() != co.size()) || (md.size() != cd.size())) {
        return incomparable;
    }

    if(!(m.isObjectiveVecEvaluated()) || !(c.isObjectiveVecEvaluated())) {
        if((m.isObjectiveVecEvaluated()) || (c.isObjectiveVecEvaluated())) {
            return incomparable;
        } else {
            for(int i=0; i<md.size(); i++) {
                if(*md.at(i) != *cd.at(i)) {
                    return false;
                }
            }
            return true;
        }
    }

    for(int i=0; i<mo.size(); i++) {
        if(*mo.at(i) != *co.at(i)) {
            return false;
        }
    }

    for(int i=0; i<md.size(); i++) {
        if(*md.at(i) != *cd.at(i)) {
            return false;
        }
    }
    return true;
}

tribool operator!=(const IMapping& m, const IMapping& c)
{
    TVector<IElementSPtr> mo = m.objectiveVec();

    TVector<IElementSPtr> co = c.objectiveVec();

    TVector<IElementSPtr> md = m.decisionVec();
    TVector<IElementSPtr> cd = c.decisionVec();

    if((mo.size() != co.size()) || (md.size() != cd.size())) {
        return incomparable;
    }

    if(!(m.isObjectiveVecEvaluated()) || !(c.isObjectiveVecEvaluated())) {
        if((m.isObjectiveVecEvaluated()) || (c.isObjectiveVecEvaluated())) {
            return incomparable;
        } else {
            for(int i=0; i<md.size(); i++) {
                if(*md.at(i) != *cd.at(i)) {
                    return true;
                }
            }
            return false;
        }
    }

    for(int i=0; i<mo.size(); i++) {
        if(*mo.at(i) != *co.at(i)) {
            return true;
        }
    }

    for(int i=0; i<md.size(); i++) {
        if(*md.at(i) != *cd.at(i)) {
            return true;
        }
    }
    return false;
}

} // namespace Representation
} // namespace Tigon
