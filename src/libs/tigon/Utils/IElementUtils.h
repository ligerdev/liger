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
#ifndef IELEMENTUTILS_H
#define IELEMENTUTILS_H

#include <tigon/tigon_global.h>
#include <tigon/tigonconstants.h>
#include <tigon/Representation/Elements/IElement.h>
#include <tigon/Representation/Properties/ElementProperties.h>

#include <tigon/Core/TVector.h>

namespace Tigon{
namespace Representation{
class ISet;
class ElementProperties;
}
}
using namespace Tigon::Representation;

namespace Tigon {

LIGER_TIGON_EXPORT
TVector<double> IElementVecToRealVec(const TVector<IElementSPtr>& ev);
LIGER_TIGON_EXPORT
TVector<double> IElementVecToRealVec(const TVector<IElement>&    ev);

LIGER_TIGON_EXPORT
TVector<IElement> copyIElementVecFromPointers(const TVector<IElementSPtr> &ev);

LIGER_TIGON_EXPORT void resetToMax(TVector<IElementSPtr> vec);
LIGER_TIGON_EXPORT void resetToMin(TVector<IElementSPtr> vec);
LIGER_TIGON_EXPORT void resetToMax(TVector<IElement>&    vec);
LIGER_TIGON_EXPORT void resetToMin(TVector<IElement>&    vec);

LIGER_TIGON_EXPORT TVector<IElementSPtr> cloneVector(TVector<IElementSPtr> vec);
LIGER_TIGON_EXPORT TVector<IElement>     copyVector(const TVector<IElement>& vec);

LIGER_TIGON_EXPORT
TVector<IElementSPtr> createIElementSPtrVector(TVector<ElementProperties>  prpts);
LIGER_TIGON_EXPORT
TVector<IElementSPtr> createIElementSPtrVector(TVector<Tigon::ElementType> types);
LIGER_TIGON_EXPORT
TVector<IElementSPtr> createIElementSPtrVector(int size, Tigon::ElementType type);
template <typename T>
TVector<IElementSPtr> createIElementSPtrVector(const TVector<T> &vals,
                                               const TVector<Tigon::ElementType> &types) {
    TVector<IElementSPtr> ret;
    ret.reserve(vals.size());
    std::transform(vals.begin(), vals.end(), types.begin(),
                   std::back_inserter(ret),
                   [](const T & val, const Tigon::ElementType &type) {
        return IElementSPtr(new IElement(type, val));
    });

    return ret;
}

LIGER_TIGON_EXPORT
TVector<IElement> createIElementVector(const TVector<ElementProperties>  &prpts);
LIGER_TIGON_EXPORT
TVector<IElement> createIElementVector(const TVector<ElementType> &types);
LIGER_TIGON_EXPORT
TVector<IElement> createIElementVector(int size, Tigon::ElementType type);

template <typename T>
TVector<IElement> createIElementVector(const TVector<T> &vals,
                                       const TVector<Tigon::ElementType> &types) {
    TVector<IElement> ret;
    std::transform(vals.begin(), vals.end(), types.begin(),
                   std::back_inserter(ret),
                   [](const T &val, Tigon::ElementType type) {
        return IElement(type, val);});
    return ret;
}

LIGER_TIGON_EXPORT void clearVector(TVector<IElementSPtr>& vec);

TVector<ElementProperties> createElemPropVec(const TVector<ElementType> &types,
                                             ProbNamespace::VectorType vecType);

} // namespace Tigon
#endif // IELEMENTUTILS_H
