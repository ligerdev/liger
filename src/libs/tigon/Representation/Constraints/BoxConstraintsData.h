/****************************************************************************
**
** Copyright (C) 2012-2022 The University of Sheffield (www.sheffield.ac.uk)
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
#ifndef BOXCONSTRAINTSDATA_H
#define BOXCONSTRAINTSDATA_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>

#include <tigon/Core/TVector.h>
#include <tigon/Representation/Properties/ElementProperties.h>

namespace Tigon {
namespace Representation {
class IElement;

class LIGER_TIGON_EXPORT BoxConstraintsData
{
public:
    BoxConstraintsData();
    BoxConstraintsData(const BoxConstraintsData& data);
    BoxConstraintsData(int size);
    BoxConstraintsData(const TVector<ElementType>&  t);
    BoxConstraintsData(const TVector<ElementProperties>&  t);
    ~BoxConstraintsData();
    BoxConstraintsDataSPtr clone();

    TVector<IElement> lowerBounds()            const;
    TVector<IElement> upperBounds()            const;
    IElement          lowerBound(int idx)      const;
    IElement          upperBound(int idx)      const;
    int               size()                   const;
    void  appendBounds(Tigon::ElementType t = Tigon::RealType);
    void  appendBounds(const IElement& lb, const IElement& ub);
    void  appendLowerBound(const    IElement&              lb);
    void  appendUpperBound(const    IElement&              ub);
    void  defineLowerBounds(const   TVector<IElement>&     lb);
    void  defineUpperBounds(const   TVector<IElement>&     ub);
    void  defineBounds(const   TVector<IElement>&     lb,
                       const   TVector<IElement>&     ub);
    // only defines the value
    void  defineLowerBound(int idx,        const IElement& lb);
    void  defineUpperBound(int idx,        const IElement& ub);
    // also defines the type
    void  replaceLowerBound(int idx,       const IElement& lb);
    void  replaceUpperBound(int idx,       const IElement& ub);
    void  clear();

private:
    TVector<IElement> m_lowerBounds;
    TVector<IElement> m_upperBounds;

};

} // namespace Representation
} // namespace Tigon

#endif // BOXCONSTRAINTSDATA_H
